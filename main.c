#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <tipos.h>
#include <processo.h>
#include <memoria.h>

Pagina memoriaPrincipal[TAM_MEMORIA_PRINCIPAL];
Processo processos[NUM_PROCESSOS];

int posicaoLivre = 0, processosCriados = 0, instante = 0;

int main()
{
    srand(time(NULL));

    inicializaMemoriaPrincipal(memoriaPrincipal);

    gerenciaMemoriaPrimeiraEtapa(memoriaPrincipal, processos, &processosCriados, &instante); // Inclui criação de processos
   
    gerenciaMemoria(memoriaPrincipal, processos, processosCriados, &instante);

    return 0;
}