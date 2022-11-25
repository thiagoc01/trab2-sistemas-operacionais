CC = gcc
DEFINES =
TARGET = gerenciador_memoria
FLAGS_TARGET = -o $(TARGET) -Wall -I include/ $(DEFINES)
FLAGS_AUX = -c -Wall -I include/ $(DEFINES)
OBJS = log.o processo.o memoria.o main.o

ifdef LIMITE_PAGINAS_PROCESSO

ifneq ($(LIMITE_PAGINAS_PROCESSO), 0)		# Impede quantidade negativa e nula
	qtdAbsoluta = $(shell if [ $(LIMITE_PAGINAS_PROCESSO) -lt 0 ]; then absoluto=$$(( $(LIMITE_PAGINAS_PROCESSO) * -1)); echo $$absoluto; else echo "$(LIMITE_PAGINAS_PROCESSO)"; fi)
	
	DEFINES += -D LIMITE_PAGINAS_PROCESSO=$(qtdAbsoluta)
endif	

endif

ifdef TAM_MEMORIA_PRINCIPAL

ifneq ($(TAM_MEMORIA_PRINCIPAL), 0)		# Impede tamanho negativo e nulo
	tamAbsoluto = $(shell if [ $(TAM_MEMORIA_PRINCIPAL) -lt 0 ]; then absoluto=$$(( $(TAM_MEMORIA_PRINCIPAL) * -1)); echo $$absoluto; else echo "$(TAM_MEMORIA_PRINCIPAL)"; fi)
	
	DEFINES += -D TAM_MEMORIA_PRINCIPAL=$(tamAbsoluto)
endif

endif

ifdef NUM_PROCESSOS

ifneq ($(NUM_PROCESSOS), 0)		# Impede quantidade negativa e nula
	qtdAbsoluta = $(shell if [ $(NUM_PROCESSOS) -lt 0 ]; then absoluto=$$(( $(NUM_PROCESSOS) * -1)); echo $$absoluto; else echo "$(NUM_PROCESSOS)"; fi)

	DEFINES += -D NUM_PROCESSOS=$(qtdAbsoluta)
endif

endif

all: main clean	

main: log gerenciamento main.c
	$(CC) $(FLAGS_AUX) main.c
	$(CC) $(OBJS) $(FLAGS_TARGET)

gerenciamento: gerenciamento/processo.c gerenciamento/memoria.c
	$(CC) $(FLAGS_AUX) gerenciamento/processo.c
	$(CC) $(FLAGS_AUX) gerenciamento/memoria.c

log: util/log.c
	$(CC) $(FLAGS_AUX) util/log.c

clean:
	rm -f *.o

