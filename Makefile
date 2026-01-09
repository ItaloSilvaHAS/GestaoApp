# Makefile para Sistema de Gestão - Serralheria
# Versão com Interface Gráfica GTK

# Verificar qual versão do GTK está disponível
GTK_VERSION := $(shell pkg-config --exists gtk+-3.0 && echo "gtk+-3.0" || echo "")

ifeq ($(GTK_VERSION),)
    GTK_VERSION := $(shell pkg-config --exists gtk4 && echo "gtk4" || echo "")
endif

ifeq ($(GTK_VERSION),)
    $(error GTK não encontrado! Por favor, instale: sudo apt-get install libgtk-3-dev ou sudo apt-get install libgtk-4-dev)
endif

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs $(GTK_VERSION)`

# Alvos
TARGET_CLI = Gestao
TARGET_GUI = GestaoGUI

# Arquivos fonte
SRC_CLI = Gestao.c
SRC_GUI = GestaoGUI.c

# Compilar versão CLI (terminal)
$(TARGET_CLI): $(SRC_CLI)
	$(CC) $(CFLAGS) -o $(TARGET_CLI) $(SRC_CLI)

# Compilar versão GUI (interface gráfica)
$(TARGET_GUI): $(SRC_GUI)
	$(CC) $(CFLAGS) -o $(TARGET_GUI) $(SRC_GUI) $(LDFLAGS)

# Compilar ambas
all: $(TARGET_CLI) $(TARGET_GUI)

# Limpar arquivos compilados
clean:
	rm -f $(TARGET_CLI) $(TARGET_GUI)

# Instalar dependências (Ubuntu/Debian)
install-deps:
	@echo "Instalando dependências GTK..."
	sudo apt-get update
	sudo apt-get install -y libgtk-3-dev pkg-config build-essential

# Ajuda
help:
	@echo "Makefile para Sistema de Gestão - Serralheria"
	@echo ""
	@echo "Alvos disponíveis:"
	@echo "  make            - Compila ambas versões (CLI e GUI)"
	@echo "  make Gestao     - Compila apenas versão terminal"
	@echo "  make GestaoGUI  - Compila apenas versão gráfica"
	@echo "  make clean      - Remove arquivos compilados"
	@echo "  make install-deps - Instala dependências GTK"
	@echo "  make help       - Mostra esta mensagem"
	@echo ""
	@echo "Versão GTK detectada: $(GTK_VERSION)"

.PHONY: all clean install-deps help
