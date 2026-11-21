/**
 * @file fyd_icone.c
 * @brief Sistema de Ícones — FydelisTech OS v3.2
 */
#include "fyd_desktop.h"
#include <string.h>

typedef struct {
    gchar *nome;
    gchar *cor_principal;
    gchar *cor_clara;
    gchar *cor_escura;
    gint tamanho_padrao;
} FydIcone;

//static GHashTable *cache_icones = NULL;

static void desenhar_forma(const gchar *tipo, cairo_t *cr,
                           double largura, double altura,
                           guint r, guint g, guint b) {
    double vr = r/255.0, vg = g/255.0, vb = b/255.0;
    cairo_set_source_rgb(cr, vr, vg, vb);

    if (g_str_has_suffix(tipo, "/pasta")) {
        cairo_rectangle(cr, 4, 10, largura-8, altura-12);
        cairo_fill(cr);
        cairo_rectangle(cr, 4, 6, largura/2.0, 8);
        cairo_fill(cr);
    } else if (g_str_has_suffix(tipo, "/lixeira")) {
        cairo_rectangle(cr, 6, 8, largura-12, altura-12);
        cairo_fill(cr);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, 4, 8);
        cairo_line_to(cr, largura-4, 8);
        cairo_stroke(cr);
    } else {
        cairo_rectangle(cr, 6, 4, largura-12, altura-8);
        cairo_fill(cr);
        cairo_move_to(cr, largura/2.0, 4);
        cairo_line_to(cr, largura-6, 4);
        cairo_line_to(cr, largura-6, altura/2.0);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
}

static void desenhar_icone(GtkDrawingArea *area, cairo_t *cr,
                           int largura, int altura, gpointer dados) {
    (void)area;
    FydDesenhoDados *d = dados;
    if (!d || !d->cor_principal || !d->tipo) return;

    guint r, g, b;
    if (sscanf(d->cor_principal+1, "%02x%02x%02x", &r, &g, &b) != 3) return;
    desenhar_forma(d->tipo, cr, largura, altura, r, g, b);
}

static void liberar_dados_icone(gpointer dados) {
    FydDesenhoDados *d = dados;
    if (d) { g_free(d->cor_principal); g_free(d->tipo); g_free(d); }
}

GtkWidget* fyd_icone_carregar(const gchar *tipo, gint tamanho) {
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_set_size_request(area, tamanho, tamanho);

    FydDesenhoDados *d = g_new0(FydDesenhoDados, 1);
    d->cor_principal = fyd_cor.roxo_neon ? g_strdup(fyd_cor.roxo_neon) : g_strdup("#9D4EDD");
    d->tipo = g_strdup(tipo);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),
        desenhar_icone, d, liberar_dados_icone);
    return area;
}

GtkWidget* fyd_icone_por_arquivo(const gchar *caminho, gint tamanho) {
    if (g_file_test(caminho, G_FILE_TEST_IS_DIR))
        return fyd_icone_carregar("sistema/pasta", tamanho);

    gchar *ext = strrchr(caminho, '.');
    if (!ext) return fyd_icone_carregar("sistema/arquivo", tamanho);

    if (g_strcmp0(ext, ".fydwd") == 0)
        return fyd_icone_carregar("ext/fydwd", tamanho);
    if (g_strcmp0(ext, ".fydcalc") == 0)
        return fyd_icone_carregar("ext/fydcalc", tamanho);
    if (g_strcmp0(ext, ".fydslide") == 0)
        return fyd_icone_carregar("ext/fydslide", tamanho);

    return fyd_icone_carregar("sistema/arquivo", tamanho);
}
