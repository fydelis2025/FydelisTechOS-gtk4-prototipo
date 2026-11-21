/**
 * @file fyd_area_trabalho.c
 * @brief Área de Trabalho — FydelisTech OS v4.2
 * Fundo de tela + Barra + Botão F + Ícones
 */
#include "fyd_desktop.h"

GtkWidget* fyd_criar_area_trabalho(void) {
    // ==========================================
    // CAMADA FUNDO — Imagem de fundo
    // ==========================================
    GtkWidget *overlay = gtk_overlay_new();

    // ✅ Imagem de fundo carregada do recurso
    GtkWidget *fundo = gtk_picture_new();
    gtk_picture_set_resource(GTK_PICTURE(fundo),
        "/com/fydelistech/recursos/background.png");
    gtk_widget_set_hexpand(fundo, TRUE);
    gtk_widget_set_vexpand(fundo, TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(fundo), GTK_CONTENT_FIT_COVER);
    gtk_widget_set_can_target(fundo, FALSE);  // Não bloqueia cliques
    gtk_overlay_set_child(GTK_OVERLAY(overlay), fundo);

    // ==========================================
    // CAMADA CONTEÚDO — Sobre o fundo
    // ==========================================
    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(principal, TRUE);
    gtk_widget_set_vexpand(principal, TRUE);
    gtk_widget_add_css_class(principal, "fyd-fundo-area");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), principal);

    // ==========================================
    // BARRA SUPERIOR
    // ==========================================
    GtkWidget *barra_sup = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_add_css_class(barra_sup, "fyd-barra-superior");
    fyd_area->barra_superior = barra_sup;

    // Botão Iniciar "F"
    GtkWidget *btn_menu = gtk_toggle_button_new_with_label("F");
    gtk_widget_set_size_request(btn_menu, 50, 50);
    gtk_widget_add_css_class(btn_menu, "fyd-botao-iniciar");
    fyd_area->btn_iniciar = btn_menu;
    g_signal_connect(btn_menu, "toggled", G_CALLBACK(fyd_menu_alternar), NULL);

    // Menu
    GtkWidget *menu = fyd_criar_menu();
    fyd_area->menu = menu;
    gtk_widget_set_visible(menu, FALSE);

    // Espaço que empurra pra direita
    GtkWidget *espaco = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_widget_set_hexpand(espaco, TRUE);

    // Painel de sistema
    GtkWidget *painel_sistema = fyd_criar_painel_sistema();

    // Montagem da barra
    gtk_box_append(GTK_BOX(barra_sup), btn_menu);
    gtk_box_append(GTK_BOX(barra_sup), espaco);
    gtk_box_append(GTK_BOX(barra_sup), painel_sistema);

    gtk_box_append(GTK_BOX(principal), barra_sup);
    gtk_box_append(GTK_BOX(principal), menu);

    // ==========================================
    // ÍCONES DOS APLICATIVOS — Centro da Tela
    // ==========================================
    GtkWidget *conteudo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_halign(conteudo, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(conteudo, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(conteudo, TRUE);

    // Writer
    GtkWidget *caixa_escritor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *btn_escritor = gtk_button_new_with_label("📝");
    gtk_widget_set_size_request(btn_escritor, 90, 90);
    gtk_widget_add_css_class(btn_escritor, "fyd-icone-app");
    gtk_box_append(GTK_BOX(caixa_escritor), btn_escritor);
    gtk_box_append(GTK_BOX(conteudo), caixa_escritor);

    // Calc
    GtkWidget *caixa_calc = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *btn_calc = gtk_button_new_with_label("📊");
    gtk_widget_set_size_request(btn_calc, 90, 90);
    gtk_widget_add_css_class(btn_calc, "fyd-icone-app");
    gtk_box_append(GTK_BOX(caixa_calc), btn_calc);
    gtk_box_append(GTK_BOX(conteudo), caixa_calc);

    // Slide
    GtkWidget *caixa_slide = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *btn_slide = gtk_button_new_with_label("📽️");
    gtk_widget_set_size_request(btn_slide, 90, 90);
    gtk_widget_add_css_class(btn_slide, "fyd-icone-app");
    gtk_box_append(GTK_BOX(caixa_slide), btn_slide);
    gtk_box_append(GTK_BOX(conteudo), caixa_slide);

    gtk_box_append(GTK_BOX(principal), conteudo);

    // ✅ Retorna o overlay (fundo + conteúdo)
    return overlay;
}