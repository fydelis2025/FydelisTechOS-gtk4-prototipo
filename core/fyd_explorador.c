/**
 * @file fyd_explorador.c
 * @brief Explorador de Arquivos — FydelisTech OS v4.0
 * ⚠️ SISTEMA PRÓPRIO · Padrão /fyd /usuarios /dados /secure
 */
#include "fyd_desktop.h"
#include "fyd_registro.h"
#include <glib.h>

static GtkWidget *janela_explorador = NULL;
static GtkWidget *grade_arquivos = NULL;

// ✅ CAMINHOS DO REGISTRO — Única fonte de verdade
static const gchar* caminho_raiz(void) {
    return fyd_registro_ler("Caminhos/raiz_sistema", "/");
}
static const gchar* caminho_sistema(void) {
    return fyd_registro_ler("Caminhos/raiz_sistema", "/fyd");
}
static const gchar* caminho_usuarios(void) {
    return fyd_registro_ler("Caminhos/perfis_usuarios", "/usuarios");
}
static const gchar* caminho_dados(void) {
    return fyd_registro_ler("Caminhos/dados_compartilhados", "/dados");
}
static const gchar* caminho_lixeira(void) {
    return "/fyd/lixeira";
}

static gchar caminho_atual[512] = "/fyd"; // Abre no núcleo do sistema

static void carregar_pasta(const gchar *caminho);

// 🧹 Limpar
static void limpar_filhos(GtkWidget *container) {
    GtkWidget *filho;
    while ((filho = gtk_widget_get_first_child(container)) != NULL) {
        gtk_widget_unparent(filho);
    }
}

// 📂 Listar arquivos
static void carregar_pasta(const gchar *caminho) {
    if (!grade_arquivos) return;
    limpar_filhos(grade_arquivos);

    // Botão voltar
    GtkWidget *btn_voltar = gtk_button_new_with_label("⬆ ..");
    gtk_widget_add_css_class(btn_voltar, "fyd-icone-app");
    gchar *pai = g_path_get_dirname(caminho);
    g_signal_connect_swapped(btn_voltar, "clicked",
        G_CALLBACK(carregar_pasta), pai);
    gtk_grid_attach(GTK_GRID(grade_arquivos), btn_voltar, 0, 0, 1, 1);

    // Atalhos rápidos
    if (g_strcmp0(caminho, "/fyd") == 0) {
        GtkWidget *btn_usu = gtk_button_new_with_label("👤 /usuarios");
        gtk_widget_add_css_class(btn_usu, "fyd-icone-app");
        g_signal_connect_swapped(btn_usu, "clicked",
            G_CALLBACK(carregar_pasta), g_strdup("/usuarios"));
        gtk_grid_attach(GTK_GRID(grade_arquivos), btn_usu, 1, 0, 1, 1);

        GtkWidget *btn_dad = gtk_button_new_with_label("📂 /dados");
        gtk_widget_add_css_class(btn_dad, "fyd-icone-app");
        g_signal_connect_swapped(btn_dad, "clicked",
            G_CALLBACK(carregar_pasta), g_strdup("/dados"));
        gtk_grid_attach(GTK_GRID(grade_arquivos), btn_dad, 2, 0, 1, 1);

        GtkWidget *btn_lix = gtk_button_new_with_label("🗑 Lixeira");
        gtk_widget_add_css_class(btn_lix, "fyd-icone-app");
        g_signal_connect_swapped(btn_lix, "clicked",
            G_CALLBACK(carregar_pasta), g_strdup("/fyd/lixeira"));
        gtk_grid_attach(GTK_GRID(grade_arquivos), btn_lix, 3, 0, 1, 1);
    }

    // Lê arquivos da pasta
    GDir *dir = g_dir_open(caminho, 0, NULL);
    if (!dir) { g_free(pai); return; }

    gint col = 1, lin = 1;
    const gchar *nome;
    while ((nome = g_dir_read_name(dir))) {
        if (nome[0] == '.') continue;

        gchar *completo = g_build_filename(caminho, nome, NULL);
        gboolean e_pasta = g_file_test(completo, G_FILE_TEST_IS_DIR);
        const gchar *icone = e_pasta ? "📁" : "📄";

        GtkWidget *btn = gtk_button_new_with_label(icone);
        gtk_widget_set_tooltip_text(btn, nome);
        gtk_widget_add_css_class(btn, "fyd-icone-app");

        if (e_pasta) {
            gchar *novo_caminho = g_build_filename(caminho, nome, NULL);
            g_signal_connect_swapped(btn, "clicked",
                G_CALLBACK(carregar_pasta), novo_caminho);
        }

        gtk_grid_attach(GTK_GRID(grade_arquivos), btn, col, lin, 1, 1);
        col++;
        if (col >= 8) { col = 1; lin++; }
        g_free(completo);
    }
    g_dir_close(dir);
    g_free(pai);
    g_strlcpy(caminho_atual, caminho, sizeof caminho_atual);
}

// 🪟 Abrir janela
void fyd_explorador_abrir(GtkWidget *item, gpointer dados) {
    (void)item; (void)dados;

    if (janela_explorador) {
        gtk_window_present(GTK_WINDOW(janela_explorador));
        return;
    }

    janela_explorador = gtk_application_window_new(fyd_area->app);
    gtk_window_set_title(GTK_WINDOW(janela_explorador), "Explorador — FydelisTech OS");
    gtk_window_set_default_size(GTK_WINDOW(janela_explorador), 900, 600);
    gtk_window_set_transient_for(GTK_WINDOW(janela_explorador), GTK_WINDOW(fyd_area->janela));

    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(caixa, 16);
    gtk_widget_set_margin_start(caixa, 16);
    gtk_widget_set_margin_end(caixa, 16);
    gtk_widget_set_margin_bottom(caixa, 16);

    GtkWidget *lbl_caminho = gtk_label_new(caminho_atual);
    gtk_widget_set_halign(lbl_caminho, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(caixa), lbl_caminho);

    grade_arquivos = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grade_arquivos), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grade_arquivos), 12);
    gtk_widget_set_vexpand(grade_arquivos, TRUE);
    gtk_box_append(GTK_BOX(caixa), grade_arquivos);

    gtk_window_set_child(GTK_WINDOW(janela_explorador), caixa);

    g_signal_connect(janela_explorador, "destroy",
        G_CALLBACK(gtk_window_destroy), NULL);

    carregar_pasta(caminho_atual);
    gtk_window_present(GTK_WINDOW(janela_explorador));
}