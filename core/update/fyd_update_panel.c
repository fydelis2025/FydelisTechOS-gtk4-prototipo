#include "fyd_update_panel.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>

#define COR_FUNDO        "#0F051F"
#define COR_CARD         "#2A1050"
#define COR_ROXO_NEON    "#9D4EDD"
#define COR_TEXTO        "#E0D7FF"
#define COR_SUCESSO      "#00FF88"
#define COR_ALERTA       "#FF9900"
#define COR_ERRO         "#FF4444"

struct FydUpdatePainel {
    GtkWidget *raiz;
    GtkWidget *lbl_status;
    GtkWidget *lbl_versao_atual;
    GtkWidget *lbl_versao_nova;
    GtkWidget *lbl_progresso;
    GtkWidget *barra_progresso;
    GtkWidget *btn_verificar;
    GtkWidget *btn_baixar;
    GtkWidget *btn_instalar;
    gchar *versao_atual;
    gchar *versao_nova;
    gchar *url_download;
    gchar *caminho_arquivo;
    gint progresso_porcento;
    gboolean baixando;
    gboolean pronto_para_instalar;
};

static void aplicar_estilo(void) {
    static gboolean carregado = FALSE;
    if (carregado) return;
    carregado = TRUE;

    GtkCssProvider *provedor = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provedor,
        "window { background: linear-gradient(135deg, #0F051F, #2A1050); color: #E0D7FF; }"
        ".fyd-card { background: rgba(42, 16, 80, 0.75); border: 1px solid rgba(157, 78, 221, 0.4); border-radius: 16px; padding: 24px; }"
        ".fyd-titulo { font-size: 22px; font-weight: 900; color: #C77DFF; }"
        ".fyd-botao { background: linear-gradient(90deg, #7B2CBF, #9D4EDD); color: white; border-radius: 10px; padding: 10px 20px; font-weight: bold; }"
        ".fyd-botao:hover { background: linear-gradient(90deg, #9D4EDD, #C77DFF); }"
        ".fyd-botao:disabled { background: #444; color: #888; }"
        ".fyd-progresso { background: #1A0B2E; border-radius: 8px; min-height: 12px; }"
        ".fyd-progresso > trough > progress { background: linear-gradient(90deg, #00FF88, #00DD77); border-radius: 8px; }"
    );
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provedor),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provedor);
}

static void atualizar_status(FydUpdatePainel *p, const gchar *texto, const gchar *cor) {
    if (!p) return;
    gchar *marca = g_strdup_printf(
        "<span font-weight='bold' color='%s'>%s</span>", cor, texto);
    gtk_label_set_markup(GTK_LABEL(p->lbl_status), marca);
    g_free(marca);
}

static void atualizar_progresso(FydUpdatePainel *p, int porcento) {
    if (!p) return;
    p->progresso_porcento = porcento;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(p->barra_progresso), porcento / 100.0);
    gchar *texto = g_strdup_printf("Baixando... %d%%", porcento);
    gtk_label_set_text(GTK_LABEL(p->lbl_progresso), texto);
    g_free(texto);
}

// ✅ Callback no formato CERTO do GTK 4 — GAsyncReadyCallback
static void resposta_dialogo(GObject *obj, GAsyncResult *res, gpointer dados) {
    GtkAlertDialog *dialogo = GTK_ALERT_DIALOG(obj);
    FydUpdatePainel *p = dados;
    
    gint resposta = gtk_alert_dialog_choose_finish(dialogo, res, NULL);
    
    if (resposta == 1) {
        atualizar_status(p, "📦 Iniciando instalação...", COR_ALERTA);
    } else {
        atualizar_status(p, "❌ Instalação cancelada", COR_TEXTO);
    }
    
    g_object_unref(dialogo);
}

// ✅ Callback no formato CERTO do g_timeout_add — retorna gboolean
static gboolean simular_progresso(gpointer dados) {
    FydUpdatePainel *p = dados;
    if (!p || !p->baixando) return FALSE; // Para o timer
    
    p->progresso_porcento += 5;
    if (p->progresso_porcento >= 100) {
        p->progresso_porcento = 100;
        atualizar_progresso(p, 100);
        fyd_update_painel_download_concluido(p);
        return FALSE; // Para o timer
    }
    
    atualizar_progresso(p, p->progresso_porcento);
    return TRUE; // Continua rodando
}

void fyd_update_painel_download_concluido(FydUpdatePainel *p) {
    if (!p) return;
    
    p->baixando = FALSE;
    p->pronto_para_instalar = TRUE;
    
    atualizar_status(p, "✅ Download concluído! Pronto para instalar.", COR_SUCESSO);
    gtk_widget_set_sensitive(p->btn_baixar, FALSE);
    gtk_widget_set_sensitive(p->btn_instalar, TRUE);
    
    // ✅ GTK 4 — Tipo correto
    GtkAlertDialog *dialogo = gtk_alert_dialog_new("Deseja instalar a atualização agora?");
    
    gtk_alert_dialog_set_buttons(dialogo, (const char *[]){
        "Não",
        "Instalar",
        NULL
    });
    
    gtk_alert_dialog_set_cancel_button(dialogo, 0);
    gtk_alert_dialog_set_default_button(dialogo, 1);
    
    // ✅ Callback no formato certo
    gtk_alert_dialog_choose(dialogo, GTK_WINDOW(gtk_widget_get_root(p->raiz)),
                            NULL, resposta_dialogo, p);
}

void fyd_update_painel_verificar_atualizacoes(GtkWidget *btn, gpointer dados) {
    FydUpdatePainel *p = dados;
    (void)btn; // ✅ Elimina aviso de parâmetro não usado
    if (!p) return;
    
    atualizar_status(p, "🔘 Verificando atualizações...", COR_ALERTA);
    gtk_widget_set_sensitive(p->btn_verificar, FALSE);
    
    p->versao_nova = g_strdup("3.3.0");
    p->url_download = g_strdup("https://fydelistechos.system/atualizacao/fyd-update-v3.3.0.fydpkg");
    
    gchar *info = g_strdup_printf("Nova versão disponível: <b>%s</b>", p->versao_nova);
    gtk_label_set_markup(GTK_LABEL(p->lbl_versao_nova), info);
    g_free(info);
    
    atualizar_status(p, "✅ Atualização detectada!", COR_SUCESSO);
    gtk_widget_set_sensitive(p->btn_baixar, TRUE);
    gtk_widget_set_sensitive(p->btn_verificar, TRUE);
}

void fyd_update_painel_iniciar_download(GtkWidget *btn, gpointer dados) {
    FydUpdatePainel *p = dados;
    (void)btn;
    if (!p || !p->url_download) return;
    
    p->baixando = TRUE;
    p->progresso_porcento = 0;
    
    atualizar_status(p, "⬇️ Baixando atualização...", COR_ALERTA);
    gtk_widget_set_sensitive(p->btn_baixar, FALSE);
    gtk_widget_set_sensitive(p->btn_instalar, FALSE);
    
    g_timeout_add(100, simular_progresso, p);
}

void fyd_update_painel_instalar(GtkWidget *btn, gpointer dados) {
    FydUpdatePainel *p = dados;
    (void)btn;
    if (!p || !p->pronto_para_instalar) return;
    
    atualizar_status(p, "🔧 Instalando...", COR_ALERTA);
    gtk_widget_set_sensitive(p->btn_instalar, FALSE);
}

GtkWidget* fyd_update_painel_novo(void) {
    aplicar_estilo();

    FydUpdatePainel *p = g_new0(FydUpdatePainel, 1);
    p->versao_atual = g_strdup("3.2.0");
    p->versao_nova = NULL;
    p->url_download = NULL;
    p->caminho_arquivo = NULL;
    p->baixando = FALSE;
    p->pronto_para_instalar = FALSE;

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_margin_start(principal, 28);
    gtk_widget_set_margin_end(principal, 28);
    gtk_widget_set_margin_top(principal, 28);
    gtk_widget_set_margin_bottom(principal, 28);
    g_object_set_data_full(G_OBJECT(principal), "painel", p, g_free);

    GtkWidget *titulo = gtk_label_new("🔄 FydelisTech — Atualizações do Sistema");
    gtk_widget_add_css_class(titulo, "fyd-titulo");
    gtk_widget_set_halign(titulo, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(principal), titulo);

    GtkWidget *caixa_atual = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *lbl_atual_texto = gtk_label_new("Versão Instalada:");
    gtk_widget_set_halign(lbl_atual_texto, GTK_ALIGN_START);
    p->lbl_versao_atual = gtk_label_new(NULL);
    gchar *txt_atual = g_strdup_printf("<b>%s</b>", p->versao_atual);
    gtk_label_set_markup(GTK_LABEL(p->lbl_versao_atual), txt_atual);
    g_free(txt_atual);
    gtk_box_append(GTK_BOX(caixa_atual), lbl_atual_texto);
    gtk_box_append(GTK_BOX(caixa_atual), p->lbl_versao_atual);
    gtk_box_append(GTK_BOX(principal), caixa_atual);

    GtkWidget *caixa_nova = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *lbl_nova_texto = gtk_label_new("Versão Disponível:");
    gtk_widget_set_halign(lbl_nova_texto, GTK_ALIGN_START);
    p->lbl_versao_nova = gtk_label_new("<i>Nenhuma verificação feita ainda</i>");
    gtk_label_set_use_markup(GTK_LABEL(p->lbl_versao_nova), TRUE);
    gtk_box_append(GTK_BOX(caixa_nova), lbl_nova_texto);
    gtk_box_append(GTK_BOX(caixa_nova), p->lbl_versao_nova);
    gtk_box_append(GTK_BOX(principal), caixa_nova);

    p->lbl_progresso = gtk_label_new("Aguardando...");
    gtk_widget_set_halign(p->lbl_progresso, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(principal), p->lbl_progresso);

    p->barra_progresso = gtk_progress_bar_new();
    gtk_widget_add_css_class(p->barra_progresso, "fyd-progresso");
    gtk_widget_set_valign(p->barra_progresso, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(p->barra_progresso, TRUE);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(p->barra_progresso), 0.0);
    gtk_box_append(GTK_BOX(principal), p->barra_progresso);

    p->lbl_status = gtk_label_new("<span color='#888'>Pronto para verificar atualizações</span>");
    gtk_label_set_use_markup(GTK_LABEL(p->lbl_status), TRUE);
    gtk_box_append(GTK_BOX(principal), p->lbl_status);

    GtkWidget *caixa_botoes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    
    p->btn_verificar = gtk_button_new_with_label("🔍 Verificar Atualizações");
    gtk_widget_add_css_class(p->btn_verificar, "fyd-botao");
    g_signal_connect(p->btn_verificar, "clicked", 
                     G_CALLBACK(fyd_update_painel_verificar_atualizacoes), p);
    
    p->btn_baixar = gtk_button_new_with_label("⬇️ Baixar Atualização");
    gtk_widget_add_css_class(p->btn_baixar, "fyd-botao");
    gtk_widget_set_sensitive(p->btn_baixar, FALSE);
    g_signal_connect(p->btn_baixar, "clicked",
                     G_CALLBACK(fyd_update_painel_iniciar_download), p);
    
    p->btn_instalar = gtk_button_new_with_label("🔧 Instalar");
    gtk_widget_add_css_class(p->btn_instalar, "fyd-botao");
    gtk_widget_set_sensitive(p->btn_instalar, FALSE);
    g_signal_connect(p->btn_instalar, "clicked",
                     G_CALLBACK(fyd_update_painel_instalar), p);

    gtk_box_append(GTK_BOX(caixa_botoes), p->btn_verificar);
    gtk_box_append(GTK_BOX(caixa_botoes), p->btn_baixar);
    gtk_box_append(GTK_BOX(caixa_botoes), p->btn_instalar);
    gtk_box_append(GTK_BOX(principal), caixa_botoes);

    p->raiz = principal;
    return principal;
}

