/**
 * @file fyd_splash.c
 * @brief Tela de Carregamento — FydelisTech OS v3.2
 */
#include "fyd_desktop.h"

typedef struct {
    GtkProgressBar *barra;
    double valor;
} FydProgressoDados;

static gboolean fyd_aplicar_progresso(FydProgressoDados *d) {
    gtk_progress_bar_set_fraction(d->barra, d->valor);
    g_free(d);
    return FALSE;
}

static gboolean fyd_ir_para_login(gpointer dados) {
    FydTela *alvo = dados;
    fyd_tela_mudar(*alvo);
    g_free(alvo);
    return FALSE;
}

static GtkWidget* fyd_criar_logo(void) {
    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(caixa, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(caixa, GTK_ALIGN_CENTER);

    GtkWidget *lbl_letra = gtk_label_new("F");
    gtk_widget_add_css_class(lbl_letra, "fyd-logo-grande");
    gtk_box_append(GTK_BOX(caixa), lbl_letra);

    GtkWidget *lbl_nome = gtk_label_new("FYDELIS TECH OS");
    gtk_widget_add_css_class(lbl_nome, "fyd-texto-sistema");
    gtk_box_append(GTK_BOX(caixa), lbl_nome);

    GtkWidget *lbl_versao = gtk_label_new("v3.2 • Salvador • Bahia 🇧🇷");
    gtk_widget_add_css_class(lbl_versao, "fyd-texto-versao");
    gtk_box_append(GTK_BOX(caixa), lbl_versao);

    return caixa;
}

// ✅ NOME CORRETO
GtkWidget* fyd_criar_tela_inicial(void) {
    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 32);
    gtk_widget_set_size_request(caixa, 500, 320);
    gtk_widget_add_css_class(caixa, "fyd-splash-fundo");
    gtk_widget_set_halign(caixa, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(caixa, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(caixa), fyd_criar_logo());

    GtkWidget *barra = gtk_progress_bar_new();
    gtk_widget_set_size_request(barra, 300, 8);
    gtk_widget_add_css_class(barra, "fyd-barra-carregando");
    gtk_box_append(GTK_BOX(caixa), barra);

    GtkWidget *lbl_status = gtk_label_new("Inicializando sistema...");
    gtk_widget_add_css_class(lbl_status, "fyd-texto-status");
    gtk_box_append(GTK_BOX(caixa), lbl_status);

    // Progresso em etapas
    FydProgressoDados *p1 = g_new0(FydProgressoDados, 1);
    p1->barra = GTK_PROGRESS_BAR(barra); p1->valor = 0.20;
    g_timeout_add(500,  (GSourceFunc)fyd_aplicar_progresso, p1);

    FydProgressoDados *p2 = g_new0(FydProgressoDados, 1);
    p2->barra = GTK_PROGRESS_BAR(barra); p2->valor = 0.45;
    g_timeout_add(1000, (GSourceFunc)fyd_aplicar_progresso, p2);

    FydProgressoDados *p3 = g_new0(FydProgressoDados, 1);
    p3->barra = GTK_PROGRESS_BAR(barra); p3->valor = 0.75;
    g_timeout_add(1500, (GSourceFunc)fyd_aplicar_progresso, p3);

    FydProgressoDados *p4 = g_new0(FydProgressoDados, 1);
    p4->barra = GTK_PROGRESS_BAR(barra); p4->valor = 1.00;
    g_timeout_add(2000, (GSourceFunc)fyd_aplicar_progresso, p4);

    FydTela *proxima = g_new0(FydTela, 1);
    *proxima = FYD_TELA_LOGIN;
    g_timeout_add(2500, (GSourceFunc)fyd_ir_para_login, proxima);

    return caixa;
}
