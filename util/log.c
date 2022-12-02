#include <tipos.h>
#include <log.h>

#include <stdio.h>

void imprimeMemoriaPrincipal(const Pagina *memoriaPrincipal)
{
    puts(YEL "Memória principal: \n");
    
    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
    {
        for (int j = 0 ; j < QUEBRA_LINHAS_MEMORIA_IMPRESSAO ; i++, j++)
            printf("%d ", memoriaPrincipal[i].pid);

        i--;
        puts("");
    }

    puts("\n" COLOR_RESET);
}

void imprimeInformacoesProcesso(const Processo processo)
{
    if (processo.paginasTotais)
    {
        printf(CYN "Tabela de páginas do processo %d\n", processo.pid);
        puts("================================================");
        for (int i = 0 ; i < processo.paginasTotais ; i++)
        {
            if (processo.tabela[i].emMemoria)
                printf("Página %d no espaço %d (tempo sem alteração: %d)\n", i, processo.tabela[i].quadro, processo.tabela[i].tempoSemUso);
            
            else
                printf("Página %d no disco\n", i);
        }
    }

    else
        printf(CYN "Processo %d ainda não possui páginas criadas.", processo.pid);            
    
    puts("\n" COLOR_RESET);
}