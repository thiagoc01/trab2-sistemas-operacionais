#ifndef MEMORIA_H
#define MEMORIA_H

#include <tipos.h>

void inicializaMemoriaPrincipal(Pagina *memoriaPrincipal);

void gerenciaMemoriaPrimeiraEtapa(Pagina *memoriaPrincipal, Processo *processos, int *processosCriados, int *instante);

void gerenciaMemoria(Pagina *memoriaPrincipal, Processo *processos, int processosCriados, int *instante);

Pagina *retornaPaginaMenosUsadaRecentemente(Processo *processo, int *pagina);

/*
    Realiza swap entre um processo qualquer e outro processo.
    Caso o processo não tenha esgotado o working set, deve-se
    retirar uma página de outro processo, seguindo a política LRU.
    O processo que irá perder a página é o mais antigo até o momento do swap.
*/

void realizaSwapGlobal(Pagina *memoriaPrincipal, Processo *processos, Processo **processo, Pagina *paraAlocar, const int *numPagina, int processosCriados);

/*
    Swap realizado no caso em que o processo já está no limite do working set.
*/

void realizaSwapLocal(Pagina *memoriaPrincipal, Processo **processo, Pagina *paraAlocar, const int *numPagina);

void realizaAlocacaoPagina(Pagina *memoriaPrincipal, Processo *processos, Processo **processo, Pagina *paginaParaAlocacao, int *numPagina, int processosCriados);

void solicitaAlocacaoPagina(Pagina *memoriaPrincipal, Processo *processos, int processosCriados);

/*
    Simula o momento que o processo procura uma página já criada
*/

void referenciaPaginaProcesso(Pagina *memoriaPrincipal, Processo *processos, int processosCriados);

#endif