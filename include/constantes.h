#ifndef CONSTANTES_H
#define CONSTANTES_H

/* Se não foi passado na compilação, define valores padrões. */

#ifndef LIMITE_PAGINAS_PROCESSO

#define LIMITE_PAGINAS_PROCESSO 10

#endif

#ifndef TAM_MEMORIA_PRINCIPAL

#define TAM_MEMORIA_PRINCIPAL 64

#endif

#ifndef NUM_PROCESSOS

#define NUM_PROCESSOS 20

#endif

/* Cores para output */

#define BMAG "\e[0;95m"
#define RED "\e[0;91m"
#define GRN "\e[0;92m"
#define YEL "\e[0;93m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;96m"
#define WHT "\e[0;37m"
#define COLOR_RESET "\e[0m"


#endif