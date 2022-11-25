#include <tipos.h>
#include <log.h>

#include <stdio.h>

void imprimeMemoriaPrincipal(const Pagina *memoriaPrincipal)
{
    puts("Memória principal: \n");
    
    for (int i = 0 ; i < TAM_MEMORIA_PRINCIPAL ; i++)
       printf("%d ", memoriaPrincipal[i].pid);

    puts("\n");
}

void imprimeInformacoesProcesso(const Processo processo)
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