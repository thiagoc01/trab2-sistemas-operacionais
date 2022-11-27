# Trabalho 2 | Sistemas Operacionais 2022/2


##### Repositório contendo o gerenciador de memória com critério de realocação de página LRU.


&nbsp;

## Como funciona?

O gerenciador atua com base num algoritmo padrão de alocação de páginas, onde o critério de reposição ou retirada é o _least recently used_ (LRU).

O programa atua de forma automática ao ser executado. A cada 3 segundos, um processo é criado (até o limite) e outro solicita a alocação de uma página. Em instantes que não são múltiplos de 3, há uma referência de uma página. O _working set_ dos processos é de tamanho 4. A cada instante, é impressa a memória principal. Se um processo irá realizar a alocação de memória, sua tabela de páginas é impressa, indicando o tempo da última modificação e a localização da mesma. Logo abaixo, é mostrado o que ocorreu durante a alocação. Se foi necessário realizar _swap_, se a página foi alocada diretamente etc. O programa roda indefinidamente até ser parado.

As constantes que controlam o tamanho da memória principal, o número de processos e o limite de páginas para cada processo podem ser modificadas na compilação.

## Como compilar?

##### Baixe o código ou clone o repositório com:

```
$ git clone https://github.com/thiagoc01/trab2-sistemas-operacionais.git
```

##### Formas de compilação:

Se você deseja executar o programa com as constantes padrões, digite:

```
$ make
```

Caso deseje alterar qualquer uma, digite o nome da constante e o valor. Por exemplo:

```
$ make TAM_MEMORIA_PRINCIPAL=128
```

As constantes disponíveis são:

- TAM_MEMORIA_PRINCIPAL: número de _frames_ na memória principal;
- NUM_PROCESSOS: quantidade de processos que serão criados;
- LIMITE_PAGINAS_PROCESSO: quantidade de páginas quem podem ser criadas por um processo.

Todas as constantes são maiores que 0. Caso você digite um valor menor ou igual a 0, os valores não serão modificados na compilação.

Para alterar o nome do executável, altere a variável TARGET para o nome desejado. Por padrão, o executável tem o nome de gerenciador_memoria.

##### Execute o programa com:

```
$ ./gerenciador_memoria
```