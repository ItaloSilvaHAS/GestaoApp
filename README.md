# Sistema de Gestão - Lojas

Sistema completo de gestão para lojas com interface gráfica (GUI) desenvolvido em C com GTK+.

## 📋 Funcionalidades

### Gestão de Produtos
- ✅ Cadastrar produtos (nome, descrição, preço, estoque)
- ✅ Listar todos os produtos cadastrados
- ✅ Buscar produtos por ID
- ✅ Controle automático de estoque

### Gestão de Clientes
- ✅ Cadastrar clientes (nome, telefone, endereço)
- ✅ Listar todos os clientes cadastrados
- ✅ Buscar clientes por ID

### Sistema de Vendas
- ✅ Nova venda (atendimento ao cliente)
- ✅ Adicionar múltiplos produtos à venda
- ✅ Cálculo automático de totais
- ✅ Atualização automática de estoque
- ✅ Listar todas as vendas realizadas
- ✅ Visualizar status das vendas (Vendido/Devolvido)

### Devoluções
- ✅ Processar devoluções completas
- ✅ Reverter estoque automaticamente
- ✅ Marcar venda como devolvida

## 🚀 Instalação

### 1. Instalar dependências (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y libgtk-3-dev pkg-config build-essential
```

### 2. Compilar o programa

#### Opção A: Usando o Makefile (Recomendado)
```bash
make GestaoGUI
```

#### Opção B: Compilação manual
```bash
gcc -Wall -Wextra -std=c11 -o GestaoGUI GestaoGUI.c `pkg-config --cflags --libs gtk+-3.0`
```

### 3. Executar o programa

```bash
./GestaoGUI
```

## 📦 Versões Disponíveis

O projeto inclui duas versões:

1. **Gestao.c** - Versão para terminal (CLI)
   - Compilar: `make Gestao` ou `gcc -o Gestao Gestao.c`
   - Executar: `./Gestao`

2. **GestaoGUI.c** - Versão com interface gráfica (GUI)
   - Compilar: `make GestaoGUI`
   - Executar: `./GestaoGUI`

## 🖥️ Interface Gráfica

A interface gráfica utiliza GTK+ e possui:

- **Abas organizadas** para Produtos, Clientes e Vendas
- **Tabelas** para visualização de dados
- **Diálogos modais** para cadastros e operações
- **Validações** automáticas de dados
- **Mensagens** informativas de sucesso/erro

## 📝 Como Usar

1. **Cadastrar Produtos**: Aba "📦 Produtos" → Botão "Cadastrar Novo Produto"
2. **Cadastrar Clientes**: Aba "👥 Clientes" → Botão "Cadastrar Novo Cliente"
3. **Realizar Venda**: Aba "🛒 Vendas" → Botão "Nova Venda (Atendimento)"
   - Digite o ID do cliente
   - Adicione produtos (ID do produto + quantidade)
   - Clique em "Adicionar Item" para cada produto
   - Clique em "Finalizar Venda" quando terminar
4. **Processar Devolução**: Aba "🛒 Vendas" → Botão "Processar Devolução"
   - Digite o ID da venda
   - Confirme a devolução

## 🔧 Solução de Problemas

### GTK não encontrado
Se aparecer erro "GTK não encontrado", instale:
```bash
sudo apt-get install libgtk-3-dev
```

### Erro de compilação
Certifique-se de ter instalado todas as dependências:
```bash
sudo apt-get install build-essential libgtk-3-dev pkg-config
```

### Interface não abre
Verifique se o display está configurado corretamente (para uso remoto):
```bash
export DISPLAY=:0
```

## 🛠️ Desenvolvimento

### Estrutura do Código

- **Estruturas de dados**: Produto, Cliente, ItemVenda, Venda
- **Funções principais**: Cadastro, listagem, busca, vendas, devoluções
- **Interface GTK**: Janelas, diálogos, tabelas, botões

### Compilar ambas versões
```bash
make all
```

### Limpar arquivos compilados
```bash
make clean
```

### Ver ajuda do Makefile
```bash
make help
```

## 📄 Licença

Este projeto foi desenvolvido para uso em lojas.

## 👨‍💻 Autor

Italo S. Santos

Desenvolvido em C puro com GTK+ em e para Linux.
