/**
 * @file fyd_menu_iniciar.c
 * @brief Menu Iniciar — FydelisTech OS v4.1
 * Tamanho fixo 280px + Categorias + Caminho do Registro
 * Salvador • Bahia 🇧🇷
 */

#include "fyd_desktop.h"
#include "fyd_registro.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    gchar *categoria;     // Nova: suporte a categoria
    gchar *subcategoria;  // Nova: suporte a subcategoria
    gchar *icone;
    gchar *nome;
    gchar *tipo;
    gchar *alvo;
} FydMenuItem;

static GList *itens_menu = NULL;

static void liberar_item_menu(gpointer dados) {
    FydMenuItem *item = dados;
    if (!item) return;
    g_free(item->categoria);
    g_free(item->subcategoria);
    g_free(item->icone);
    g_free(item->nome);
    g_free(item->tipo);
    g_free(item->alvo);
    g_free(item);
}

static void abrir_externo(GtkWidget *btn, gpointer dados) {
    gchar *caminho = dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);
    
    g_print("[Fyd] Abrindo: %s\n", caminho);
    GError *erro = NULL;
    g_spawn_command_line_async(caminho, &erro);
    if (erro) {
        g_warning("[Fyd] Erro: %s", erro->message);
        g_error_free(erro);
    }
    g_free(caminho);
}

static void acao_desligar(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);
    g_print("[Fyd] ⏹️ Desligando...\n");
    GError *e = NULL;
    g_spawn_command_line_async("systemctl poweroff", &e);
    if (e) { g_warning("[Fyd] Erro desligar: %s", e->message); g_error_free(e); }
}

static void acao_reiniciar(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);
    g_print("[Fyd] 🔄 Reiniciando...\n");
    GError *e = NULL;
    g_spawn_command_line_async("systemctl reboot", &e);
    if (e) { g_warning("[Fyd] Erro reiniciar: %s", e->message); g_error_free(e); }
}

static void acao_hibernar(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);
    g_print("[Fyd] 💤 Hibernando...\n");
    GError *e = NULL;
    g_spawn_command_line_async("systemctl hibernate", &e);
    if (e) { g_warning("[Fyd] Erro hibernar: %s", e->message); g_error_free(e); }
}

static void acao_sair_sessao(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);
    g_print("[Fyd] 🚪 Saindo da sessão...\n");
    if (painel && painel->temporizador > 0)
        g_source_remove(painel->temporizador);
    g_application_quit(G_APPLICATION(fyd_area->app));
}

static void executar_interno(GtkWidget *btn, gpointer dados) {
    gchar *nome = dados;
    gtk_widget_set_visible(fyd_area->menu, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fyd_area->btn_iniciar), FALSE);

    if (g_strcmp0(nome, "explorador") == 0)
        fyd_explorador_abrir(NULL, NULL);
    else if (g_strcmp0(nome, "painel_controle") == 0)
        fyd_painel_controle_abrir(NULL, NULL);
    else if (g_strcmp0(nome, "sair_sessao") == 0)
        acao_sair_sessao(btn, NULL);
    else if (g_strcmp0(nome, "hibernar") == 0)
        acao_hibernar(btn, NULL);
    else if (g_strcmp0(nome, "reiniciar") == 0)
        acao_reiniciar(btn, NULL);
    else if (g_strcmp0(nome, "desligar") == 0)
        acao_desligar(btn, NULL);
    
    g_free(nome);
}

/* ==========================================
   🧹 LIMPAR ESPAÇOS
   ========================================== */
static void limpar_str(gchar *str) {
    if (!str) return;
    gchar *i = str, *o = str;
    while (*i == ' ' || *i == '\t') i++;
    if (*i == '\0') { *str = '\0'; return; }
    gchar *fim = i + strlen(i) - 1;
    while (fim > i && (*fim == ' ' || *fim == '\t' || *fim == '\n' || *fim == '\r'))
        *fim-- = '\0';
    if (i != o) g_memmove(o, i, strlen(i) + 1);
}

/* ==========================================
   📂 CAMINHO DO ARQUIVO — DIRETO DO REGISTRO
   ========================================== */
static gchar* obter_caminho_menu(void) {
    const gchar *pasta_menu = fyd_registro_ler("Caminhos/menu_dinamico", "/fyd/menu");
    return g_strdup_printf("%s/menu_principal.fydmenu", pasta_menu);
}

/* ==========================================
   📋 CARREGAR MENU COM CATEGORIAS
   ========================================== */
static void carregar_menu_arquivo(void) {
    g_list_free_full(itens_menu, liberar_item_menu);
    itens_menu = NULL;

    gchar *caminho = obter_caminho_menu();
    g_print("[Fyd] Lendo menu em: %s\n", caminho);

    if (!g_file_test(caminho, G_FILE_TEST_EXISTS)) {
        g_warning("[Fyd] ⚠️ Arquivo de menu não encontrado: %s\n", caminho);
        g_free(caminho);
        return;
    }

    FILE *arq = fopen(caminho, "r");
    if (!arq) { g_free(caminho); return; }

    gchar linha[512];
    gchar *categoria_atual = NULL;
    gchar *subcategoria_atual = NULL;

    while (fgets(linha, sizeof linha, arq)) {
        linha[strcspn(linha, "\r\n")] = 0;
        if (!linha[0] || linha[0] == '#') continue;

        // Separador visual
        if (strncmp(linha, "---", 3) == 0) {
            FydMenuItem *sep = g_new0(FydMenuItem, 1);
            sep->tipo = g_strdup("separador");
            sep->categoria = categoria_atual ? g_strdup(categoria_atual) : NULL;
            sep->subcategoria = subcategoria_atual ? g_strdup(subcategoria_atual) : NULL;
            itens_menu = g_list_append(itens_menu, sep);
            continue;
        }

        // Categoria: == Nome > Subnome
        if (g_str_has_prefix(linha, "==")) {
            g_free(categoria_atual);
            g_free(subcategoria_atual);
            
            gchar *nome_cat = linha + 2;
            limpar_str(nome_cat);
            
            gchar *seta = strstr(nome_cat, ">");
            if (seta) {
                *seta = '\0';
                categoria_atual = g_strdup(nome_cat);
                subcategoria_atual = g_strdup(seta + 1);
                limpar_str(categoria_atual);
                limpar_str(subcategoria_atual);
            } else {
                categoria_atual = g_strdup(nome_cat);
                subcategoria_atual = NULL;
            }
            continue;
        }

        // Item de menu: icone | nome | tipo | alvo
        if (!categoria_atual) continue;

        gchar **partes = g_strsplit(linha, "|", 4);
        if (!partes || !partes[0] || !partes[1] || !partes[2] || !partes[3]) {
            if (partes) g_strfreev(partes);
            continue;
        }

        FydMenuItem *item = g_new0(FydMenuItem, 1);
        item->categoria = g_strdup(categoria_atual);
        item->subcategoria = subcategoria_atual ? g_strdup(subcategoria_atual) : NULL;
        item->icone = g_strdup(g_strstrip(partes[0]));
        item->nome  = g_strdup(g_strstrip(partes[1]));
        item->tipo  = g_strdup(g_strstrip(partes[2]));
        item->alvo  = g_strdup(g_strstrip(partes[3]));
        itens_menu = g_list_append(itens_menu, item);
        g_strfreev(partes);
    }

    fclose(arq);
    g_free(caminho);
    g_free(categoria_atual);
    g_free(subcategoria_atual);
    
    g_print("[Fyd] ✅ Menu carregado: %d itens\n", g_list_length(itens_menu));
}

/* ==========================================
   🖼️ CRIAR WIDGET — VERSÃO SIMPLES (tudo junto)
   ========================================== */
GtkWidget* fyd_criar_menu(void) {
    carregar_menu_arquivo();
    
    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_name(caixa, "fyd-menu");

    // Tamanho fixo + alinhamento à esquerda
    gtk_widget_set_size_request(caixa, 280, -1);
    gtk_widget_set_hexpand(caixa, FALSE);
    gtk_widget_set_halign(caixa, GTK_ALIGN_START);

    gchar *ultima_cat = NULL;
    gchar *ultima_sub = NULL;

    for (GList *no = itens_menu; no; no = no->next) {
        FydMenuItem *item = no->data;

        // Separador
        if (g_strcmp0(item->tipo, "separador") == 0) {
            GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
            gtk_widget_add_css_class(sep, "fyd-separador");
            gtk_widget_set_margin_top(sep, 4);
            gtk_widget_set_margin_bottom(sep, 4);
            gtk_box_append(GTK_BOX(caixa), sep);
            continue;
        }

        // Cabeçalho de Categoria
        gboolean nova_cat = !ultima_cat || !item->categoria || g_strcmp0(ultima_cat, item->categoria) != 0;
        if (nova_cat && item->categoria) {
            GtkWidget *lbl_cat = gtk_label_new(NULL);
            gchar *texto = item->subcategoria 
                ? g_strdup_printf("<b>%s › %s</b>", item->categoria, item->subcategoria)
                : g_strdup_printf("<b>%s</b>", item->categoria);
            gtk_label_set_markup(GTK_LABEL(lbl_cat), texto);
            g_free(texto);
            gtk_label_set_xalign(GTK_LABEL(lbl_cat), 0.0);
            gtk_widget_set_margin_top(lbl_cat, 8);
            gtk_widget_set_margin_bottom(lbl_cat, 4);
            gtk_widget_set_margin_start(lbl_cat, 8);
            gtk_box_append(GTK_BOX(caixa), lbl_cat);
            g_free(ultima_cat);
            ultima_cat = g_strdup(item->categoria);
        }

        // Item do menu
        GtkWidget *linha = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_hexpand(linha, FALSE);
        gtk_widget_set_margin_start(linha, 8);
        gtk_widget_set_margin_end(linha, 8);
        gtk_widget_set_margin_top(linha, 5);
        gtk_widget_set_margin_bottom(linha, 5);

        GtkWidget *ico = gtk_label_new(item->icone);
        gtk_widget_set_size_request(ico, 24, -1);
        gtk_label_set_xalign(GTK_LABEL(ico), 0.5);

        GtkWidget *nome = gtk_label_new(item->nome);
        gtk_label_set_xalign(GTK_LABEL(nome), 0.0);
        gtk_widget_set_hexpand(nome, TRUE);

        gtk_box_append(GTK_BOX(linha), ico);
        gtk_box_append(GTK_BOX(linha), nome);

        GtkWidget *btn = gtk_button_new();
        gtk_button_set_child(GTK_BUTTON(btn), linha);
        gtk_widget_add_css_class(btn, "fyd-item-menu");
        gtk_widget_set_hexpand(btn, FALSE);

        if (g_strcmp0(item->tipo, "app") == 0)
            g_signal_connect(btn, "clicked", G_CALLBACK(abrir_externo), g_strdup(item->alvo));
        else if (g_strcmp0(item->tipo, "func") == 0)
            g_signal_connect(btn, "clicked", G_CALLBACK(executar_interno), g_strdup(item->alvo));

        gtk_box_append(GTK_BOX(caixa), btn);
    }

    g_free(ultima_cat);
    return caixa;
}

void fyd_menu_alternar(GtkWidget *botao, gpointer dados) {
    (void)dados;
    if (!fyd_area || !fyd_area->menu) {
        g_warning("[Fyd] ⚠️ Menu não inicializado!");
        return;
    }
    gboolean aberto = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(botao));
    gtk_widget_set_visible(fyd_area->menu, aberto);
    g_print("[Fyd] Menu %s\n", aberto ? "ABERTO ✅" : "FECHADO ❌");
}