#ifndef LOG_H
#define LOG_H

#include <constantes.h>

#if (TAM_MEMORIA_PRINCIPAL % 4) == 0

#define QUEBRA_LINHAS_MEMORIA_IMPRESSAO   (TAM_MEMORIA_PRINCIPAL / 4)

#elif (TAM_MEMORIA_PRINCIPAL < 4)

#define QUEBRA_LINHAS_MEMORIA_IMPRESSAO   TAM_MEMORIA_PRINCIPAL

#else

#define QUEBRA_LINHAS_MEMORIA_IMPRESSAO (TAM_MEMORIA_PRINCIPAL / (TAM_MEMORIA_PRINCIPAL % 4))

#endif

void imprimeMemoriaPrincipal(const Pagina *memoriaPrincipal);

void imprimeInformacoesProcesso(const Processo processo);

#endif