/**
 * @file fyd_painel_sistema.c
 * @brief Painel de Status — FydelisTech OS v4.1
 * ✅ Caminhos do registro /fyd/registro/sistema.fydreg
 * ✅ Fallback para Linux quando em ambiente de desenvolvimento
 * Salvador • Bahia 🇧🇷
 */

#include "fyd_desktop.h"
#include "fyd_registro.h"
#include <glib.h>
#include <math.h>
#include <string.h>

/* ==========================================
   📂 CAMINHOS — DO REGISTRO OU PADRÃO NATIVO
   ========================================== */
static const gchar* caminho_raiz_sistema(void) {
    return fyd_registro_ler("Caminhos/raiz_sistema", "/fyd");
}

static const gchar* caminho_dados_protegidos(void) {
    return fyd_registro_ler("Caminhos/dados_protegidos", "/secure");
}

/* ==========================================
   📊 LEITURA DE CPU — Nativo ou Linux
   ========================================== */
static gdouble fyd_ler_cpu_uso(void) {
    static gulong ult_total = 0, ult_ocupado = 0;
    const gchar *caminho_cpu = "/proc/stat"; // Em sistema nativo → /fyd/stats/cpu
    
    FILE *stat = fopen(caminho_cpu, "r");
    if (!stat) return 0.0;
    
    gulong u = 0, n = 0, i = 0, w = 0, s = 0, irq = 0;
    fscanf(stat, "cpu %lu %lu %lu %lu %lu %lu", &u, &n, &i, &w, &s, &irq);
    fclose(stat);
    
    gulong total = u + n + i + w + s + irq;
    gulong ocupado = u + n;
    gdouble uso = 0.0;
    
    if (ult_total > 0 && total > ult_total)
        uso = MIN(99.0, MAX(0.0, (ocupado - ult_ocupado) * 100.0 / (total - ult_total)));
    
    ult_total = total; 
    ult_ocupado = ocupado;
    return uso;
}

/* ==========================================
   🧠 LEITURA DE MEMÓRIA
   ========================================== */
static gdouble fyd_ler_mem_uso(void) {
    const gchar *caminho_mem = "/proc/meminfo"; // Nativo → /fyd/stats/mem
    
    FILE *mem = fopen(caminho_mem, "r");
    if (!mem) return 0.0;
    
    gint total = 0, livre = 0, buf = 0, cache = 0;
    char linha[128];
    
    while (fgets(linha, sizeof linha, mem)) {
        if (sscanf(linha, "MemTotal: %d", &total)) continue;
        if (sscanf(linha, "MemFree: %d", &livre)) continue;
        if (sscanf(linha, "Buffers: %d", &buf)) continue;
        if (sscanf(linha, "Cached: %d", &cache)) continue;
    }
    fclose(mem);
    
    if (total <= 0) return 0.0;
    gdouble usado = total - livre - buf - cache;
    return MIN(99.0, MAX(0.0, usado * 100.0 / total));
}

/* ==========================================
   💿 LEITURA DE DISCO — Raiz do sistema
   ========================================== */
static gdouble fyd_ler_disco_uso(void) {
    gchar *raiz = g_strdup_printf("%s", caminho_raiz_sistema());
    
    GFile *arquivo_raiz = g_file_new_for_path(raiz);
    GFileInfo *info = g_file_query_filesystem_info(
        arquivo_raiz,
        G_FILE_ATTRIBUTE_FILESYSTEM_USED "," G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,
        NULL, NULL
    );
    
    gdouble uso = 0.0;
    if (info) {
        guint64 usado = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_USED);
        guint64 total = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
        if (total > 0)
            uso = MIN(99.0, MAX(0.0, usado * 100.0 / total));
        g_object_unref(info);
    }
    
    g_object_unref(arquivo_raiz);
    g_free(raiz);
    return uso;
}

/* ==========================================
   🕐 RELÓGIO — Fuso do registro
   ========================================== */
static void fyd_atualizar_relogio(void) {
    const gchar *formato = "%H:%M  ·  %d/%m/%Y";
    // Em sistema nativo → usar fuso_horario do registro
    
    GDateTime *agora = g_date_time_new_now_local();
    gchar *texto = g_date_time_format(agora, formato);
    
    gtk_label_set_text(GTK_LABEL(painel->lbl_relogio), texto);
    g_free(texto);
    g_date_time_unref(agora);
}

/* ==========================================
   📶 STATUS DE REDE E BATERIA
   ========================================== */
static const gchar* fyd_icone_rede(void) {
    // Em sistema nativo → ler /fyd/stats/rede
    return "📶";
}

static const gchar* fyd_icone_bateria(void) {
    // Em sistema nativo → ler /fyd/stats/bateria
    return "🔌";
}

/* ==========================================
   🔄 ATUALIZAR TODOS OS DADOS
   ========================================== */
static gboolean fyd_atualizar_painel(gpointer dados) {
    (void)dados;
    
    fyd_atualizar_relogio();
    
    gdouble cpu = fyd_ler_cpu_uso();
    gdouble mem = fyd_ler_mem_uso();
    gdouble disco = fyd_ler_disco_uso();
    
    gtk_label_set_text(GTK_LABEL(painel->lbl_cpu),  g_strdup_printf("CPU: %.0f%%", cpu));
    gtk_label_set_text(GTK_LABEL(painel->lbl_mem),  g_strdup_printf("RAM: %.0f%%", mem));
    gtk_label_set_text(GTK_LABEL(painel->lbl_disco), g_strdup_printf("Disco: %.0f%%", disco));
    gtk_label_set_text(GTK_LABEL(painel->lbl_rede),  fyd_icone_rede());
    gtk_label_set_text(GTK_LABEL(painel->lbl_bateria), fyd_icone_bateria());
    
    return TRUE;
}

/* ==========================================
   🖼️ CRIAR PAINEL
   ========================================== */
GtkWidget* fyd_criar_painel_sistema(void) {
    painel = g_new0(FydPainelSistema, 1);
    
    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_add_css_class(caixa, "fyd-grupo-sistema");

    // CPU
    GtkWidget *col_cpu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    painel->lbl_cpu = gtk_label_new("CPU: --");
    gtk_widget_add_css_class(painel->lbl_cpu, "fyd-indicador");
    gtk_box_append(GTK_BOX(col_cpu), painel->lbl_cpu);
    gtk_box_append(GTK_BOX(caixa), col_cpu);

    // Memória
    GtkWidget *col_mem = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    painel->lbl_mem = gtk_label_new("RAM: --");
    gtk_widget_add_css_class(painel->lbl_mem, "fyd-indicador");
    gtk_box_append(GTK_BOX(col_mem), painel->lbl_mem);
    gtk_box_append(GTK_BOX(caixa), col_mem);

    // Disco
    GtkWidget *col_disco = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    painel->lbl_disco = gtk_label_new("Disco: --");
    gtk_widget_add_css_class(painel->lbl_disco, "fyd-indicador");
    gtk_box_append(GTK_BOX(col_disco), painel->lbl_disco);
    gtk_box_append(GTK_BOX(caixa), col_disco);

    // Rede
    painel->lbl_rede = gtk_label_new("📶");
    gtk_widget_add_css_class(painel->lbl_rede, "fyd-icone-status");
    gtk_box_append(GTK_BOX(caixa), painel->lbl_rede);

    // Bateria
    painel->lbl_bateria = gtk_label_new("🔌");
    gtk_widget_add_css_class(painel->lbl_bateria, "fyd-icone-status");
    gtk_box_append(GTK_BOX(caixa), painel->lbl_bateria);

    // Relógio
    painel->lbl_relogio = gtk_label_new("--:--");
    gtk_widget_add_css_class(painel->lbl_relogio, "fyd-relogio");
    gtk_box_append(GTK_BOX(caixa), painel->lbl_relogio);

    fyd_atualizar_relogio();
    painel->temporizador = g_timeout_add_seconds(1, fyd_atualizar_painel, NULL);
    
    g_print("[Painel] ✅ Iniciado — Raiz: %s\n", caminho_raiz_sistema());
    return caixa;
}
