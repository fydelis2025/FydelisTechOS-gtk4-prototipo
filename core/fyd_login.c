/**
 * @file fyd_login.c
 * @brief Tela de Login — FydelisTech OS v4.2
 * ✅ Sem duplicatas · Chama fyd_usuarios.h
 * Salvador • Bahia 🇧🇷
 */

#include "fyd_desktop.h"
#include "fyd_registro.h"
#include "fyd_usuarios.h"  // ✅ Importante: puxa as funções daqui!
#include <string.h>

typedef struct {
    GtkEntry         *entrada_nome;
    GtkPasswordEntry *entrada_senha;
    GtkLabel         *lbl_aviso;
    gint             tentativas;
    gint             max_tentativas;
} FydCamposLogin;

// ==========================================
// CALLBACKS
// ==========================================
static gboolean cb_mudar_para_area_trabalho(gpointer dados) {
    (void)dados;
    fyd_tela_mudar(FYD_TELA_DESKTOP);
    return G_SOURCE_REMOVE;
}

// ==========================================
// VERIFICAR ACESSO
// ==========================================
static void fyd_verificar_acesso(GtkWidget *botao, gpointer dados) {
    (void)botao;
    FydCamposLogin *campos = dados;

    const gchar *nome = gtk_editable_get_text(GTK_EDITABLE(campos->entrada_nome));
    const gchar *senha = gtk_editable_get_text(GTK_EDITABLE(campos->entrada_senha));

    if (!nome || !*nome || !senha || !*senha) {
        gtk_label_set_text(campos->lbl_aviso, "⚠️ Preencha usuário e senha");
        return;
    }

    if (campos->max_tentativas > 0 && campos->tentativas >= campos->max_tentativas) {
        gtk_label_set_text(campos->lbl_aviso, "🔒 Muitas tentativas — sistema bloqueado");
        gtk_widget_set_sensitive(GTK_WIDGET(botao), FALSE);
        return;
    }

    // ✅ Chama de fyd_usuarios.c — NÃO redefine aqui!
    FydUsuario *lista = NULL;
    int qtd = fyd_usuario_listar(&lista);

    if (qtd == 0) {
        if (fyd_usuario_criar(nome, senha, 1)) {
            g_message("✅ Usuário admin criado: %s", nome);
            gtk_label_set_text(campos->lbl_aviso, "✅ Bem-vindo! Configurando área de trabalho...");
            g_timeout_add(800, cb_mudar_para_area_trabalho, NULL);
            g_free(campos);
            g_clear_pointer(&lista, g_free);
            return;
        } else {
            gtk_label_set_text(campos->lbl_aviso, "❌ Erro ao criar usuário");
            g_clear_pointer(&lista, g_free);
            return;
        }
    }

    g_clear_pointer(&lista, g_free);

    if (fyd_usuario_autenticar(nome, senha)) {
        gtk_label_set_text(campos->lbl_aviso, "✅ Entrando...");
        g_timeout_add(600, cb_mudar_para_area_trabalho, NULL);
        g_free(campos);
    } else {
        campos->tentativas++;
        gchar *aviso = g_strdup_printf(
            "❌ Usuário ou senha incorretos (%d/%d)",
            campos->tentativas, campos->max_tentativas);
        gtk_label_set_text(campos->lbl_aviso, aviso);
        g_free(aviso);
    }
}

// ==========================================
// CRIAR TELA DE LOGIN
// ==========================================
GtkWidget* fyd_criar_tela_login(void) {
    GtkWidget *overlay = gtk_overlay_new();

    // Imagem de fundo
    GtkWidget *fundo_img = gtk_picture_new();
    gtk_picture_set_resource(GTK_PICTURE(fundo_img),
        "/com/fydelistech/recursos/fundo_login.png");
    gtk_widget_set_hexpand(fundo_img, TRUE);
    gtk_widget_set_vexpand(fundo_img, TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(fundo_img), GTK_CONTENT_FIT_COVER);
    gtk_widget_set_can_target(fundo_img, FALSE);
    gtk_overlay_set_child(GTK_OVERLAY(overlay), fundo_img);

    GtkWidget *conteudo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(conteudo, TRUE);
    gtk_widget_set_vexpand(conteudo, TRUE);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), conteudo);

    // Caixa central com vidro
    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_size_request(caixa, 420, 440);
    gtk_widget_add_css_class(caixa, "fyd-caixa-vidro");
    gtk_widget_set_halign(caixa, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(caixa, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(caixa, TRUE);

    GtkWidget *lbl_f = gtk_label_new("F");
    gtk_widget_add_css_class(lbl_f, "fyd-logo-pequeno");
    gtk_box_append(GTK_BOX(caixa), lbl_f);

    GtkWidget *lbl_bemvindo = gtk_label_new("Bem-vindo ao FydelisTech OS");
    gtk_widget_add_css_class(lbl_bemvindo, "fyd-texto-titulo");
    gtk_box_append(GTK_BOX(caixa), lbl_bemvindo);

    GtkWidget *entrada_nome = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_nome), "Nome de usuário");
    gtk_widget_set_size_request(entrada_nome, 300, 48);
    gtk_widget_add_css_class(entrada_nome, "fyd-entrada");
    gtk_box_append(GTK_BOX(caixa), entrada_nome);

    GtkWidget *entrada_senha = gtk_password_entry_new();
    gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(entrada_senha), TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_senha), "Senha");
    gtk_widget_set_size_request(entrada_senha, 300, 48);
    gtk_widget_add_css_class(entrada_senha, "fyd-entrada");
    gtk_box_append(GTK_BOX(caixa), entrada_senha);

    GtkWidget *lbl_aviso = gtk_label_new("");
    gtk_widget_add_css_class(lbl_aviso, "fyd-texto-status");
    gtk_box_append(GTK_BOX(caixa), lbl_aviso);

    GtkWidget *btn_entrar = gtk_button_new_with_label("ENTRAR");
    gtk_widget_set_size_request(btn_entrar, 300, 50);
    gtk_widget_add_css_class(btn_entrar, "fyd-botao-neon");

    FydCamposLogin *campos = g_new0(FydCamposLogin, 1);
    campos->entrada_nome = GTK_ENTRY(entrada_nome);
    campos->entrada_senha = GTK_PASSWORD_ENTRY(entrada_senha);
    campos->lbl_aviso    = GTK_LABEL(lbl_aviso);
    campos->tentativas   = 0;
    campos->max_tentativas = fyd_registro_ler_int("Seguranca", "bloquear_tentativas", 5);

    g_signal_connect(btn_entrar, "clicked", G_CALLBACK(fyd_verificar_acesso), campos);
    gtk_box_append(GTK_BOX(caixa), btn_entrar);

    gtk_box_append(GTK_BOX(conteudo), caixa);

    return overlay;
}

