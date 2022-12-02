#include <stdio.h>

#include <processo.h>


void criaProcesso(Processo *processos, int *processosCriados)
{
    Processo novo = {.pid = *processosCriados + 1, .paginasCarregadas = 0, .paginasTotais = 0, .tempo = 0};
    processos[(*processosCriados)++] = novo;
    printf(BMAG "Processo %d criado.\n\n" COLOR_RESET, novo.pid);
}

void incrementaTempoProcessos(Processo *processos, const int processosCriados)
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

Processo *retornaProcessoMaisAntigo(Processo *processos, int pidProcessoSolicitante, int processosCriados)
{
    int maiorTempo = processos[0].tempo;
    Processo *maisAntigo = &processos[0];

    for (int j = 0 ; j < processosCriados ; j++)
    {
        if (processos[j].paginasCarregadas && processos[j].pid != pidProcessoSolicitante)
        {
            maiorTempo = processos[j].tempo;
            maisAntigo = &processos[j];
            break;      // Encontramos um processo em memória
        }
    }

    for (int i = 0 ; i < processosCriados ; i++)
    {
        if (processos[i].tempo > maiorTempo && processos[i].paginasCarregadas != 0 && processos[i].pid != pidProcessoSolicitante)
        {
            maisAntigo = &processos[i];
            maiorTempo = processos[i].tempo;
        }
    }

    return maisAntigo;
}