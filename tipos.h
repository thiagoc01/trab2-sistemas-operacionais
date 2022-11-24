#ifndef TIPOS_H
#define TIPOS_H

#define LIMITE_PAGINAS_PROCESSO 50

typedef struct Pagina
{
    int quadro;
    short int emMemoria;
    int tempoSemUso;
    int pid;
} Pagina;

typedef struct Processo
{
    int pid;
    Pagina tabela[LIMITE_PAGINAS_PROCESSO];
    short int paginasCarregadas;
    short int paginasTotais;
    int tempo;
} Processo;


#endif