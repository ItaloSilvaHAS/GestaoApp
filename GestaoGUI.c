#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PRODUTOS 1000
#define MAX_CLIENTES 500
#define MAX_VENDAS 2000
#define MAX_ITENS_VENDA 50

// Estruturas de dados
typedef struct {
    int id;
    char nome[100];
    char descricao[200];
    float preco;
    int estoque;
    int ativo;
} Produto;

typedef struct {
    int id;
    char nome[100];
    char telefone[20];
    char endereco[200];
    int ativo;
} Cliente;

typedef struct {
    int produto_id;
    char nome_produto[100];
    int quantidade;
    float preco_unitario;
    float subtotal;
} ItemVenda;

typedef struct {
    int id;
    int cliente_id;
    char nome_cliente[100];
    ItemVenda itens[MAX_ITENS_VENDA];
    int num_itens;
    float total;
    char data[11];
    int status; // 1 = vendido, 2 = devolvido
} Venda;

// Estrutura auxiliar para dialog de venda
typedef struct {
    Venda *venda;
    GtkWidget *produto_spin;
    GtkWidget *qtd_spin;
    GtkListStore *itens_store;
    GtkWidget *total_label;
} VendaDialogData;

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

// Widgets globais para a janela principal
GtkWidget *window;
GtkWidget *notebook;
GtkListStore *produtos_store;
GtkListStore *clientes_store;
GtkListStore *vendas_store;
GtkListStore *itens_venda_store;

// Protótipos
int encontrarProdutoPorId(int id);
int encontrarClientePorId(int id);
int encontrarVendaPorId(int id);
void atualizarListaProdutos();
void atualizarListaClientes();
void atualizarListaVendas();
void obterDataAtual(char *data);
void adicionar_item_venda(GtkButton *button, gpointer data);
void on_cadastrar_produto_clicked(GtkButton *button, gpointer user_data);
void on_cadastrar_cliente_clicked(GtkButton *button, gpointer user_data);
void on_nova_venda_clicked(GtkButton *button, gpointer user_data);
void on_devolver_clicked(GtkButton *button, gpointer user_data);
GtkWidget* criar_pagina_produtos();
GtkWidget* criar_pagina_clientes();
GtkWidget* criar_pagina_vendas();
gboolean on_window_destroy(GtkWidget *widget, gpointer data);

// Função para obter data atual
void obterDataAtual(char *data) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(data, 11, "%d/%m/%Y", tm_info);
}

// Função para encontrar produto por ID
int encontrarProdutoPorId(int id) {
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].id == id && produtos[i].ativo) {
            return i;
        }
    }
    return -1;
}

// Função para encontrar cliente por ID
int encontrarClientePorId(int id) {
    for (int i = 0; i < total_clientes; i++) {
        if (clientes[i].id == id && clientes[i].ativo) {
            return i;
        }
    }
    return -1;
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

// Atualizar lista de produtos na interface
void atualizarListaProdutos() {
    gtk_list_store_clear(produtos_store);
    GtkTreeIter iter;
    char preco_str[20];
    
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].ativo) {
            gtk_list_store_append(produtos_store, &iter);
            sprintf(preco_str, "%.2f", produtos[i].preco);
            gtk_list_store_set(produtos_store, &iter,
                0, produtos[i].id,
                1, produtos[i].nome,
                2, produtos[i].descricao,
                3, preco_str,
                4, produtos[i].estoque,
                -1);
        }
    }
}

// Atualizar lista de clientes na interface
void atualizarListaClientes() {
    gtk_list_store_clear(clientes_store);
    GtkTreeIter iter;
    
    for (int i = 0; i < total_clientes; i++) {
        if (clientes[i].ativo) {
            gtk_list_store_append(clientes_store, &iter);
            gtk_list_store_set(clientes_store, &iter,
                0, clientes[i].id,
                1, clientes[i].nome,
                2, clientes[i].telefone,
                3, clientes[i].endereco,
                -1);
        }
    }
}

// Atualizar lista de vendas na interface
void atualizarListaVendas() {
    gtk_list_store_clear(vendas_store);
    GtkTreeIter iter;
    char total_str[20];
    
    for (int i = 0; i < total_vendas; i++) {
        gtk_list_store_append(vendas_store, &iter);
        const char *status_str = (vendas[i].status == 1) ? "Vendido" : "Devolvido";
        sprintf(total_str, "%.2f", vendas[i].total);
        gtk_list_store_set(vendas_store, &iter,
            0, vendas[i].id,
            1, vendas[i].nome_cliente,
            2, vendas[i].data,
            3, total_str,
            4, status_str,
            -1);
    }
}

// Callback: Cadastrar Produto
void on_cadastrar_produto_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *box;
    GtkWidget *nome_entry, *desc_entry, *preco_entry, *estoque_entry;
    GtkWidget *label;
    gint response;
    
    dialog = gtk_dialog_new_with_buttons("Cadastrar Produto",
                                        GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL,
                                        "Cadastrar", GTK_RESPONSE_ACCEPT,
                                        "Cancelar", GTK_RESPONSE_REJECT,
                                        NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    // Nome
    label = gtk_label_new("Nome do Produto:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    nome_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), nome_entry, FALSE, FALSE, 0);
    
    // Descrição
    label = gtk_label_new("Descrição:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    desc_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), desc_entry, FALSE, FALSE, 0);
    
    // Preço
    label = gtk_label_new("Preço (R$):");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    preco_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), preco_entry, FALSE, FALSE, 0);
    
    // Estoque
    label = gtk_label_new("Quantidade em Estoque:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    estoque_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), estoque_entry, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_show_all(dialog);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        if (total_produtos < MAX_PRODUTOS) {
            Produto novo;
            novo.id = id_produto_atual++;
            novo.ativo = 1;
            
            strcpy(novo.nome, gtk_entry_get_text(GTK_ENTRY(nome_entry)));
            strcpy(novo.descricao, gtk_entry_get_text(GTK_ENTRY(desc_entry)));
            novo.preco = atof(gtk_entry_get_text(GTK_ENTRY(preco_entry)));
            novo.estoque = atoi(gtk_entry_get_text(GTK_ENTRY(estoque_entry)));
            
            if (strlen(novo.nome) > 0) {
                produtos[total_produtos++] = novo;
                atualizarListaProdutos();
                
                GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Produto cadastrado com sucesso!\nID: %d", novo.id);
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
            }
        } else {
            GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Limite de produtos atingido!");
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Callback: Cadastrar Cliente
void on_cadastrar_cliente_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *box;
    GtkWidget *nome_entry, *tel_entry, *end_entry;
    GtkWidget *label;
    gint response;
    
    dialog = gtk_dialog_new_with_buttons("Cadastrar Cliente",
                                        GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL,
                                        "Cadastrar", GTK_RESPONSE_ACCEPT,
                                        "Cancelar", GTK_RESPONSE_REJECT,
                                        NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    label = gtk_label_new("Nome do Cliente:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    nome_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), nome_entry, FALSE, FALSE, 0);
    
    label = gtk_label_new("Telefone:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    tel_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), tel_entry, FALSE, FALSE, 0);
    
    label = gtk_label_new("Endereço:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    end_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), end_entry, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_show_all(dialog);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        if (total_clientes < MAX_CLIENTES) {
            Cliente novo;
            novo.id = id_cliente_atual++;
            novo.ativo = 1;
            
            strcpy(novo.nome, gtk_entry_get_text(GTK_ENTRY(nome_entry)));
            strcpy(novo.telefone, gtk_entry_get_text(GTK_ENTRY(tel_entry)));
            strcpy(novo.endereco, gtk_entry_get_text(GTK_ENTRY(end_entry)));
            
            if (strlen(novo.nome) > 0) {
                clientes[total_clientes++] = novo;
                atualizarListaClientes();
                
                GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Cliente cadastrado com sucesso!\nID: %d", novo.id);
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
            }
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Callback: Adicionar item à venda
void adicionar_item_venda(GtkButton *button, gpointer data) {
    VendaDialogData *dialog_data = (VendaDialogData *)data;
    Venda *venda = dialog_data->venda;
    
    if (venda->num_itens >= MAX_ITENS_VENDA) {
        GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Limite de itens por venda atingido!");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    
    int produto_id = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(dialog_data->produto_spin));
    int quantidade = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(dialog_data->qtd_spin));
    
    int produto_idx = encontrarProdutoPorId(produto_id);
    if (produto_idx == -1) {
        GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Produto não encontrado!");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    
    if (produtos[produto_idx].estoque < quantidade) {
        GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Estoque insuficiente!\nDisponível: %d", produtos[produto_idx].estoque);
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    
    // Adicionar item
    ItemVenda item;
    item.produto_id = produto_id;
    strcpy(item.nome_produto, produtos[produto_idx].nome);
    item.quantidade = quantidade;
    item.preco_unitario = produtos[produto_idx].preco;
    item.subtotal = quantidade * produtos[produto_idx].preco;
    
    venda->itens[venda->num_itens++] = item;
    venda->total += item.subtotal;
    
    // Atualizar estoque
    produtos[produto_idx].estoque -= quantidade;
    
    // Adicionar à lista visual
    GtkTreeIter iter;
    char preco_str[20], subtotal_str[20];
    sprintf(preco_str, "%.2f", item.preco_unitario);
    sprintf(subtotal_str, "%.2f", item.subtotal);
    
    gtk_list_store_append(dialog_data->itens_store, &iter);
    gtk_list_store_set(dialog_data->itens_store, &iter,
        0, item.nome_produto,
        1, item.quantidade,
        2, preco_str,
        3, subtotal_str,
        -1);
    
    // Atualizar total
    char total_str[50];
    sprintf(total_str, "Total: R$ %.2f", venda->total);
    gtk_label_set_text(GTK_LABEL(dialog_data->total_label), total_str);
}

// Callback: Nova Venda
void on_nova_venda_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *main_box, *box_h, *box_v;
    GtkWidget *label, *cliente_spin, *produto_spin, *qtd_spin;
    GtkWidget *add_btn;
    GtkWidget *tree_view;
    GtkListStore *itens_store;
    GtkWidget *total_label;
    gint response;
    
    Venda nova_venda;
    nova_venda.id = id_venda_atual++;
    nova_venda.num_itens = 0;
    nova_venda.total = 0.0;
    nova_venda.status = 1;
    obterDataAtual(nova_venda.data);
    
    dialog = gtk_dialog_new_with_buttons("Nova Venda - Atendimento ao Cliente",
                                        GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL,
                                        "Finalizar Venda", GTK_RESPONSE_ACCEPT,
                                        "Cancelar", GTK_RESPONSE_REJECT,
                                        NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    
    // Cliente
    box_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label = gtk_label_new("ID do Cliente:");
    gtk_box_pack_start(GTK_BOX(box_v), label, FALSE, FALSE, 0);
    cliente_spin = gtk_spin_button_new_with_range(1, 9999, 1);
    gtk_box_pack_start(GTK_BOX(box_v), cliente_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), box_v, FALSE, FALSE, 0);
    
    // Adicionar produto
    box_h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    box_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label = gtk_label_new("ID do Produto:");
    gtk_box_pack_start(GTK_BOX(box_v), label, FALSE, FALSE, 0);
    produto_spin = gtk_spin_button_new_with_range(1, 9999, 1);
    gtk_box_pack_start(GTK_BOX(box_v), produto_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_h), box_v, FALSE, FALSE, 0);
    
    box_v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label = gtk_label_new("Quantidade:");
    gtk_box_pack_start(GTK_BOX(box_v), label, FALSE, FALSE, 0);
    qtd_spin = gtk_spin_button_new_with_range(1, 9999, 1);
    gtk_box_pack_start(GTK_BOX(box_v), qtd_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_h), box_v, FALSE, FALSE, 0);
    
    add_btn = gtk_button_new_with_label("➕ Adicionar Item");
    gtk_box_pack_start(GTK_BOX(box_h), add_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), box_h, FALSE, FALSE, 0);
    
    // Lista de itens
    itens_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(itens_store));
    
    GtkCellRenderer *renderer;
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Produto", renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Qtd", renderer, "text", 1, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Unit. (R$)", renderer, "text", 2, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Subtotal (R$)", renderer, "text", 3, NULL);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Label total
    total_label = gtk_label_new("Total: R$ 0,00");
    gtk_box_pack_start(GTK_BOX(main_box), total_label, FALSE, FALSE, 0);
    
    // Estrutura auxiliar para callback
    VendaDialogData dialog_data;
    dialog_data.venda = &nova_venda;
    dialog_data.produto_spin = produto_spin;
    dialog_data.qtd_spin = qtd_spin;
    dialog_data.itens_store = itens_store;
    dialog_data.total_label = total_label;
    
    g_signal_connect(add_btn, "clicked", G_CALLBACK(adicionar_item_venda), &dialog_data);
    
    gtk_container_add(GTK_CONTAINER(content_area), main_box);
    gtk_widget_show_all(dialog);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT && nova_venda.num_itens > 0) {
        int cliente_id = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(cliente_spin));
        int cliente_idx = encontrarClientePorId(cliente_id);
        
        if (cliente_idx != -1) {
            nova_venda.cliente_id = cliente_id;
            strcpy(nova_venda.nome_cliente, clientes[cliente_idx].nome);
            
            vendas[total_vendas++] = nova_venda;
            atualizarListaVendas();
            atualizarListaProdutos();
            
            GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Venda realizada com sucesso!\n\nID da Venda: %d\nCliente: %s\nTotal: R$ %.2f",
                nova_venda.id, nova_venda.nome_cliente, nova_venda.total);
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
        } else {
            // Reverter estoque se cliente inválido
            for (int i = 0; i < nova_venda.num_itens; i++) {
                int prod_idx = encontrarProdutoPorId(nova_venda.itens[i].produto_id);
                if (prod_idx != -1) {
                    produtos[prod_idx].estoque += nova_venda.itens[i].quantidade;
                }
            }
            atualizarListaProdutos();
            
            GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Cliente não encontrado!\nEstoque revertido.");
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
        }
    } else if (nova_venda.num_itens > 0) {
        // Reverter estoque se cancelar
        for (int i = 0; i < nova_venda.num_itens; i++) {
            int prod_idx = encontrarProdutoPorId(nova_venda.itens[i].produto_id);
            if (prod_idx != -1) {
                produtos[prod_idx].estoque += nova_venda.itens[i].quantidade;
            }
        }
        atualizarListaProdutos();
    }
    
    gtk_widget_destroy(dialog);
}

// Callback: Processar Devolução
void on_devolver_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *box;
    GtkWidget *label, *venda_spin;
    gint response;
    
    dialog = gtk_dialog_new_with_buttons("Processar Devolução",
                                        GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL,
                                        "Confirmar Devolução", GTK_RESPONSE_ACCEPT,
                                        "Cancelar", GTK_RESPONSE_REJECT,
                                        NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    label = gtk_label_new("ID da Venda para Devolver:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    venda_spin = gtk_spin_button_new_with_range(1, 9999, 1);
    gtk_box_pack_start(GTK_BOX(box), venda_spin, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_show_all(dialog);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        int venda_id = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(venda_spin));
        int venda_idx = encontrarVendaPorId(venda_id);
        
        if (venda_idx != -1) {
            if (vendas[venda_idx].status == 2) {
                GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_WARNING,
                    GTK_BUTTONS_OK,
                    "Esta venda já foi devolvida anteriormente!");
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
            } else {
                // Reverter estoque
                for (int i = 0; i < vendas[venda_idx].num_itens; i++) {
                    int prod_idx = encontrarProdutoPorId(vendas[venda_idx].itens[i].produto_id);
                    if (prod_idx != -1) {
                        produtos[prod_idx].estoque += vendas[venda_idx].itens[i].quantidade;
                    }
                }
                
                vendas[venda_idx].status = 2;
                atualizarListaVendas();
                atualizarListaProdutos();
                
                GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Devolução processada com sucesso!\nEstoque revertido.");
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
            }
        } else {
            GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Venda não encontrada!");
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Criar página de produtos
GtkWidget* criar_pagina_produtos() {
    GtkWidget *box, *box_h;
    GtkWidget *button;
    GtkWidget *tree_view;
    GtkWidget *scrolled;
    GtkCellRenderer *renderer;
    
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    // Botão cadastrar
    box_h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    button = gtk_button_new_with_label("➕ Cadastrar Novo Produto");
    g_signal_connect(button, "clicked", G_CALLBACK(on_cadastrar_produto_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_h), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), box_h, FALSE, FALSE, 0);
    
    // Tabela de produtos
    produtos_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(produtos_store));
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "ID", renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Nome", renderer, "text", 1, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Descrição", renderer, "text", 2, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Preço (R$)", renderer, "text", 3, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Estoque", renderer, "text", 4, NULL);
    
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, 0);
    
    return box;
}

// Criar página de clientes
GtkWidget* criar_pagina_clientes() {
    GtkWidget *box, *box_h;
    GtkWidget *button;
    GtkWidget *tree_view;
    GtkWidget *scrolled;
    GtkCellRenderer *renderer;
    
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    box_h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    button = gtk_button_new_with_label("➕ Cadastrar Novo Cliente");
    g_signal_connect(button, "clicked", G_CALLBACK(on_cadastrar_cliente_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_h), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), box_h, FALSE, FALSE, 0);
    
    clientes_store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(clientes_store));
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "ID", renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Nome", renderer, "text", 1, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Telefone", renderer, "text", 2, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Endereço", renderer, "text", 3, NULL);
    
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, 0);
    
    return box;
}

// Criar página de vendas
GtkWidget* criar_pagina_vendas() {
    GtkWidget *box, *box_h;
    GtkWidget *button_venda, *button_devolver;
    GtkWidget *tree_view;
    GtkWidget *scrolled;
    GtkCellRenderer *renderer;
    
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    
    box_h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    button_venda = gtk_button_new_with_label("🛒 Nova Venda (Atendimento)");
    button_devolver = gtk_button_new_with_label("↩️ Processar Devolução");
    g_signal_connect(button_venda, "clicked", G_CALLBACK(on_nova_venda_clicked), NULL);
    g_signal_connect(button_devolver, "clicked", G_CALLBACK(on_devolver_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box_h), button_venda, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_h), button_devolver, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), box_h, FALSE, FALSE, 0);
    
    vendas_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(vendas_store));
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "ID", renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Cliente", renderer, "text", 1, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Data", renderer, "text", 2, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Total (R$)", renderer, "text", 3, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1,
        "Status", renderer, "text", 4, NULL);
    
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, 0);
    
    return box;
}

// Callback: Fechar aplicação
gboolean on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
    return FALSE;
}

// Função principal
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Criar janela principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema de Gestão - Lojas");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    
    // Criar notebook (abas)
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    
    // Adicionar páginas
    GtkWidget *label1 = gtk_label_new("📦 Produtos");
    GtkWidget *page1 = criar_pagina_produtos();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page1, label1);
    
    GtkWidget *label2 = gtk_label_new("👥 Clientes");
    GtkWidget *page2 = criar_pagina_clientes();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page2, label2);
    
    GtkWidget *label3 = gtk_label_new("🛒 Vendas");
    GtkWidget *page3 = criar_pagina_vendas();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page3, label3);
    
    gtk_container_add(GTK_CONTAINER(window), notebook);
    
    gtk_widget_show_all(window);
    
    // Inicializar listas vazias
    atualizarListaProdutos();
    atualizarListaClientes();
    atualizarListaVendas();
    
    gtk_main();
    
    return 0;
}
