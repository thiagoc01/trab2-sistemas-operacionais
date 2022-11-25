#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include "tipos.h"

#define TAM_MEMORIA_PRINCIPAL 64
#define NUM_PROCESSOS 20

Pagina memoriaPrincipal[TAM_MEMORIA_PRINCIPAL];
int posicaoLivre = 0;
Processo processos[NUM_PROCESSOS];
int processosCriados = 0;
int instante = 0;

void imprimeMemoriaPrincipal()
{
    puts("Memória principal: \n");

    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
       printf("%d ", memoriaPrincipal[i].pid);

    puts("\n");
}

void incrementaTempoProcessos()
{
    for (int i = 0 ; i < processosCriados ; i++)
    {
        if (processos[i].paginasCarregadas != 0)
            processos[i].tempo++;

        for (int j = 0 ; j < processos[i].paginasTotais ; j++)
        {
            if (processos[i].tabela[j].emMemoria)
                processos[i].tabela[j].tempoSemUso++;
        }
    }
}

void imprimeInformacoesProcesso(Processo processo)
{
    if (processo.paginasTotais)
    {
        printf("Tabela de páginas do processo %d\n", processo.pid);
        puts("================================================");
        for (int i = 0 ; i < processo.paginasTotais ; i++)
        {
            if (processo.tabela[i].emMemoria)
                printf("Página %d no espaço %d\n", i, processo.tabela[i].quadro);
            
            else
                printf("Página %d no disco\n", i);
        }
    }

    else
        printf("Processo %d ainda não possui páginas criadas.", processo.pid);            
    
    puts("\n");
}

void criaProcesso()
{
    Processo novo = {.pid = processosCriados + 1, .paginasCarregadas = 0, .paginasTotais = 0, .tempo = 0};
    processos[processosCriados++] = novo;
    printf("Processo %d criado.\n\n", novo.pid);
}

Processo *retornaProcessoMaisAntigo(int pidProcessoSolicitante)
{
    int maiorTempo = processos[0].tempo, i = 1;
    Processo *maisAntigo = &processos[0];

    if (pidProcessoSolicitante == 1)
    {
        maisAntigo = &processos[1];
        maiorTempo = processos[1].tempo;
        i = 2;
    }       

    for ( ; i < processosCriados ; i++)
    {
        if (processos[i].tempo > maiorTempo && processos[i].paginasCarregadas != 0 && maisAntigo->pid != i)
        {
            maisAntigo = &processos[i];
            maiorTempo = processos[i].tempo;
        }
    }

    return maisAntigo;
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

void realizaSwapGlobal(Processo **processo, Pagina *paraAlocar, const int *numPagina)
{
    Processo *maisAntigo = retornaProcessoMaisAntigo((*processo)->pid);
    int pagina;

    Pagina *menosUsadaRecentemente = retornaPaginaMenosUsadaRecentemente(maisAntigo, &pagina);

    int posicao = menosUsadaRecentemente->quadro;

    paraAlocar->emMemoria = 1;
    paraAlocar->tempoSemUso = 0;    
    paraAlocar->quadro = posicao;

    if (*numPagina == -1)
    {
        paraAlocar->pid = (*processo)->pid;
        printf("A página %d do processo %d foi colocada em disco para dar lugar à nova página %d do processo %d.\n\n", pagina, maisAntigo->pid, (*processo)->paginasTotais++, (*processo)->pid);
        (*processo)->tabela[(*processo)->paginasTotais - 1] = *paraAlocar;
    }

    else
    {
        printf("A página %d do processo %d foi colocada em disco para dar lugar à página %d do processo %d.\n\n", pagina, maisAntigo->pid, *numPagina, (*processo)->pid);
        (*processo)->tabela[*numPagina] = *paraAlocar;
    }     

    memoriaPrincipal[posicao] = *paraAlocar;
    (*processo)->paginasCarregadas++;

    menosUsadaRecentemente->emMemoria = 0;
    menosUsadaRecentemente->quadro = -1;
    menosUsadaRecentemente->tempoSemUso = -1;
    maisAntigo->paginasCarregadas--;

    if (maisAntigo->paginasCarregadas == 0)
    {
        printf("Processo %d está sem páginas na memória. O mesmo irá se bloquear.\n\n", maisAntigo->pid);
        maisAntigo->tempo = 0;
    }
}

void realizaSwapLocal(Processo **processo, Pagina *paraAlocar, const int *numPagina)
{
    int maiorTempo = -1, k = 0, pagina;
    Pagina *menosUsadaRecentemente = retornaPaginaMenosUsadaRecentemente(*processo, &pagina);

    paraAlocar->emMemoria = 1;
    paraAlocar->tempoSemUso = 0;
    paraAlocar->pid = (*processo)->pid;
    paraAlocar->quadro = menosUsadaRecentemente->quadro;    

    /* Troca as páginas na memória principal e faz o swap */

    if (*numPagina == -1)
    {
        printf("Página %d no endereço %d do processo %d foi trocada pela página %d.\n", pagina, menosUsadaRecentemente->quadro, (*processo)->pid, (*processo)->paginasTotais++);
        (*processo)->tabela[(*processo)->paginasTotais - 1] = *paraAlocar;
    }

    else
    {
        printf("Página %d no endereço %d do processo %d foi trocada pela página %d do mesmo processo.\n", pagina, menosUsadaRecentemente->quadro, (*processo)->pid, *numPagina);
        (*processo)->tabela[*numPagina] = *paraAlocar;
    }    
    
    memoriaPrincipal[menosUsadaRecentemente->quadro] = *paraAlocar;    

    menosUsadaRecentemente->emMemoria = 0;
    menosUsadaRecentemente->quadro = -1;
    menosUsadaRecentemente->tempoSemUso = -1;
}

void realizaAlocacaoPagina(Processo **processo, Pagina *paginaParaAlocacao, int *numPagina)
{
    if ((*processo)->paginasCarregadas == 4)    // Working set no limite, iremos tirar uma página que não esteja sendo usada
        realizaSwapLocal(processo, paginaParaAlocacao, numPagina);

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
            realizaSwapGlobal(processo, paginaParaAlocacao, numPagina);
    }
}

void solicitaAlocacaoPagina()
{            
    /* Seleciona um processo aleatório para alocar uma página de memória */
    int processoCriacaoMemoria = 1 + rand() % processosCriados;
    Processo *processo = &processos[processoCriacaoMemoria - 1];        

    imprimeInformacoesProcesso(*processo);

    Pagina paginaParaAlocacao;
    int numPagina = -1;

    if (processo->paginasTotais == LIMITE_PAGINAS_PROCESSO)
    {
        numPagina = rand() % processo->paginasTotais;

        while (processo->tabela[numPagina].emMemoria)
            numPagina = rand() % processo->paginasTotais;

        paginaParaAlocacao = processo->tabela[numPagina];
    }

    realizaAlocacaoPagina(&processo, &paginaParaAlocacao, &numPagina);     
}

void referenciaPaginaProcesso()
{
    Processo *processoAleatorio = &processos[rand() % processosCriados];

    while (processoAleatorio->paginasTotais == 0)
        processoAleatorio = &processos[rand() % processosCriados];

    int numPagina = rand() % processoAleatorio->paginasTotais;

    Pagina paginaAleatoria = processoAleatorio->tabela[numPagina];

    if (paginaAleatoria.emMemoria)
    {
        paginaAleatoria.tempoSemUso = 0;
        memoriaPrincipal[paginaAleatoria.quadro].tempoSemUso = 0;
        processoAleatorio->tabela[numPagina] = paginaAleatoria;

        printf("Página %d do processo %d no endereço %d foi referenciada.\n\n", numPagina, processoAleatorio->pid, paginaAleatoria.quadro);
    }

    else
    {
        printf("Página %d do processo %d foi referenciada, mas não está carregada em memória.\n", numPagina, processoAleatorio->pid);

        realizaAlocacaoPagina(&processoAleatorio, &paginaAleatoria, &numPagina);
    }
}

int main()
{
    srand(time(NULL));

    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
        memoriaPrincipal[i].pid = 0;

    while (processosCriados != NUM_PROCESSOS)
    {
        printf("\n\nInstante %d\n", instante);
        puts("================================================");

        imprimeMemoriaPrincipal();

        incrementaTempoProcessos();

        if (instante++ % 3 == 0)
        {
            criaProcesso();
            solicitaAlocacaoPagina();
        }

        else
            referenciaPaginaProcesso();

        sleep(1);
    }

    while (true)
    {
        printf("\n\nInstante %d\n", instante);
        puts("================================================");

        imprimeMemoriaPrincipal();

        incrementaTempoProcessos();

        if (instante++ % 3 == 0)
            solicitaAlocacaoPagina();

        else
            referenciaPaginaProcesso();

        sleep(1);
    }

    return 0;
}