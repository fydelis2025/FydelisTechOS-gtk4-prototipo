#include "fyd_desktop.h"

void fyd_carregar_estilos(void) {
    GtkCssProvider *css = gtk_css_provider_new();
    
    // Converte altura da barra para string
    gchar altura_barra_str[16];
    g_snprintf(altura_barra_str, sizeof altura_barra_str, "%d", 
               fyd_cor.altura_barra > 0 ? fyd_cor.altura_barra : 56);
    
    // Cor vidro_claro derivada
    gchar vidro_claro[128];
    if (fyd_cor.vidro && g_str_has_prefix(fyd_cor.vidro, "rgba")) {
        g_snprintf(vidro_claro, sizeof vidro_claro, "rgba(42, 16, 80, 0.45)");
    } else if (fyd_cor.vidro) {
        g_snprintf(vidro_claro, sizeof vidro_claro, "%s", fyd_cor.vidro);
    } else {
        g_snprintf(vidro_claro, sizeof vidro_claro, "rgba(42, 16, 80, 0.45)");
    }

    // ✅ Valores PADRÃO do design — garantido se fyd_cor não carregar
    const gchar *fundo        = fyd_cor.fundo        ? fyd_cor.fundo        : "#0F051F";
    const gchar *roxo_escuro   = fyd_cor.roxo_escuro   ? fyd_cor.roxo_escuro   : "#2A1050";
    const gchar *roxo_medio    = fyd_cor.roxo_medio    ? fyd_cor.roxo_medio    : "#5E35B1";
    const gchar *roxo_neon     = fyd_cor.roxo_neon     ? fyd_cor.roxo_neon     : "#9D4EDD";
    const gchar *roxo_claro    = fyd_cor.roxo_claro    ? fyd_cor.roxo_claro    : "#C77DFF";
    const gchar *texto         = fyd_cor.texto         ? fyd_cor.texto         : "#E0D7FF";
    const gchar *texto_claro   = fyd_cor.texto_claro   ? fyd_cor.texto_claro   : "#FFFFFF";
    const gchar *vidro          = fyd_cor.vidro          ? fyd_cor.vidro          : "rgba(42, 16, 80, 0.75)";
    const gchar *borda_neon    = fyd_cor.borda_neon    ? fyd_cor.borda_neon    : "rgba(157, 78, 221, 0.8)";

    // ✅ CSS completo — idêntico ao seu design
    gchar *css_completo = g_strdup_printf(
        "window { background: linear-gradient(135deg, %s, %s); color: %s; }"
        ".fyd-splash-fundo { background: linear-gradient(180deg, %s, %s); }"
        ".fyd-logo-grande { font-size: 96px; font-weight: 900; color: %s; text-shadow: 0 0 30px %s, 0 0 60px %s; }"
        ".fyd-texto-sistema { font-size: 28px; font-weight: bold; color: %s; letter-spacing: 4px; }"
        ".fyd-texto-versao { font-size: 13px; color: %s; opacity: 0.8; }"
        ".fyd-barra-carregando > trough { background: %s; border-radius: 4px; }"
        ".fyd-barra-carregando > progress { background: linear-gradient(90deg, %s, %s); border-radius: 4px; box-shadow: 0 0 10px %s; min-height: 8px; }"
        ".fyd-texto-status { font-size: 14px; color: %s; margin-top: 8px; }"
        ".fyd-login-fundo { background: linear-gradient(135deg, %s, %s); }"
        ".fyd-caixa-vidro { background: %s; border: 1px solid %s; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.5); padding: 40px; }"
        ".fyd-logo-pequeno { font-size: 64px; font-weight: 900; color: %s; text-shadow: 0 0 20px %s; }"
        ".fyd-texto-titulo { font-size: 22px; color: %s; margin-bottom: 20px; }"
        ".fyd-entrada { background: rgba(15,5,31,0.6); border: 1px solid %s; border-radius: 10px; color: white; padding: 12px 16px; font-size: 16px; }"
        ".fyd-entrada:focus { border-color: %s; box-shadow: 0 0 12px %s; }"
        ".fyd-botao-neon { background: linear-gradient(90deg, %s, %s); border: none; border-radius: 10px; color: white; font-weight: bold; font-size: 16px; box-shadow: 0 0 15px %s; }"
        ".fyd-botao-neon:hover { box-shadow: 0 0 25px %s; }"
        "#fyd-barra { background: %s; border-top: 1px solid %s; min-height: %spx; padding: 6px 12px; spacing: 10px; }"
        "#fyd-btn-iniciar { background: linear-gradient(135deg, %s, %s); border: none; border-radius: 10px; min-width: 64px; min-height: 44px; font-size: 24px; font-weight: bold; color: white; box-shadow: 0 0 12px %s; }"
        "#fyd-btn-iniciar:hover { box-shadow: 0 0 20px %s; }"
        "#fyd-btn-iniciar:checked { background: linear-gradient(135deg, %s, %s); }"
        ".fyd-icone-app { border-radius: 8px; min-width: 44px; min-height: 44px; background: rgba(94,53,177,0.5); border: 1px solid rgba(157,78,221,0.3); }"
        ".fyd-icone-app:hover { background: %s; box-shadow: 0 0 10px %s; }"
        "#fyd-menu { background: %s; border: 1px solid %s; border-radius: 12px; padding: 12px; min-width: 380px; }"
        ".fyd-item-menu { padding: 12px 16px; border-radius: 6px; color: %s; font-size: 15px; }"
        ".fyd-item-menu:hover { background: rgba(94,53,177,0.5); border-left: 3px solid %s; }"
        ".fyd-separador { background: rgba(157,78,221,0.4); min-height: 1px; margin: 6px 12px; }"
        ".fyd-icone-area { background: transparent; border: none; border-radius: 8px; padding: 12px 8px; }"
        ".fyd-icone-area:hover { background: rgba(94,53,177,0.4); box-shadow: 0 0 12px rgba(157,78,221,0.4); }"
        ".fyd-texto-icone { font-size: 12px; color: #FFFFFF; text-shadow: 0 1px 3px rgba(0,0,0,0.8); }"
        "#fyd-menu .fyd-entrada { background: rgba(15,5,31,0.6); border: 1px solid rgba(157,78,221,0.4); border-radius: 8px; color: white; padding: 8px 12px; font-size: 14px; }"
        "#fyd-menu .fyd-entrada:focus { border-color: %s; box-shadow: 0 0 10px rgba(157,78,221,0.5); }"
        ".fyd-indicador { font-size: 12px; color: %s; }"
        ".fyd-relogio { font-size: 14px; font-weight: 600; color: %s; margin-left: 12px; }"
        ".fyd-icone-status { font-size: 16px; }",

        /*  1 */ fundo, roxo_escuro, texto,
        /*  2 */ fundo, roxo_escuro,
        /*  3 */ roxo_neon, roxo_neon, roxo_claro,
        /*  4 */ texto_claro,
        /*  5 */ roxo_claro,
        /*  6 */ roxo_escuro,
        /*  7 */ roxo_medio, roxo_neon, roxo_neon,
        /*  8 */ roxo_claro,
        /*  9 */ fundo, roxo_escuro,
        /* 10 */ vidro, borda_neon,
        /* 11 */ roxo_neon, roxo_neon,
        /* 12 */ texto_claro,
        /* 13 */ borda_neon,
        /* 14 */ roxo_neon, roxo_neon,
        /* 15 */ roxo_medio, roxo_neon, roxo_neon,
        /* 16 */ roxo_neon,
        /* 17 */ vidro, borda_neon, altura_barra_str,
        /* 18 */ roxo_medio, roxo_neon, roxo_neon,
        /* 19 */ roxo_neon,
        /* 20 */ roxo_escuro, roxo_medio,
        /* 21 */ roxo_medio, roxo_neon,
        /* 22 */ vidro, borda_neon,
        /* 23 */ texto,
        /* 24 */ roxo_neon,
        /* 25 */ roxo_neon,
        /* 26 */ roxo_claro,
        /* 27 */ texto_claro
    );

    gtk_css_provider_load_from_string(css, css_completo);
    
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(css);
    g_free(css_completo);
}
