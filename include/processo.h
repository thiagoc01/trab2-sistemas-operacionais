#ifndef PROCESSO_H
#define PROCESSO_H

#include <tipos.h>

void criaProcesso(Processo *processos, int *processosCriados);

/*
    Adiciona tempo em memória para todos os processos carregados e
    adiciona tempo para todas as páginas carregadas.
*/
void incrementaTempoProcessos(Processo *processos, const int processosCriados);

Processo *retornaProcessoMaisAntigo(Processo *processos, int pidProcessoSolicitante, int processosCriados);

#endif