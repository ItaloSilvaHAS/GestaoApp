#!/bin/bash
# Script de instalação para Sistema de Gestão - Serralheria

echo "╔════════════════════════════════════════════╗"
echo "║  Instalação Sistema de Gestão - Serralheria ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# Verificar se é root (necessário para instalar pacotes)
if [ "$EUID" -ne 0 ]; then 
    echo "⚠️  Executando instalação de dependências..."
    echo "   (Será solicitada sua senha)"
    echo ""
fi

# Verificar se GTK está instalado
if ! pkg-config --exists gtk+-3.0 2>/dev/null && ! pkg-config --exists gtk4 2>/dev/null; then
    echo "📦 Instalando dependências GTK..."
    
    # Detectar distribuição
    if [ -f /etc/debian_version ]; then
        sudo apt-get update
        sudo apt-get install -y libgtk-3-dev pkg-config build-essential
    elif [ -f /etc/redhat-release ]; then
        sudo yum install -y gtk3-devel pkgconfig gcc
    elif [ -f /etc/arch-release ]; then
        sudo pacman -S --noconfirm gtk3 pkgconfig base-devel
    else
        echo "❌ Distribuição não suportada automaticamente."
        echo "   Por favor, instale manualmente: libgtk-3-dev (ou equivalente)"
        exit 1
    fi
else
    echo "✓ GTK já está instalado!"
fi

echo ""
echo "🔨 Compilando o programa..."

# Compilar
if make GestaoGUI 2>&1; then
    echo ""
    echo "╔════════════════════════════════════════════╗"
    echo "║         ✓ Instalação concluída!            ║"
    echo "╚════════════════════════════════════════════╝"
    echo ""
    echo "Para executar o programa, use:"
    echo "  ./GestaoGUI"
    echo ""
    echo "Ou execute:"
    echo "  make GestaoGUI && ./GestaoGUI"
    echo ""
else
    echo ""
    echo "❌ Erro na compilação!"
    echo "Verifique as mensagens acima."
    exit 1
fi
