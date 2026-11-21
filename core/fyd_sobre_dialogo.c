#include "fyd_sobre_dialogo.h"
#include "fyd_desktop.h"

// ==========================================
// 🔗 Abrir GitHub
// ==========================================
static void abrir_github(GtkWidget *btn, gpointer d) {
    (void)btn; (void)d;
    GtkUriLauncher *launcher = gtk_uri_launcher_new("https://github.com/fydelis2025");
    gtk_uri_launcher_launch(launcher, NULL, NULL, NULL, NULL);
    g_object_unref(launcher);
}

// ==========================================
// 🌐 Abrir Site do Projeto
// ==========================================
static void abrir_site(GtkWidget *btn, gpointer d) {
    (void)btn; (void)d;
    GtkUriLauncher *launcher = gtk_uri_launcher_new("https://www.fydelistechos.com.br");
    gtk_uri_launcher_launch(launcher, NULL, NULL, NULL, NULL);
    g_object_unref(launcher);
}

// ==========================================
// 🪟 Montar e Exibir Janela
// ==========================================
void fyd_sobre_abrir(GtkWidget *pai) {
    GtkWidget *janela = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(janela), "ℹ️ Sobre o FydelisTech OS");
    gtk_window_set_default_size(GTK_WINDOW(janela), 540, 500);
    gtk_window_set_modal(GTK_WINDOW(janela), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(janela), GTK_WINDOW(pai));

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_start(principal, 32);
    gtk_widget_set_margin_end(principal, 32);
    gtk_widget_set_margin_top(principal, 32);
    gtk_widget_set_margin_bottom(principal, 32);

    GtkWidget *lbl_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_titulo),
        "<span class='fyd-texto-sistema'>FydelisTech OS</span>");
    gtk_widget_set_halign(lbl_titulo, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(principal), lbl_titulo);

    GtkWidget *lbl_versao = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_versao),
        "<span class='fyd-texto-versao'>Versão 3.1 — CVE Shield • Segurança Integrada</span>");
    gtk_widget_set_halign(lbl_versao, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(lbl_versao, 8);
    gtk_box_append(GTK_BOX(principal), lbl_versao);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(sep, 12);
    gtk_widget_set_margin_bottom(sep, 20);
    gtk_box_append(GTK_BOX(principal), sep);

    const gchar *texto =
        "<p style='text-align:center; font-size:15px; line-height:1.8;'>"
        "🇧🇷 <b>Desenvolvido com paixão em Salvador, Bahia</b><br><br>"
        "Sistema operacional completo — do kernel à interface gráfica,<br>"
        "com foco em segurança, privacidade e liberdade digital.<br><br>"
        "<b>👤 Idealizador & Desenvolvedor Principal:</b><br>"
        "Adiel Santos Fontes<br><br>"
        "<b>🤝 Agradecimentos Especiais:</b><br>"
        "• À comunidade de tecnologia livre<br>"
        "• Aos amigos e familiares que apoiaram<br>"
        "• À Bahia e sua cultura que inspira<br>"
        "• A todos que acreditam no conhecimento compartilhado<br><br>"
        "🛡️ CVE Shield — Módulo de auditoria e proteção em tempo real<br>"
        "🔒 SQLCipher AES-256 — Dados sempre protegidos<br>"
        "💻 FydelTerm — Terminal nativo do sistema"
        "</p>";

    GtkWidget *lbl_texto = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_texto), texto);
    gtk_label_set_justify(GTK_LABEL(lbl_texto), GTK_JUSTIFY_CENTER);
    gtk_label_set_wrap(GTK_LABEL(lbl_texto), TRUE);
    gtk_widget_set_vexpand(lbl_texto, TRUE);
    gtk_box_append(GTK_BOX(principal), lbl_texto);

    GtkWidget *caixa_botoes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_halign(caixa_botoes, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(caixa_botoes, 10);

    GtkWidget *btn_github = gtk_button_new_with_label("🐙 GitHub");
    gtk_widget_add_css_class(btn_github, "fyd-botao-neon");
    gtk_widget_set_size_request(btn_github, 140, 48);
    g_signal_connect(btn_github, "clicked", G_CALLBACK(abrir_github), NULL);

    GtkWidget *btn_site = gtk_button_new_with_label("🌐 Projeto");
    gtk_widget_add_css_class(btn_site, "fyd-botao-neon");
    gtk_widget_set_size_request(btn_site, 140, 48);
    g_signal_connect(btn_site, "clicked", G_CALLBACK(abrir_site), NULL);

    GtkWidget *btn_fechar = gtk_button_new_with_label("Fechar");
    gtk_widget_add_css_class(btn_fechar, "fyd-icone-app");
    gtk_widget_set_size_request(btn_fechar, 120, 48);
    g_signal_connect_swapped(btn_fechar, "clicked",
        G_CALLBACK(gtk_window_destroy), janela);

    gtk_box_append(GTK_BOX(caixa_botoes), btn_github);
    gtk_box_append(GTK_BOX(caixa_botoes), btn_site);
    gtk_box_append(GTK_BOX(caixa_botoes), gtk_label_new(""));
    gtk_box_append(GTK_BOX(caixa_botoes), btn_fechar);

    gtk_box_append(GTK_BOX(principal), caixa_botoes);

    gtk_window_set_child(GTK_WINDOW(janela), principal);
    gtk_widget_set_visible(janela, TRUE);
}

