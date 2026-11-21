/**
 * @file fyd_tema.c
 * @brief Carregamento de Tema — FydelisTech OS v3.3
 * Barra de Tarefas + Botão "F" Redondo + Painel de Sistema
 */
#include "fyd_desktop.h"

void fyd_carregar_tema(void) {
    // Valores padrão roxo neon
    fyd_cor.fundo        = g_strdup("#0f051a");
    fyd_cor.roxo_escuro  = g_strdup("#2a1045");
    fyd_cor.roxo_medio   = g_strdup("#4a1f70");
    fyd_cor.roxo_neon    = g_strdup("#9d4edd");
    fyd_cor.roxo_claro   = g_strdup("#c77dff");
    fyd_cor.texto        = g_strdup("#e0d6f0");
    fyd_cor.texto_claro  = g_strdup("#ffffff");
    fyd_cor.vidro        = g_strdup("rgba(20, 10, 40, 0.85)");
    fyd_cor.borda_neon   = g_strdup("#9d4edd");
    fyd_cor.altura_barra = 48;
    g_message("[Tema] ✅ Carregado");
}

void fyd_aplicar_estilos(void) {
    GtkCssProvider *provedor = gtk_css_provider_new();
    gchar *css = g_strdup_printf(
        "window { background: %s; color: %s; }"
        
        /* ===== FUNDO DA ÁREA DE TRABALHO ===== */
        ".fyd-fundo-area { background: linear-gradient(180deg, %s, %s); }"
        
        /* ===== BARRA SUPERIOR ===== */
        ".fyd-barra-superior { "
        "  background: linear-gradient(180deg, %s, %s); "
        "  padding: 8px 16px; "
        "  min-height: 52px; "
        "  border-bottom: 1px solid rgba(255,255,255,0.1); "
        "}"
        
        /* ===== BOTÃO INICIAR — LETRA "F" REDONDO ===== */
        ".fyd-botao-iniciar { "
        "  background: linear-gradient(135deg, %s, %s); "
        "  color: white; "
        "  font-weight: bold; "
        "  font-size: 20px; "
        "  border-radius: 50%%; "
        "  min-width: 50px; "
        "  min-height: 50px; "
        "  padding: 0; "
        "  border: none; "
        "}"
        ".fyd-botao-iniciar:hover { background: linear-gradient(135deg, %s, %s); }"
        ".fyd-botao-iniciar:checked, .fyd-botao-iniciar:active { "
        "  background: linear-gradient(135deg, %s, %s); "
        "}"
        
        /* ===== PAINEL DE SISTEMA — CPU, MEMÓRIA, REDE, RELÓGIO ===== */
        ".fyd-grupo-sistema { gap: 16px; }"
        ".fyd-indicador { color: %s; font-size: 12px; }"
        ".fyd-icone-status { font-size: 18px; }"
        ".fyd-relogio { color: %s; font-weight: 500; font-size: 14px; padding-left: 12px; }"
        
        /* ===== MENU — Tamanho automático conforme os itens ===== */
	"#fyd-menu { "
	"  background: rgba(26, 8, 56, 0.98); "
	"  border: 2px solid #9d4edd; "
	"  border-radius: 12px; "
	"  padding: 12px; "
	"  margin: 4px 8px; "
	"  min-width: 200px; "        /* Largura mínima razoável */
	"  max-width: 350px; "        /* Não fica gigante */
	"  width: 280px; "              /* Ajusta sozinho */
	"}"
	".fyd-item-menu { "
	"  background: transparent; "
	"  color: #e0d6f0; "
	"  padding: 10px 14px; "
	"  border-radius: 6px; "
	"}"
	".fyd-item-menu:hover { "
	"  background: #4a1f70; "
	"}"
	".fyd-separador { "
	"  margin: 6px 20px; "
	"  background: #9d4edd; "
	"  opacity: 0.3; "
	"}"
        
        /* ===== ÍCONES DE APLICATIVOS ===== */
        ".fyd-icone-app { "
        "  background: transparent; "
        "  color: %s; "
        "  font-size: 36px; "
        "  min-width: 90px; "
        "  min-height: 90px; "
        "  border-radius: 12px; "
        "}"
        ".fyd-icone-app:hover { background: rgba(157,78,221,0.3); }"
        
        /* ===== OUTROS ===== */
        ".fyd-splash-fundo { background: linear-gradient(135deg, %s, %s); padding: 40px; }"
        ".fyd-login-fundo { background: linear-gradient(135deg, %s, %s); }"
        ".fyd-caixa-vidro { background: %s; border-radius: 16px; border: 2px solid %s; padding: 32px; }"
        ".fyd-logo-grande { font-size: 72px; color: %s; font-weight: bold; }"
        ".fyd-logo-pequeno { font-size: 48px; color: %s; font-weight: bold; }"
        ".fyd-botao-neon { background: %s; color: white; border-radius: 8px; padding: 12px; font-weight: bold; }"
        ".fyd-botao-neon:hover { background: %s; }"
        ".fyd-entrada { background: rgba(255,255,255,0.1); border: 1px solid %s; border-radius: 8px; color: white; padding: 12px; }"
        ".fyd-barra-carregando { background: %s; border-radius: 4px; }"
        ".fyd-barra-carregando > trough { background: rgba(255,255,255,0.1); border-radius: 4px; }"
        ".fyd-barra-carregando > progress { background: %s; border-radius: 4px; }"
        ".fyd-texto-sistema { font-size: 24px; color: %s; }"
        ".fyd-texto-titulo { font-size: 20px; color: %s; }"
        ".fyd-texto-status { color: %s; }"
        ".fyd-texto-versao { font-size: 12px; color: %s; opacity: 0.7; }",
        
        fyd_cor.fundo, fyd_cor.texto,
        fyd_cor.roxo_escuro, fyd_cor.fundo,
        fyd_cor.roxo_escuro, fyd_cor.roxo_medio,
        fyd_cor.roxo_neon, fyd_cor.roxo_claro,
        fyd_cor.roxo_claro, fyd_cor.roxo_neon,
        fyd_cor.texto,
        fyd_cor.texto_claro,
        fyd_cor.vidro, fyd_cor.borda_neon,
        fyd_cor.texto, fyd_cor.roxo_medio,
        fyd_cor.texto,
        fyd_cor.roxo_escuro, fyd_cor.roxo_medio,
        fyd_cor.vidro, fyd_cor.borda_neon,
        fyd_cor.roxo_neon,
        fyd_cor.roxo_neon,
        fyd_cor.roxo_neon, fyd_cor.roxo_claro,
        fyd_cor.roxo_neon,
        fyd_cor.roxo_neon,
        fyd_cor.roxo_neon,
        fyd_cor.texto,
        fyd_cor.texto,
        fyd_cor.texto,
        fyd_cor.texto
    );

    gtk_css_provider_load_from_string(provedor, css);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provedor), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    g_object_unref(provedor);
    g_free(css);
    
    g_message("[Tema] ✅ Estilos aplicados");
}
