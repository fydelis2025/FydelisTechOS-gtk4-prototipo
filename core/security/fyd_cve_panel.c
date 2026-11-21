#include "fyd_cve_panel.h"
#include <string.h>
#include <time.h>
#include <glib.h>

#define COR_FUNDO        "#0F051F"
#define COR_CARD         "#2A1050"
#define COR_BORDA        "#5E35B1"
#define COR_TEXTO        "#E0D7FF"
#define COR_TEXTO_CLARO  "#FFFFFF"
#define COR_STATUS_OK    "#00FF88"
#define COR_RISCO_CRIT   "#FF4444"
#define COR_RISCO_ALTO   "#FF9900"
#define COR_RISCO_MEDIO  "#FFDD00"
#define COR_RISCO_BAIXO  "#88CC88"

// ✅ Estrutura definida aqui
struct FydCVEPainel {
    GtkWidget *raiz;
    GtkWidget *lbl_status;
    GtkWidget *lbl_critico;
    GtkWidget *lbl_alto;
    GtkWidget *lbl_medio;
    GtkWidget *lbl_total;
    GtkWidget *container_lista;
    int cont_crit;
    int cont_alto;
    int cont_medio;
};

static const gchar* cor_para_nivel(FydCVENivel n) {
    switch (n) {
        case FYD_CVE_CRITICA: return COR_RISCO_CRIT;
        case FYD_CVE_ALTA:    return COR_RISCO_ALTO;
        case FYD_CVE_MEDIA:   return COR_RISCO_MEDIO;
        case FYD_CVE_BAIXA:   return COR_RISCO_BAIXO;
        default:              return COR_STATUS_OK;
    }
}

static const gchar* texto_risco(FydCVENivel n) {
    switch (n) {
        case FYD_CVE_CRITICA: return "CRÍTICO";
        case FYD_CVE_ALTA:    return "ALTO";
        case FYD_CVE_MEDIA:   return "MÉDIO";
        case FYD_CVE_BAIXA:   return "BAIXO";
        default:              return "OK";
    }
}

static void aplicar_estilo(void) {
    static gboolean carregado = FALSE;
    if (carregado) return;
    carregado = TRUE;

    GtkCssProvider *provedor = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provedor,
        "window { background: linear-gradient(135deg, #0F051F, #2A1050); color: #E0D7FF; }"
        ".fyd-card { background: rgba(42, 16, 80, 0.75); border: 1px solid rgba(157, 78, 221, 0.4); border-radius: 16px; padding: 20px; }"
        ".fyd-titulo { font-size: 20px; font-weight: 900; color: #C77DFF; }"
        ".fyd-contador { font-size: 16px; font-weight: bold; padding: 8px 12px; border-radius: 8px; background: rgba(15, 5, 31, 0.5); }"
        ".fyd-lista { background: rgba(15, 5, 31, 0.5); border: 1px solid rgba(94, 53, 177, 0.3); border-radius: 12px; padding: 12px; font-family: 'Consolas', monospace; }"
        ".fyd-rotulo { font-size: 14px; color: #E0D7FF; }"
    );
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provedor),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provedor);
}

GtkWidget* fyd_cve_painel_novo(void) {
    aplicar_estilo();

    FydCVEPainel *painel = g_new0(FydCVEPainel, 1);
    painel->cont_crit = painel->cont_alto = painel->cont_medio = 0;

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_start(principal, 24);
    gtk_widget_set_margin_end(principal, 24);
    gtk_widget_set_margin_top(principal, 24);
    gtk_widget_set_margin_bottom(principal, 24);
    g_object_set_data_full(G_OBJECT(principal), "painel", painel, g_free);

    GtkWidget *titulo = gtk_label_new("🛡️ FydelisTech CVE Shield — Auditoria de Segurança");
    gtk_widget_add_css_class(titulo, "fyd-titulo");
    gtk_widget_set_halign(titulo, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(principal), titulo);

    painel->lbl_status = gtk_label_new(NULL);
    fyd_cve_painel_definir_status(painel, "✅ SISTEMA MONITORADO — Auditoria Ativa", COR_STATUS_OK);
    gtk_box_append(GTK_BOX(principal), painel->lbl_status);

    GtkWidget *caixa_cont = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    
    painel->lbl_critico = gtk_label_new("<span color='" COR_RISCO_CRIT "'>🔴 Crítico: 0</span>");
    painel->lbl_alto = gtk_label_new("<span color='" COR_RISCO_ALTO "'>🟠 Alto: 0</span>");
    painel->lbl_medio = gtk_label_new("<span color='" COR_RISCO_MEDIO "'>🟡 Médio: 0</span>");
    painel->lbl_total = gtk_label_new("<span color='" COR_TEXTO_CLARO "'>📋 Total: 0</span>");

    gtk_label_set_use_markup(GTK_LABEL(painel->lbl_critico), TRUE);
    gtk_label_set_use_markup(GTK_LABEL(painel->lbl_alto), TRUE);
    gtk_label_set_use_markup(GTK_LABEL(painel->lbl_medio), TRUE);
    gtk_label_set_use_markup(GTK_LABEL(painel->lbl_total), TRUE);

    gtk_widget_add_css_class(painel->lbl_critico, "fyd-contador");
    gtk_widget_add_css_class(painel->lbl_alto, "fyd-contador");
    gtk_widget_add_css_class(painel->lbl_medio, "fyd-contador");
    gtk_widget_add_css_class(painel->lbl_total, "fyd-contador");

    gtk_box_append(GTK_BOX(caixa_cont), painel->lbl_critico);
    gtk_box_append(GTK_BOX(caixa_cont), painel->lbl_alto);
    gtk_box_append(GTK_BOX(caixa_cont), painel->lbl_medio);
    gtk_box_append(GTK_BOX(caixa_cont), painel->lbl_total);
    
    gtk_box_append(GTK_BOX(principal), caixa_cont);

    GtkWidget *lbl_lista = gtk_label_new("📋 Últimos Eventos Detectados:");
    gtk_widget_set_halign(lbl_lista, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl_lista, 12);
    gtk_box_append(GTK_BOX(principal), lbl_lista);

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);
    painel->container_lista = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_add_css_class(painel->container_lista, "fyd-lista");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), painel->container_lista);
    gtk_box_append(GTK_BOX(principal), scrolled);

    painel->raiz = principal;
    return principal;
}

void fyd_cve_painel_atualizar_contadores(FydCVEPainel *p, int crit, int alto, int med) {
    if (!p) return;
    p->cont_crit = crit;
    p->cont_alto = alto;
    p->cont_medio = med;
    int total = crit + alto + med;

    gchar *txt_crit = g_strdup_printf("<span color='" COR_RISCO_CRIT "'>🔴 Crítico: %d</span>", crit);
    gchar *txt_alto = g_strdup_printf("<span color='" COR_RISCO_ALTO "'>🟠 Alto: %d</span>", alto);
    gchar *txt_med = g_strdup_printf("<span color='" COR_RISCO_MEDIO "'>🟡 Médio: %d</span>", med);
    gchar *txt_tot = g_strdup_printf("<span color='" COR_TEXTO_CLARO "'>📋 Total: %d</span>", total);

    gtk_label_set_markup(GTK_LABEL(p->lbl_critico), txt_crit);
    gtk_label_set_markup(GTK_LABEL(p->lbl_alto), txt_alto);
    gtk_label_set_markup(GTK_LABEL(p->lbl_medio), txt_med);
    gtk_label_set_markup(GTK_LABEL(p->lbl_total), txt_tot);

    g_free(txt_crit); g_free(txt_alto); g_free(txt_med); g_free(txt_tot);
}

void fyd_cve_painel_adicionar_evento(FydCVEPainel *p, const gchar *data, 
        FydCVENivel risco, const gchar *cve, const gchar *mod, const gchar *msg) {
    if (!p) return;
    (void)msg;

    switch (risco) {
        case FYD_CVE_CRITICA: p->cont_crit++; break;
        case FYD_CVE_ALTA:    p->cont_alto++; break;
        case FYD_CVE_MEDIA:   p->cont_medio++; break;
        default: break;
    }
    fyd_cve_painel_atualizar_contadores(p, p->cont_crit, p->cont_alto, p->cont_medio);

    gchar *linha = g_strdup_printf(
        "<span color='%s'>%s | <b>%s</b> | %-12s | %s</span>",
        cor_para_nivel(risco),
        data ? data : "--/--/----",
        texto_risco(risco),
        cve ? cve : "---",
        mod ? mod : "---"
    );

    GtkWidget *item = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(item), linha);
    gtk_widget_set_halign(item, GTK_ALIGN_START);
    gtk_widget_set_margin_top(item, 6);
    gtk_widget_set_margin_bottom(item, 6);
    gtk_widget_set_margin_start(item, 8);
    gtk_widget_set_margin_end(item, 8);

    gtk_box_prepend(GTK_BOX(p->container_lista), item);

    GListModel *modelo = gtk_widget_observe_children(p->container_lista);
    guint n_itens = g_list_model_get_n_items(modelo);
    for (guint i = 30; i < n_itens; i++) {
        GObject *filho = g_list_model_get_item(modelo, i);
        gtk_widget_set_visible(GTK_WIDGET(filho), FALSE);
        g_object_unref(filho);
    }
    g_object_unref(modelo);

    g_free(linha);
}

void fyd_cve_painel_definir_status(FydCVEPainel *p, const gchar *texto, const gchar *cor) {
    if (!p) return;
    gchar *marca = g_strdup_printf(
        "<span font-weight='bold' color='%s'>%s</span>", cor, texto);
    gtk_label_set_markup(GTK_LABEL(p->lbl_status), marca);
    g_free(marca);
}

void fyd_cve_painel_limpar_lista(FydCVEPainel *p) {
    if (!p) return;
    
    GListModel *modelo = gtk_widget_observe_children(p->container_lista);
    while (g_list_model_get_n_items(modelo) > 0) {
        GObject *filho = g_list_model_get_item(modelo, 0);
        gtk_widget_unparent(GTK_WIDGET(filho));
        g_object_unref(filho);
    }
    g_object_unref(modelo);

    p->cont_crit = p->cont_alto = p->cont_medio = 0;
    fyd_cve_painel_atualizar_contadores(p, 0, 0, 0);
    fyd_cve_painel_definir_status(p, "✅ SISTEMA MONITORADO — Auditoria Ativa", COR_STATUS_OK);
}

void fyd_cve_painel_verificar_arquivo(FydCVEPainel *painel, FYDCVEChecker *checker,
                                      const char *caminho, const char *linguagem) {
    if (!painel || !checker || !caminho) return;

    FILE *arq = fopen(caminho, "r");
    if (!arq) {
        fyd_cve_painel_adicionar_evento(painel, "--/--/----", FYD_CVE_ALTA,
                                        "SYS-ARQ", caminho, "Não foi possível abrir arquivo");
        return;
    }

    fseek(arq, 0, SEEK_END);
    long tam = ftell(arq);
    fseek(arq, 0, SEEK_SET);
    char *codigo = malloc(tam + 1);
    if (!codigo) { fclose(arq); return; }
    fread(codigo, 1, tam, arq);
    codigo[tam] = '\0';
    fclose(arq);

    FYDResultadoVerificacao res = fyd_cve_checker_verificar_codigo(checker, codigo, linguagem);
    time_t agora = time(NULL);
    char data[32];
    strftime(data, sizeof(data), "%d/%m/%Y %H:%M", localtime(&agora));

    if (res.encontrado) {
        FydCVENivel nivel = FYD_CVE_BAIXA;
        if (g_strstr_len(res.regra.risco, -1, "CRIT") || g_strstr_len(res.regra.risco, -1, "CRÍT"))
            nivel = FYD_CVE_CRITICA;
        else if (g_strstr_len(res.regra.risco, -1, "ALTO"))
            nivel = FYD_CVE_ALTA;
        else if (g_strstr_len(res.regra.risco, -1, "MED"))
            nivel = FYD_CVE_MEDIA;

        fyd_cve_painel_adicionar_evento(painel, data, nivel, res.regra.id, caminho, res.regra.mensagem);

        if (nivel == FYD_CVE_CRITICA)
            fyd_cve_painel_definir_status(painel, "🚨 AMEAÇA DETECTADA — Verificação de Segurança", COR_RISCO_CRIT);
        else if (nivel == FYD_CVE_ALTA)
            fyd_cve_painel_definir_status(painel, "⚠️ ALERTA — Pontos de Atenção Detectados", COR_RISCO_ALTO);
    } else {
        fyd_cve_painel_adicionar_evento(painel, data, FYD_CVE_SEGURANCA, "OK", caminho, "Sem ameaças detectadas");
    }

    free(codigo);
}
