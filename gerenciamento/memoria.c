#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <memoria.h>
#include <processo.h>
#include <log.h>

extern int posicaoLivre; // Posição livre atual na memória. Aumenta em ordem crescente.

void inicializaMemoriaPrincipal(Pagina *memoriaPrincipal)
{
    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
        memoriaPrincipal[i].pid = 0;
}

void gerenciaMemoriaPrimeiraEtapa(Pagina *memoriaPrincipal, Processo *processos, int *processosCriados, int *instante)
{
    while (*processosCriados != NUM_PROCESSOS)
    {
        printf("\n\nInstante %d\n", *instante);
        puts("================================================");

        imprimeMemoriaPrincipal(memoriaPrincipal);

        incrementaTempoProcessos(processos, *processosCriados);

        if ((*instante)++ % 3 == 0)
        {
            criaProcesso(processos, processosCriados);
            solicitaAlocacaoPagina(memoriaPrincipal, processos, *processosCriados);
        }

        else
            referenciaPaginaProcesso(memoriaPrincipal, processos, *processosCriados);

        sleep(1);
    }
}

void gerenciaMemoria(Pagina *memoriaPrincipal, Processo *processos, int processosCriados, int *instante)
{
    while (true)
    {
        printf("\n\nInstante %d\n", *instante);
        puts("================================================");

        imprimeMemoriaPrincipal(memoriaPrincipal);

        incrementaTempoProcessos(processos, processosCriados);

        if ((*instante)++ % 3 == 0)
            solicitaAlocacaoPagina(memoriaPrincipal, processos, processosCriados);

        else
            referenciaPaginaProcesso(memoriaPrincipal, processos, processosCriados);

        sleep(1);
    }
}

Pagina *retornaPaginaMenosUsadaRecentemente(Processo *processo, int *pagina)
{
    int maiorTempoPagina = -1, k = 0;
    Pagina *menosUsadaRecentemente;

    for ( ; k < processo->paginasTotais ; k++)   // Procura a página menos usada recentemente
    {
        if (processo->tabela[k].emMemoria && processo->tabela[k].tempoSemUso > maiorTempoPagina)
        {
            maiorTempoPagina = processo->tabela[k].tempoSemUso;
            menosUsadaRecentemente = &processo->tabela[k];
            *pagina = k;
        } 
    }

    return menosUsadaRecentemente;
}

void realizaSwapGlobal(Pagina *memoriaPrincipal, Processo *processos, Processo **processo, Pagina *paraAlocar,
                        const int *numPagina, int processosCriados)
{
    Processo *maisAntigo = retornaProcessoMaisAntigo(processos, (*processo)->pid, processosCriados);
    int pagina;

    Pagina *menosUsadaRecentemente = retornaPaginaMenosUsadaRecentemente(maisAntigo, &pagina);

    int posicao = menosUsadaRecentemente->quadro;

    /* Inicializa a página. */

    paraAlocar->emMemoria = 1;
    paraAlocar->tempoSemUso = 0;    
    paraAlocar->quadro = posicao;

    if (*numPagina == -1)   // É uma página nova. O processo não atingiu o limite de páginas virtuais.
    {
        paraAlocar->pid = (*processo)->pid;
        printf("A página %d do processo %d foi colocada em disco para dar lugar à nova página %d do processo %d.\n\n", pagina, maisAntigo->pid, (*processo)->paginasTotais++, (*processo)->pid);
        (*processo)->tabela[(*processo)->paginasTotais - 1] = *paraAlocar;
    }

    else // Processo já atingiu o limite de páginas virtuais e/ou está referenciando uma página do disco.
    {
        printf("A página %d do processo %d foi colocada em disco para dar lugar à página %d do processo %d.\n\n", pagina, maisAntigo->pid, *numPagina, (*processo)->pid);
        (*processo)->tabela[*numPagina] = *paraAlocar;
    }     

    memoriaPrincipal[posicao] = *paraAlocar;
    (*processo)->paginasCarregadas++;

    /* Coloca a página do processo mais antigo no disco. */

    menosUsadaRecentemente->emMemoria = 0;
    menosUsadaRecentemente->quadro = -1;
    menosUsadaRecentemente->tempoSemUso = -1;
    maisAntigo->paginasCarregadas--;

    if (maisAntigo->paginasCarregadas == 0) // Não há páginas na memória, ele não está carregado.
    {
        printf("Processo %d está sem páginas na memória. O mesmo irá se bloquear.\n\n", maisAntigo->pid);
        maisAntigo->tempo = 0;
    }
}

void realizaSwapLocal(Pagina *memoriaPrincipal, Processo **processo, Pagina *paraAlocar, const int *numPagina)
{
    int pagina;
    Pagina *menosUsadaRecentemente = retornaPaginaMenosUsadaRecentemente(*processo, &pagina);

    /* Inicializa a página. */

    paraAlocar->emMemoria = 1;
    paraAlocar->tempoSemUso = 0;
    paraAlocar->pid = (*processo)->pid;
    paraAlocar->quadro = menosUsadaRecentemente->quadro;    

    /* Troca as páginas na memória principal e faz o swap */

    if (*numPagina == -1)   // A página a ser colocada foi criada.
    {
        printf("Página %d no endereço %d do processo %d foi trocada pela nova página %d.\n", pagina, menosUsadaRecentemente->quadro, (*processo)->pid, (*processo)->paginasTotais++);
        (*processo)->tabela[(*processo)->paginasTotais - 1] = *paraAlocar;
    }

    else    // É uma página que já foi criada e estava no disco.
    {
        printf("Página %d no endereço %d do processo %d foi trocada pela página %d do mesmo processo.\n", pagina, menosUsadaRecentemente->quadro, (*processo)->pid, *numPagina);
        (*processo)->tabela[*numPagina] = *paraAlocar;
    }    
    
    memoriaPrincipal[menosUsadaRecentemente->quadro] = *paraAlocar;    

    /* Coloca a página no disco. */
    menosUsadaRecentemente->emMemoria = 0;
    menosUsadaRecentemente->quadro = -1;
    menosUsadaRecentemente->tempoSemUso = -1;
}

void realizaAlocacaoPagina(Pagina *memoriaPrincipal, Processo *processos, Processo **processo, Pagina *paginaParaAlocacao, int *numPagina, int processosCriados)
{
    if ((*processo)->paginasCarregadas == 4)    // Working set no limite, iremos tirar uma página que não esteja sendo usada
        realizaSwapLocal(memoriaPrincipal, processo, paginaParaAlocacao, numPagina);

    else  // O processo não atingiu o working set máximo
    {
        if (posicaoLivre != TAM_MEMORIA_PRINCIPAL)
        {
            paginaParaAlocacao->emMemoria = 1;
            paginaParaAlocacao->tempoSemUso = 0;
            paginaParaAlocacao->pid = (*processo)->pid;
            paginaParaAlocacao->quadro = posicaoLivre;
            
            memoriaPrincipal[posicaoLivre] = *paginaParaAlocacao;

            printf("Página %d do processo %d alocada na posição %d.\n", (*processo)->paginasTotais, (*processo)->pid, posicaoLivre++);
            (*processo)->tabela[(*processo)->paginasTotais] = *paginaParaAlocacao;
            (*processo)->paginasCarregadas++;
            (*processo)->paginasTotais++;
        }

        else // Remove a página mais antiga do processo mais antigo
            realizaSwapGlobal(memoriaPrincipal, processos, processo, paginaParaAlocacao, numPagina, processosCriados);
    }
}

void solicitaAlocacaoPagina(Pagina *memoriaPrincipal, Processo *processos, int processosCriados)
{            
    /* Seleciona um processo aleatório para alocar uma página de memória */

    int processoCriacaoMemoria = 1 + rand() % processosCriados;
    Processo *processo = &processos[processoCriacaoMemoria - 1];        

    imprimeInformacoesProcesso(*processo);

    /* Assume-se que o processo não atingiu o limite de páginas virtuais. */
    Pagina paginaParaAlocacao;
    int numPagina = -1;

    if (processo->paginasTotais == LIMITE_PAGINAS_PROCESSO) // Se atingiu, irá selecionar uma página aleatória no disco.
    {
        numPagina = rand() % processo->paginasTotais;

        while (processo->tabela[numPagina].emMemoria)
            numPagina = rand() % processo->paginasTotais;

        paginaParaAlocacao = processo->tabela[numPagina];
    }

    realizaAlocacaoPagina(memoriaPrincipal, processos, &processo, &paginaParaAlocacao, &numPagina, processosCriados);     
}

void referenciaPaginaProcesso(Pagina *memoriaPrincipal, Processo *processos, int processosCriados)
{
    Processo *processoAleatorio = &processos[rand() % processosCriados];

    while (processoAleatorio->paginasTotais == 0)
        processoAleatorio = &processos[rand() % processosCriados];

    int numPagina = rand() % processoAleatorio->paginasTotais;

    Pagina paginaAleatoria = processoAleatorio->tabela[numPagina];

    if (paginaAleatoria.emMemoria)  // Já está carregada, basta mudar o timestamp.
    {
        paginaAleatoria.tempoSemUso = 0;
        memoriaPrincipal[paginaAleatoria.quadro].tempoSemUso = 0;
        processoAleatorio->tabela[numPagina] = paginaAleatoria;

        printf("Página %d do processo %d no endereço %d foi referenciada.\n\n", numPagina, processoAleatorio->pid, paginaAleatoria.quadro);
    }

    else    // Page fault, iremos buscar a página no disco e trazer para a memória principal.
    {
        printf("Página %d do processo %d foi referenciada, mas não está carregada em memória.\n", numPagina, processoAleatorio->pid);

        realizaAlocacaoPagina(memoriaPrincipal, processos, &processoAleatorio, &paginaAleatoria, &numPagina, processosCriados);
    }
}