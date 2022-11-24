#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
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

void imprimeProcessos()
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

int main()
{
    srand(time(NULL));

    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
        memoriaPrincipal[i].pid = 0;

    while (1)
    {
        printf("\n\nInstante %d\n", instante);
        puts("================================================");

        imprimeMemoriaPrincipal();

        imprimeProcessos();

        if (instante++ % 3 == 0)
        {
            if (processosCriados != NUM_PROCESSOS)
                criaProcesso();
            
            /* Seleciona um processo aleatório para alocar uma página de memória */
            int processoCriacaoMemoria = 1 + rand() % processosCriados;
            Processo *processo = &processos[processoCriacaoMemoria - 1];        

            imprimeInformacoesProcesso(*processo);

            if (processo->paginasTotais < LIMITE_PAGINAS_PROCESSO)   // Há possibilidade de criar uma nova página
            {
                Pagina nova;
                nova.emMemoria = 1;
                nova.tempoSemUso = 0;
                nova.pid = processo->pid;

                if (processo->paginasCarregadas == 4)    // Working set no limite, iremos tirar uma página que não esteja sendo usada
                {
                    int maiorTempo = -1, k = 0, posicao, pagina;
                    Pagina *menosUsadaRecentemente;

                    for ( ; k < processo->paginasTotais ; k++)   // Procura a página menos usada recentemente
                    {
                        if (processo->tabela[k].emMemoria && processo->tabela[k].tempoSemUso > maiorTempo)
                        {
                            maiorTempo = processo->tabela[k].tempoSemUso;
                            menosUsadaRecentemente = &processo->tabela[k];
                            pagina = k;
                        } 
                    }

                    /* Troca as páginas na memória principal e faz o swap */
                    printf("Página %d no endereço %d do processo %d foi trocada pela página %d.\n", pagina, menosUsadaRecentemente->quadro, processo->pid, processo->paginasTotais++);
                    posicao = menosUsadaRecentemente->quadro;
                    nova.quadro = posicao;
                    processo->tabela[processo->paginasTotais - 1] = nova;
                    memoriaPrincipal[posicao] = nova;

                    menosUsadaRecentemente->emMemoria = 0;
                    menosUsadaRecentemente->quadro = -1;
                    menosUsadaRecentemente->tempoSemUso = -1;
                }

                else  // O processo não atingiu o working set máximo
                {
                    if (posicaoLivre != TAM_MEMORIA_PRINCIPAL)
                    {
                        nova.quadro = posicaoLivre;
                        
                        memoriaPrincipal[posicaoLivre] = nova;

                        printf("Página %d do processo %d alocada na posição %d.\n", processo->paginasTotais, processo->pid, posicaoLivre++);
                        processo->tabela[processo->paginasTotais] = nova;
                        processo->paginasCarregadas++;
                        processo->paginasTotais++;
                    }

                    else // Remove a página mais antiga do processo mais antigo
                    {
                        int maiorTempo = -1;
                        Processo *maisAntigo;

                        for (int i = 0 ; i < processosCriados ; i++)
                        {
                            if (processos[i].tempo > maiorTempo && processos[i].paginasCarregadas != 0)
                            {
                                maisAntigo = &processos[i];
                                maiorTempo = processos[i].tempo;
                            }
                        }

                        int maiorTempoPagina = -1, k = 0, posicao, pagina;
                        Pagina *menosUsadaRecentemente;

                        for ( ; k < maisAntigo->paginasTotais ; k++)   // Procura a página menos usada recentemente
                        {
                            if (maisAntigo->tabela[k].emMemoria && maisAntigo->tabela[k].tempoSemUso > maiorTempoPagina)
                            {
                                maiorTempoPagina = maisAntigo->tabela[k].tempoSemUso;
                                menosUsadaRecentemente = &maisAntigo->tabela[k];
                                pagina = k;
                            } 
                        }

                        printf("A página %d do processo %d foi colocada em disco para dar lugar à nova página %d do processo %d.\n\n", pagina, maisAntigo->pid, processo->paginasTotais++, processo->pid);

                        posicao = menosUsadaRecentemente->quadro;
                        nova.quadro = posicao;
                        processo->tabela[processo->paginasTotais - 1] = nova;
                        memoriaPrincipal[posicao] = nova;
                        processo->paginasCarregadas++;

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
                }
                
            }

            else    // Já chegou no limite, iremos fazer swap entre uma página no disco e outra na memória ou retirar uma página de um processo
            {
                Pagina *paginaDisco;
                int paginaAleatoriaDisco = rand() % processo->paginasTotais;

                while (processo->tabela[paginaAleatoriaDisco].emMemoria)
                    paginaAleatoriaDisco = rand() % processo->paginasTotais;

                paginaDisco = &processo->tabela[paginaAleatoriaDisco];

                if (processo->paginasCarregadas == 4)
                {
                    int maiorTempo = -1, k = 0, pagina;
                    Pagina *menosUsadaRecentemente;

                    for ( ; k < processo->paginasTotais ; k++)   // Procura a página menos usada recentemente
                    {
                        if (processo->tabela[k].emMemoria && processo->tabela[k].tempoSemUso > maiorTempo)
                        {
                            maiorTempo = processo->tabela[k].tempoSemUso;
                            menosUsadaRecentemente = &processo->tabela[k];
                            pagina = k;
                        } 
                    }                   

                    paginaDisco->emMemoria = 1;
                    paginaDisco->quadro = menosUsadaRecentemente->quadro;
                    paginaDisco->tempoSemUso = 0;

                    memoriaPrincipal[menosUsadaRecentemente->quadro] = *paginaDisco; 

                    printf("Página %d do processo %d foi trocada pela página %d do mesmo processo.\n", pagina, processo->pid, paginaAleatoriaDisco);                   

                    menosUsadaRecentemente->emMemoria = 0;
                    menosUsadaRecentemente->quadro = -1;
                    menosUsadaRecentemente->tempoSemUso = -1;
                }

                else // Iremos retirar a página de um processo
                {
                    int maiorTempo = -1;
                    Processo *maisAntigo;

                    for (int i = 0 ; i < processosCriados ; i++)
                    {
                        if (processos[i].tempo > maiorTempo && processos[i].paginasCarregadas != 0)
                        {
                            maisAntigo = &processos[i];
                            maiorTempo = processos[i].tempo;
                        }
                    }

                    int maiorTempoPagina = -1, k = 0, posicao, pagina;
                    Pagina *menosUsadaRecentemente;

                    for ( ; k < maisAntigo->paginasTotais ; k++)   // Procura a página menos usada recentemente
                    {
                        if (maisAntigo->tabela[k].emMemoria && maisAntigo->tabela[k].tempoSemUso > maiorTempoPagina)
                        {
                            maiorTempoPagina = maisAntigo->tabela[k].tempoSemUso;
                            menosUsadaRecentemente = &maisAntigo->tabela[k];
                            pagina = k;
                        } 
                    }

                    printf("A página %d do processo %d foi colocada em disco para dar lugar à nova página %d do processo %d.\n\n", pagina, maisAntigo->pid, paginaAleatoriaDisco, processo->pid);

                    posicao = menosUsadaRecentemente->quadro;
                    
                    paginaDisco->emMemoria = 1;
                    paginaDisco->quadro = posicao;
                    paginaDisco->tempoSemUso = 0;

                    memoriaPrincipal[posicao] = *paginaDisco;
                    processo->paginasCarregadas++;

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
            }       
            
        }

        else // Apenas referencia uma página na memória
        {
            int paginaAleatoria = rand() % TAM_MEMORIA_PRINCIPAL;

            while (memoriaPrincipal[paginaAleatoria].pid == 0)
                paginaAleatoria = rand() % TAM_MEMORIA_PRINCIPAL;

            Processo *donoPagina = &processos[memoriaPrincipal[paginaAleatoria].pid - 1];

            for (int i = 0 ; i < donoPagina->paginasTotais ; i++)
            {
                if (donoPagina->tabela[i].quadro == paginaAleatoria)
                {
                    Pagina *referenciada = &donoPagina->tabela[i];

                    referenciada->tempoSemUso = 0;

                    memoriaPrincipal[paginaAleatoria] = *referenciada;

                    printf("Página %d do processo %d foi referenciada.\n\n", i, donoPagina->pid);

                    break;
                }
            }
        }

        //sleep(1);
    }

    return 0;
}