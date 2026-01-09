#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 1000
#define MAX_CLIENTES 500
#define MAX_VENDAS 2000
#define MAX_ITENS_VENDA 50

// Estrutura para os Produtos
typedef struct {
    int id;
    char nome[100];
    char descricao[200];
    float preco;
    int estoque;
    int ativo; // 1 = ativo, 0 = inativo
} Produto;

// Estrutura para Clientes
typedef struct {
    int id;
    char nome[100];
    char telefone[20];
    char endereco[200];
    int ativo; // 1 = ativo, 0 = inativo
} Cliente;

// Estrutura para itens de venda
typedef struct {
    int produto_id;
    char nome_produto[100];
    int quantidade;
    float preco_unitario;
    float subtotal;
} ItemVenda;

// Estrutura para Vendas
typedef struct {
    int id;
    int cliente_id;
    char nome_cliente[100];
    ItemVenda itens[MAX_ITENS_VENDA];
    int num_itens;
    float total;
    char data[11]; // formato: DD/MM/AAAA
    int status; // 1 = vendido, 2 = devolvido
} Venda;

// Variáveis globais
Produto produtos[MAX_PRODUTOS];
Cliente clientes[MAX_CLIENTES];
Venda vendas[MAX_VENDAS];

int total_produtos = 0;
int total_clientes = 0;
int total_vendas = 0;
int id_produto_atual = 1;
int id_cliente_atual = 1;
int id_venda_atual = 1;

// Protótipos de funções
void menu();
void cadastrarProduto();
void listarProdutos();
void buscarProduto();
void cadastrarCliente();
void listarClientes();
void buscarCliente();
void novaVenda();
void listarVendas();
void processarDevolucao();
void carregarDados();
void salvarDados();
void limparTela();
void pausar();
int encontrarProdutoPorId(int id);
int encontrarClientePorId(int id);
int encontrarVendaPorId(int id);
void obterDataAtual(char *data);

// Função para limpar a tela (funciona em Linux e Windows)
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Função para pausar e esperar o usuário
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar(); // Limpar o buffer
}

// Função para obter data atual (formato simples)
void obterDataAtual(char *data) {
    printf("Digite a data (DD/MM/AAAA): ");
    fgets(data, 11, stdin);
    data[strcspn(data, "\n")] = 0;
    if (strlen(data) < 10) {
        strcpy(data, "HOJE");
    }
}

// Menu principal
void menu() {
    limparTela();
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║         SISTEMA DE GESTÃO - lOJAS             ║\n");
    printf("╠═══════════════════════════════════════════════════════╣\n");
    printf("║ PRODUTOS:                                             ║\n");
    printf("║   1. Cadastrar Produto                                ║\n");
    printf("║   2. Listar Produtos                                  ║\n");
    printf("║   3. Buscar Produto                                   ║\n");
    printf("║                                                        ║\n");
    printf("║ CLIENTES:                                             ║\n");
    printf("║   4. Cadastrar Cliente                                ║\n");
    printf("║   5. Listar Clientes                                  ║\n");
    printf("║   6. Buscar Cliente                                   ║\n");
    printf("║                                                        ║\n");
    printf("║ VENDAS:                                               ║\n");
    printf("║   7. Nova Venda (Atendimento)                         ║\n");
    printf("║   8. Listar Vendas                                    ║\n");
    printf("║                                                        ║\n");
    printf("║ DEVOLUÇÕES:                                           ║\n");
    printf("║   9. Processar Devolução                              ║\n");
    printf("║                                                        ║\n");
    printf("║   0. Sair                                             ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\nEscolha uma opcao: ");
}

// Função para cadastrar produto
void cadastrarProduto() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║        CADASTRO DE PRODUTO                 ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    if (total_produtos >= MAX_PRODUTOS) {
        printf("ERRO: Limite de produtos atingido!\n");
        pausar();
        return;
    }

    Produto novo;
    novo.id = id_produto_atual++;
    novo.ativo = 1;

    printf("Nome do produto: ");
    fgets(novo.nome, 100, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = 0;

    printf("Descricao: ");
    fgets(novo.descricao, 200, stdin);
    novo.descricao[strcspn(novo.descricao, "\n")] = 0;

    printf("Preco (R$): ");
    scanf("%f", &novo.preco);
    getchar();

    printf("Quantidade em estoque: ");
    scanf("%d", &novo.estoque);
    getchar();

    produtos[total_produtos++] = novo;
    printf("\n✓ Produto cadastrado com sucesso! ID: %d\n", novo.id);
    pausar();
}

// Função para listar produtos
void listarProdutos() {
    limparTela();
    printf("\n╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        LISTA DE PRODUTOS                                  ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");

    if (total_produtos == 0) {
        printf("║ Nenhum produto cadastrado.                                               ║\n");
    } else {
        printf("║ %-5s %-30s %-15s %-10s %-10s ║\n", "ID", "NOME", "PREÇO (R$)", "ESTOQUE", "STATUS");
        printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
        
        for (int i = 0; i < total_produtos; i++) {
            if (produtos[i].ativo) {
                printf("║ %-5d %-30s %-15.2f %-10d %-10s ║\n",
                    produtos[i].id,
                    produtos[i].nome,
                    produtos[i].preco,
                    produtos[i].estoque,
                    "Ativo");
            }
        }
    }
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    pausar();
}

// Função para buscar produto
void buscarProduto() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║           BUSCAR PRODUTO                   ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    int id;
    printf("Digite o ID do produto: ");
    scanf("%d", &id);
    getchar();

    int indice = encontrarProdutoPorId(id);
    if (indice != -1 && produtos[indice].ativo) {
        printf("\n✓ Produto encontrado:\n");
        printf("  ID: %d\n", produtos[indice].id);
        printf("  Nome: %s\n", produtos[indice].nome);
        printf("  Descricao: %s\n", produtos[indice].descricao);
        printf("  Preco: R$ %.2f\n", produtos[indice].preco);
        printf("  Estoque: %d\n", produtos[indice].estoque);
    } else {
        printf("\n✗ Produto nao encontrado ou inativo.\n");
    }
    pausar();
}

// Função para encontrar produto por ID
int encontrarProdutoPorId(int id) {
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Função para cadastrar cliente
void cadastrarCliente() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║        CADASTRO DE CLIENTE                 ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    if (total_clientes >= MAX_CLIENTES) {
        printf("ERRO: Limite de clientes atingido!\n");
        pausar();
        return;
    }

    Cliente novo;
    novo.id = id_cliente_atual++;
    novo.ativo = 1;

    printf("Nome do cliente: ");
    fgets(novo.nome, 100, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = 0;

    printf("Telefone: ");
    fgets(novo.telefone, 20, stdin);
    novo.telefone[strcspn(novo.telefone, "\n")] = 0;

    printf("Endereco: ");
    fgets(novo.endereco, 200, stdin);
    novo.endereco[strcspn(novo.endereco, "\n")] = 0;

    clientes[total_clientes++] = novo;
    printf("\n✓ Cliente cadastrado com sucesso! ID: %d\n", novo.id);
    pausar();
}

// Função para listar clientes
void listarClientes() {
    limparTela();
    printf("\n╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        LISTA DE CLIENTES                                  ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");

    if (total_clientes == 0) {
        printf("║ Nenhum cliente cadastrado.                                                 ║\n");
    } else {
        printf("║ %-5s %-35s %-20s ║\n", "ID", "NOME", "TELEFONE");
        printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
        
        for (int i = 0; i < total_clientes; i++) {
            if (clientes[i].ativo) {
                printf("║ %-5d %-35s %-20s ║\n",
                    clientes[i].id,
                    clientes[i].nome,
                    clientes[i].telefone);
            }
        }
    }
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    pausar();
}

// Função para buscar cliente
void buscarCliente() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║           BUSCAR CLIENTE                   ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    int id;
    printf("Digite o ID do cliente: ");
    scanf("%d", &id);
    getchar();

    int indice = encontrarClientePorId(id);
    if (indice != -1 && clientes[indice].ativo) {
        printf("\n✓ Cliente encontrado:\n");
        printf("  ID: %d\n", clientes[indice].id);
        printf("  Nome: %s\n", clientes[indice].nome);
        printf("  Telefone: %s\n", clientes[indice].telefone);
        printf("  Endereco: %s\n", clientes[indice].endereco);
    } else {
        printf("\n✗ Cliente nao encontrado ou inativo.\n");
    }
    pausar();
}

// Função para encontrar cliente por ID
int encontrarClientePorId(int id) {
    for (int i = 0; i < total_clientes; i++) {
        if (clientes[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Função para nova venda (atendimento ao cliente)
void novaVenda() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║        NOVA VENDA - ATENDIMENTO            ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    if (total_vendas >= MAX_VENDAS) {
        printf("ERRO: Limite de vendas atingido!\n");
        pausar();
        return;
    }

    Venda nova_venda;
    nova_venda.id = id_venda_atual++;
    nova_venda.num_itens = 0;
    nova_venda.total = 0.0;
    nova_venda.status = 1; // Vendido

    // Selecionar cliente
    int cliente_id;
    printf("Digite o ID do cliente: ");
    scanf("%d", &cliente_id);
    getchar();

    int cliente_indice = encontrarClientePorId(cliente_id);
    if (cliente_indice == -1 || !clientes[cliente_indice].ativo) {
        printf("ERRO: Cliente nao encontrado!\n");
        pausar();
        return;
    }

    nova_venda.cliente_id = cliente_id;
    strcpy(nova_venda.nome_cliente, clientes[cliente_indice].nome);

    // Obter data
    obterDataAtual(nova_venda.data);

    // Adicionar produtos
    char continuar = 'S';
    while (continuar == 'S' || continuar == 's') {
        if (nova_venda.num_itens >= MAX_ITENS_VENDA) {
            printf("Limite de itens por venda atingido!\n");
            break;
        }

        int produto_id, quantidade;
        printf("\n--- Adicionar Produto ---\n");
        printf("ID do produto: ");
        scanf("%d", &produto_id);
        getchar();

        int produto_indice = encontrarProdutoPorId(produto_id);
        if (produto_indice == -1 || !produtos[produto_indice].ativo) {
            printf("ERRO: Produto nao encontrado!\n");
            continue;
        }

        if (produtos[produto_indice].estoque <= 0) {
            printf("ERRO: Produto sem estoque!\n");
            continue;
        }

        printf("Quantidade (estoque disponivel: %d): ", produtos[produto_indice].estoque);
        scanf("%d", &quantidade);
        getchar();

        if (quantidade <= 0 || quantidade > produtos[produto_indice].estoque) {
            printf("ERRO: Quantidade invalida!\n");
            continue;
        }

        // Adicionar item à venda
        ItemVenda item;
        item.produto_id = produto_id;
        strcpy(item.nome_produto, produtos[produto_indice].nome);
        item.quantidade = quantidade;
        item.preco_unitario = produtos[produto_indice].preco;
        item.subtotal = quantidade * produtos[produto_indice].preco;

        nova_venda.itens[nova_venda.num_itens++] = item;
        nova_venda.total += item.subtotal;

        // Atualizar estoque
        produtos[produto_indice].estoque -= quantidade;

        printf("✓ Produto adicionado! Subtotal: R$ %.2f\n", item.subtotal);
        printf("Deseja adicionar mais produtos? (S/N): ");
        scanf(" %c", &continuar);
        getchar();
    }

    if (nova_venda.num_itens > 0) {
        // Mostrar resumo da venda
        printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
        printf("║                    RESUMO DA VENDA                                 ║\n");
        printf("╠═══════════════════════════════════════════════════════════════════╣\n");
        printf("║ Venda ID: %-54d ║\n", nova_venda.id);
        printf("║ Cliente: %-58s ║\n", nova_venda.nome_cliente);
        printf("║ Data: %-62s ║\n", nova_venda.data);
        printf("╠═══════════════════════════════════════════════════════════════════╣\n");
        printf("║ %-30s %-10s %-12s %-12s ║\n", "PRODUTO", "QTDE", "UNIT. (R$)", "SUBTOTAL (R$)");
        printf("╠═══════════════════════════════════════════════════════════════════╣\n");
        
        for (int i = 0; i < nova_venda.num_itens; i++) {
            printf("║ %-30s %-10d %-12.2f %-12.2f ║\n",
                nova_venda.itens[i].nome_produto,
                nova_venda.itens[i].quantidade,
                nova_venda.itens[i].preco_unitario,
                nova_venda.itens[i].subtotal);
        }
        
        printf("╠═══════════════════════════════════════════════════════════════════╣\n");
        printf("║ TOTAL: %-63.2f ║\n", nova_venda.total);
        printf("╚═══════════════════════════════════════════════════════════════════╝\n");

        // Confirmar venda
        char confirmar;
        printf("\nConfirmar venda? (S/N): ");
        scanf(" %c", &confirmar);
        getchar();

        if (confirmar == 'S' || confirmar == 's') {
            vendas[total_vendas++] = nova_venda;
            printf("\n✓ Venda registrada com sucesso! ID da venda: %d\n", nova_venda.id);
        } else {
            // Reverter estoque se cancelar
            for (int i = 0; i < nova_venda.num_itens; i++) {
                int prod_indice = encontrarProdutoPorId(nova_venda.itens[i].produto_id);
                if (prod_indice != -1) {
                    produtos[prod_indice].estoque += nova_venda.itens[i].quantidade;
                }
            }
            printf("\n✗ Venda cancelada. Estoque revertido.\n");
        }
    } else {
        printf("\n✗ Nenhum produto adicionado. Venda cancelada.\n");
    }

    pausar();
}

// Função para listar vendas
void listarVendas() {
    limparTela();
    printf("\n╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        LISTA DE VENDAS                                    ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");

    if (total_vendas == 0) {
        printf("║ Nenhuma venda registrada.                                                  ║\n");
    } else {
        printf("║ %-6s %-25s %-15s %-12s %-10s ║\n", "ID", "CLIENTE", "DATA", "TOTAL (R$)", "STATUS");
        printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
        
        for (int i = 0; i < total_vendas; i++) {
            char status_str[15];
            if (vendas[i].status == 1) {
                strcpy(status_str, "Vendido");
            } else if (vendas[i].status == 2) {
                strcpy(status_str, "Devolvido");
            } else {
                strcpy(status_str, "Indefinido");
            }

            printf("║ %-6d %-25s %-15s %-12.2f %-10s ║\n",
                vendas[i].id,
                vendas[i].nome_cliente,
                vendas[i].data,
                vendas[i].total,
                status_str);
        }
    }
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    pausar();
}

// Função para processar devolução
void processarDevolucao() {
    limparTela();
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║        PROCESSAR DEVOLUÇÃO                ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    if (total_vendas == 0) {
        printf("Nenhuma venda registrada para devolver.\n");
        pausar();
        return;
    }

    int venda_id;
    printf("Digite o ID da venda para devolver: ");
    scanf("%d", &venda_id);
    getchar();

    int venda_indice = encontrarVendaPorId(venda_id);
    if (venda_indice == -1) {
        printf("ERRO: Venda nao encontrada!\n");
        pausar();
        return;
    }

    if (vendas[venda_indice].status == 2) {
        printf("ATENCAO: Esta venda ja foi devolvida anteriormente!\n");
        pausar();
        return;
    }

    // Mostrar detalhes da venda
    printf("\n--- Detalhes da Venda ---\n");
    printf("ID: %d\n", vendas[venda_indice].id);
    printf("Cliente: %s\n", vendas[venda_indice].nome_cliente);
    printf("Data: %s\n", vendas[venda_indice].data);
    printf("Total: R$ %.2f\n", vendas[venda_indice].total);
    printf("\nItens:\n");
    
    for (int i = 0; i < vendas[venda_indice].num_itens; i++) {
        printf("  - %s (Qtd: %d, Preco: R$ %.2f)\n",
            vendas[venda_indice].itens[i].nome_produto,
            vendas[venda_indice].itens[i].quantidade,
            vendas[venda_indice].itens[i].preco_unitario);
    }

    // Confirmar devolução
    char confirmar;
    printf("\nConfirmar devolucao completa? (S/N): ");
    scanf(" %c", &confirmar);
    getchar();

    if (confirmar == 'S' || confirmar == 's') {
        // Reverter estoque de todos os itens
        for (int i = 0; i < vendas[venda_indice].num_itens; i++) {
            int prod_indice = encontrarProdutoPorId(vendas[venda_indice].itens[i].produto_id);
            if (prod_indice != -1) {
                produtos[prod_indice].estoque += vendas[venda_indice].itens[i].quantidade;
                printf("✓ Estoque do produto '%s' revertido. Novo estoque: %d\n",
                    produtos[prod_indice].nome,
                    produtos[prod_indice].estoque);
            }
        }

        // Marcar venda como devolvida
        vendas[venda_indice].status = 2;
        printf("\n✓ Devolucao processada com sucesso!\n");
        printf("  Venda ID %d marcada como DEVOLVIDA\n", vendas[venda_indice].id);
        printf("  Estoque de todos os produtos foi revertido\n");
    } else {
        printf("\n✗ Devolucao cancelada.\n");
    }

    pausar();
}

// Função para encontrar venda por ID
int encontrarVendaPorId(int id) {
    for (int i = 0; i < total_vendas; i++) {
        if (vendas[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Função para carregar dados (implementação futura com arquivos)
void carregarDados() {
    // TODO: Implementar carregamento de arquivos
}

// Função para salvar dados (implementação futura com arquivos)
void salvarDados() {
    // TODO: Implementar salvamento em arquivos
}

// Função principal
int main() {
    int opcao;

    // Carregar dados ao iniciar
    carregarDados();

    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // Limpar o buffer do teclado

        switch(opcao) {
            case 1:
                cadastrarProduto();
                break;
            case 2:
                listarProdutos();
                break;
            case 3:
                buscarProduto();
                break;
            case 4:
                cadastrarCliente();
                break;
            case 5:
                listarClientes();
                break;
            case 6:
                buscarCliente();
                break;
            case 7:
                novaVenda();
                break;
            case 8:
                listarVendas();
                break;
            case 9:
                processarDevolucao();
                break;
            case 0:
                limparTela();
                printf("\n╔════════════════════════════════════════════╗\n");
                printf("║   Salvando dados e encerrando sistema...  ║\n");
                printf("╚════════════════════════════════════════════╝\n");
                salvarDados();
                printf("Sistema encerrado. Obrigado!\n\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                pausar();
        }
    } while(opcao != 0);

    return 0;
}