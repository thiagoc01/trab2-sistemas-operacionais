#ifndef TIPOS_H
#define TIPOS_H

#include <constantes.h>

typedef struct Pagina
{
    int quadro;             // Posição em memória
    short int emMemoria;    // Está no disco ou na memória principal?
    int tempoSemUso;        // Quantidade de tempo sem ser referenciada naquele instante
    int pid;

} Pagina;

typedef struct Processo
{
    int pid;
    Pagina tabela[LIMITE_PAGINAS_PROCESSO];     // Tabela de páginas do processo
    short int paginasCarregadas;                // Páginas em memória principal
    short int paginasTotais;                    // Páginas em memória principal + páginas em disco
    int tempo;                                  // Tempo em memória naquele instante

} Processo;


#endif