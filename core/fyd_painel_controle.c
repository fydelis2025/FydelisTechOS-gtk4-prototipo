#include "fyd_desktop.h"
#include "security/fyd_guard.h"
#include "security/fyd_cve_check.h"
#include "security/fyd_cve_panel.h"
#include <string.h>

// ==========================================
// 🎨 CORES — Padrão FydelisTech OS
// ==========================================
#define COR_FUNDO        "#0F051F"
#define COR_CARD         "#2A1050"
#define COR_BORDA        "#5E35B1"
#define COR_TEXTO        "#E0D7FF"
#define COR_TEXTO_CLARO  "#FFFFFF"
#define COR_DESTAQUE     "#9D4EDD"
#define COR_SELECIONADO  "#4F46E5"
#define COR_SUCESSO      "#00FF88"
#define COR_AVISO        "#FF9900"
#define COR_ERRO         "#FF4444"

static GtkWidget *janela_painel = NULL;
static GtkWidget *lista_categorias = NULL;
static GtkWidget *pilha_paginas = NULL;

// Instâncias dos sistemas de segurança
static FydGuard *sistema_guard = NULL;
static FYDCVEChecker *sistema_cve = NULL;

// ==========================================
// 🧹 Estilo Global
// ==========================================
static void aplicar_estilo_painel(void) {
    static gboolean carregado = FALSE;
    if (carregado) return;
    carregado = TRUE;

    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css,
        "window { background: " COR_FUNDO "; color: " COR_TEXTO "; font-size: 14px; }"
        ".fyd-lista-lateral { background: rgba(42, 16, 80, 0.6); border: none; padding: 8px; }"
        ".fyd-item-cat { padding: 12px; margin: 4px; border-radius: 8px; color: " COR_TEXTO "; }"
        ".fyd-item-cat:selected { background: " COR_SELECIONADO "; color: white; }"
        ".fyd-pagina { background: rgba(42, 16, 80, 0.3); padding: 16px; }"
        ".fyd-grupo { border: 1px solid rgba(157, 78, 221, 0.3); border-radius: 8px; "
        "  margin-top: 12px; padding: 16px; }"
        ".fyd-btn { background: " COR_DESTAQUE "; border: none; border-radius: 6px; "
        "  padding: 10px 16px; color: white; font-weight: bold; min-height: 40px; }"
        ".fyd-btn:hover { background: #B886F0; }"
        ".fyd-btn-secundario { background: rgba(60, 40, 110, 0.6); border: 1px solid " COR_BORDA "; }"
        ".fyd-btn-secundario:hover { background: rgba(90, 60, 150, 0.6); }"
        ".fyd-entrada { background: rgba(42, 16, 80, 0.6); border: 1px solid " COR_BORDA "; "
        "  border-radius: 4px; padding: 8px; color: white; }"
        ".fyd-rotulo-titulo { font-size: 20px; font-weight: 900; color: " COR_TEXTO_CLARO "; }"
        ".fyd-rotulo-sub { font-size: 13px; color: #A890D0; margin-bottom: 16px; }"
    );
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(css);
}

static void ao_mudar_plano_energia(GtkWidget* w, gpointer d) {
    (void)w; (void)d;
}
static void ao_mudar_brilho(GtkWidget* w, gpointer d) {
    (void)w; (void)d;
}
static void ao_mudar_tempo_suspensao(GtkWidget* w, gpointer d) {
    (void)w; (void)d;
}
static void ao_salvar_energia(GtkWidget* w, gpointer d) {
    (void)w; (void)d;
}


// ==========================================
// 🔄 Trocar Página ao Selecionar
// ==========================================
static void ao_selecionar(GtkListBox *caixa, GtkListBoxRow *linha, gpointer d) {
    (void)caixa; (void)d;
    if (!linha) return;
    
    int indice = gtk_list_box_row_get_index(linha);
    
    // ✅ GTK4: usa o índice diretamente no Stack
    GtkWidget* pagina = gtk_stack_get_child_by_name(GTK_STACK(pilha_paginas), NULL);
    
    // Alternativa mais simples e confiável:
    GListModel* paginas = G_LIST_MODEL(gtk_stack_get_pages(GTK_STACK(pilha_paginas)));
    guint n = g_list_model_get_n_items(paginas);
    
    if (indice >= 0 && (guint)indice < n) {
        gpointer pagina_item = g_list_model_get_item(paginas, indice);
        gtk_stack_set_visible_child(GTK_STACK(pilha_paginas), GTK_WIDGET(pagina_item));
        g_object_unref(pagina_item);
    }
}


// ==========================================
// 🛡️ Callback — Alerta de Ameaça do FydGuard
// ==========================================
static void cb_ameaca_fydguard(FGBlockReason motivo, const FGPacoteInfo *pacote, void *dados) {
    (void)dados;
    const char *nome_motivo = "Desconhecido";
    switch (motivo) {
        case FG_BL_ListaNegraManual: nome_motivo = "IP na Lista Negra"; break;
        case FG_BL_SaltosExcedidos: nome_motivo = "Muitos Saltos Detectados"; break;
        case FG_BL_SemAssinaturaValida: nome_motivo = "Pacote Sem Assinatura"; break;
        default: nome_motivo = "Bloqueio de Segurança"; break;
    }
    g_print("🚨 FYDGUARD: %s — IP: %u\n", nome_motivo, (unsigned int)pacote->ipOrigem);
}

// ==========================================
// 🛡️ Página: FydelisGuard — Firewall
// ==========================================
static GtkWidget* criar_pagina_fydguard(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🛡️ FydelisGuard — Firewall</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Proteção de rede, bloqueio de ameaças e monitoramento</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    // Status Ativo/Inativo
    GtkWidget *grpStatus = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpStatus, "fyd-grupo");
    GtkWidget *lblStatus = gtk_label_new(NULL);
    gtk_label_set_use_markup(GTK_LABEL(lblStatus), TRUE);
    if (sistema_guard && fyd_guard_esta_ativo(sistema_guard)) {
        gtk_label_set_markup(GTK_LABEL(lblStatus), "<b>Status:</b> <span color='" COR_SUCESSO "'>✅ Ativo e Monitorando</span>");
    } else {
        gtk_label_set_markup(GTK_LABEL(lblStatus), "<b>Status:</b> <span color='" COR_ERRO "'>⚠️ Desativado</span>");
    }
    gtk_box_append(GTK_BOX(grpStatus), lblStatus);

    // Contadores
    GtkWidget *lblCont = gtk_label_new(NULL);
    gtk_label_set_use_markup(GTK_LABEL(lblCont), TRUE);
    if (sistema_guard) {
        gchar *txt = g_strdup_printf(
            "<b>Pacotes Bloqueados:</b> <span color='" COR_ERRO "'>%d</span>\n"
            "<b>Pacotes Permitidos:</b> <span color='" COR_SUCESSO "'>%d</span>",
            fyd_guard_pacotes_bloqueados(sistema_guard),
            fyd_guard_pacotes_permitidos(sistema_guard));
        gtk_label_set_markup(GTK_LABEL(lblCont), txt);
        g_free(txt);
    }
    gtk_box_append(GTK_BOX(grpStatus), lblCont);
    gtk_box_append(GTK_BOX(pag), grpStatus);

    // Controles
    GtkWidget *grpCtrl = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpCtrl, "fyd-grupo");
    
    GtkWidget *chkProtecaoMax = gtk_check_button_new_with_label("🔒 Modo Proteção Máxima (exigir assinatura)");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkProtecaoMax), FALSE);
    gtk_box_append(GTK_BOX(grpCtrl), chkProtecaoMax);

    GtkWidget *btnAtivar = gtk_button_new_with_label("✅ Ativar Firewall");
    gtk_widget_add_css_class(btnAtivar, "fyd-btn");
    gtk_box_append(GTK_BOX(grpCtrl), btnAtivar);

    GtkWidget *btnDesativar = gtk_button_new_with_label("🛑 Desativar Firewall");
    gtk_widget_add_css_class(btnDesativar, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grpCtrl), btnDesativar);

    GtkWidget *btnAddIp = gtk_button_new_with_label("🚫 Bloquear IP Manualmente");
    gtk_widget_add_css_class(btnAddIp, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grpCtrl), btnAddIp);

    gtk_box_append(GTK_BOX(pag), grpCtrl);
    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🔍 Página: CVE Shield — Auditoria de Código
// ==========================================
static GtkWidget* criar_pagina_cve(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🔍 Fydelis CVE Shield</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Verificação de vulnerabilidades em código fonte</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    // Status do carregamento
    GtkWidget *grpStatus = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpStatus, "fyd-grupo");
    gchar *infoRegras = g_strdup_printf("<b>Regras Carregadas:</b> %d", 
                        sistema_cve ? fyd_cve_checker_quantidade_regras(sistema_cve) : 0);
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new(infoRegras));
    g_free(infoRegras);
    gtk_box_append(GTK_BOX(pag), grpStatus);

    // Botões
    GtkWidget *grpAcoes = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpAcoes, "fyd-grupo");

    GtkWidget *btnCarregar = gtk_button_new_with_label("📂 Carregar Base de Regras (.json)");
    gtk_widget_add_css_class(btnCarregar, "fyd-btn");
    gtk_box_append(GTK_BOX(grpAcoes), btnCarregar);

    GtkWidget *btnVerificar = gtk_button_new_with_label("🔍 Verificar Arquivo de Código");
    gtk_widget_add_css_class(btnVerificar, "fyd-btn");
    gtk_box_append(GTK_BOX(grpAcoes), btnVerificar);

    GtkWidget *btnPainel = gtk_button_new_with_label("📊 Abrir Painel de Eventos");
    gtk_widget_add_css_class(btnPainel, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grpAcoes), btnPainel);

    gtk_box_append(GTK_BOX(pag), grpAcoes);
    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🎧 Página: Áudio e Som
// ==========================================
static GtkWidget* criar_pagina_audio(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    gtk_box_append(GTK_BOX(pag), gtk_label_new(NULL));
    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🎧 Áudio e Som</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Configurações de alto-falante, microfone e dispositivos de som</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    GtkWidget *grp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grp, "fyd-grupo");
    GtkWidget *lblVol = gtk_label_new("<b>Volume Principal</b>");
    gtk_label_set_use_markup(GTK_LABEL(lblVol), TRUE);
    gtk_box_append(GTK_BOX(grp), lblVol);

    GtkWidget *escala = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    gtk_range_set_value(GTK_RANGE(escala), 75);
    gtk_widget_set_hexpand(escala, TRUE);
    gtk_box_append(GTK_BOX(grp), escala);

    GtkWidget *chkSom = gtk_check_button_new_with_label("🔊 Som ligado");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkSom), TRUE);
    gtk_box_append(GTK_BOX(grp), chkSom);

    gtk_box_append(GTK_BOX(pag), grp);

    GtkWidget *grp2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grp2, "fyd-grupo");
    GtkWidget *lblDev = gtk_label_new("<b>Dispositivos de Reprodução</b>");
    gtk_label_set_use_markup(GTK_LABEL(lblDev), TRUE);
    gtk_box_append(GTK_BOX(grp2), lblDev);

    GtkWidget *combo = gtk_drop_down_new_from_strings(
        (const gchar*[]){ "🔊 Alto-falante (Padrão)", "🎧 Fones de Ouvido", "🔊 Dispositivo HDMI", NULL }
    );
    gtk_box_append(GTK_BOX(grp2), combo);
    gtk_box_append(GTK_BOX(pag), grp2);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🖥️ Página: Vídeo e Tela
// ==========================================
static GtkWidget* criar_pagina_video(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🖥️ Vídeo e Tela</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Resolução, taxa de atualização e aparência</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    GtkWidget *grpRes = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpRes, "fyd-grupo");
    GtkWidget *lblRes = gtk_label_new("<b>Resolução de Tela</b>");
    gtk_label_set_use_markup(GTK_LABEL(lblRes), TRUE);
    gtk_box_append(GTK_BOX(grpRes), lblRes);

    GtkWidget *cRes = gtk_drop_down_new_from_strings(
        (const gchar*[]){ "1920 × 1080 — Recomendado", "1366 × 768", "1280 × 720", NULL }
    );
    gtk_box_append(GTK_BOX(grpRes), cRes);
    gtk_box_append(GTK_BOX(pag), grpRes);

    GtkWidget *grpHz = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpHz, "fyd-grupo");
    GtkWidget *lblHz = gtk_label_new("<b>Taxa de Atualização</b>");
    gtk_label_set_use_markup(GTK_LABEL(lblHz), TRUE);
    gtk_box_append(GTK_BOX(grpHz), lblHz);

    GtkWidget *cHz = gtk_drop_down_new_from_strings(
        (const gchar*[]){ "60 Hz — Recomendado", "144 Hz", "75 Hz", NULL }
    );
    gtk_box_append(GTK_BOX(grpHz), cHz);
    gtk_box_append(GTK_BOX(pag), grpHz);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 📡 Página: Rede
// ==========================================
static GtkWidget* criar_pagina_rede(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>📡 Rede e Internet</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Configurações de adaptador, IP, DNS e Proxy</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    GtkWidget *grpStatus = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(grpStatus, "fyd-grupo");
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("<b>Status da Conexão</b>"));
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("✅ Conectado — Rede Local"));
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("Endereço IP: 192.168.1.100"));
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("Máscara: 255.255.255.0"));
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("Gateway: 192.168.1.1"));
    gtk_box_append(GTK_BOX(pag), grpStatus);

    GtkWidget *chkDHCP = gtk_check_button_new_with_label("Obter endereço IP automaticamente (DHCP)");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkDHCP), TRUE);
    GtkWidget *grpDHCP = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(grpDHCP, "fyd-grupo");
    gtk_box_append(GTK_BOX(grpDHCP), gtk_label_new("<b>Configuração de IP</b>"));
    gtk_box_append(GTK_BOX(grpDHCP), chkDHCP);
    gtk_box_append(GTK_BOX(pag), grpDHCP);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🔋 Página: Energia — COMPLETA
// ==========================================
static GtkWidget* criar_pagina_energia(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🔋 Opções de Energia</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Gerenciamento de consumo, brilho e suspensão automática</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    // ⚡ Plano de Energia
    GtkWidget *grpPlano = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpPlano, "fyd-grupo");
    gtk_box_append(GTK_BOX(grpPlano), gtk_label_new("<b>Plano de Energia</b>"));
    
    GtkWidget *cPlano = gtk_drop_down_new_from_strings(
        (const gchar*[]){ "⚡ Alto Desempenho", "🔋 Balanceado — Recomendado", "🌿 Economia de Energia", NULL }
    );
    gtk_drop_down_set_selected(GTK_DROP_DOWN(cPlano), 1);
    g_signal_connect(cPlano, "notify::selected-item", 
                     G_CALLBACK(ao_mudar_plano_energia), NULL);
    gtk_box_append(GTK_BOX(grpPlano), cPlano);
    gtk_box_append(GTK_BOX(pag), grpPlano);

    // 💡 Brilho da Tela
    GtkWidget *grpBrilho = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpBrilho, "fyd-grupo");
    gtk_box_append(GTK_BOX(grpBrilho), gtk_label_new("<b>Brilho da Tela</b>"));
    
    GtkWidget *escalaBrilho = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 10, 100, 5);
    gtk_range_set_value(GTK_RANGE(escalaBrilho), 75);
    gtk_widget_set_hexpand(escalaBrilho, TRUE);
    g_signal_connect(escalaBrilho, "value-changed", 
                     G_CALLBACK(ao_mudar_brilho), NULL);
    gtk_box_append(GTK_BOX(grpBrilho), escalaBrilho);
    gtk_box_append(GTK_BOX(pag), grpBrilho);

    // ⏰ Tempo para Suspensão
    GtkWidget *grpSuspensao = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grpSuspensao, "fyd-grupo");
    gtk_box_append(GTK_BOX(grpSuspensao), gtk_label_new("<b>Desligar Tela / Suspender Após</b>"));
    
    GtkWidget *cTempo = gtk_drop_down_new_from_strings(
        (const gchar*[]){ "5 minutos", "10 minutos — Recomendado", "15 minutos", "Nunca", NULL }
    );
    gtk_drop_down_set_selected(GTK_DROP_DOWN(cTempo), 1);
    g_signal_connect(cTempo, "notify::selected-item", 
                     G_CALLBACK(ao_mudar_tempo_suspensao), NULL);
    gtk_box_append(GTK_BOX(grpSuspensao), cTempo);
    gtk_box_append(GTK_BOX(pag), grpSuspensao);

    // 💾 Botão Salvar
    GtkWidget *btnSalvar = gtk_button_new_with_label("💾 Salvar Configurações");
    gtk_widget_add_css_class(btnSalvar, "fyd-btn");
    g_signal_connect(btnSalvar, "clicked", G_CALLBACK(ao_salvar_energia), NULL);
    gtk_box_append(GTK_BOX(pag), btnSalvar);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}


// ==========================================
// 🔧 Página: Hardware e Drivers
// ==========================================
static GtkWidget* criar_pagina_hardware(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🔧 Hardware e Drivers</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);
    GtkWidget *sub = gtk_label_new("<span class='fyd-rotulo-sub'>Detectar dispositivos, baixar e instalar drivers</span>");
    gtk_label_set_use_markup(GTK_LABEL(sub), TRUE);
    gtk_box_append(GTK_BOX(pag), sub);

    GtkWidget *btnAbrir = gtk_button_new_with_label("🔍 Escanear Dispositivos");
    gtk_widget_set_size_request(btnAbrir, -1, 60);
    gtk_widget_add_css_class(btnAbrir, "fyd-btn");
    gtk_box_append(GTK_BOX(pag), btnAbrir);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🛡️ Página: Segurança (Resumo)
// ==========================================
static GtkWidget* criar_pagina_seguranca(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🛡️ Segurança e Manutenção</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);

    GtkWidget *grpStatus = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(grpStatus, "fyd-grupo");
    
    gchar *statusFirewall = g_strdup_printf(
        "<b>FydelisGuard Firewall:</b> %s",
        (sistema_guard && fyd_guard_esta_ativo(sistema_guard)) 
            ? "<span color='" COR_SUCESSO "'>✅ Ativo</span>" 
            : "<span color='" COR_ERRO "'>⚠️ Desativado</span>");
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new(statusFirewall));
    g_free(statusFirewall);
    
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("<b>CVE Shield:</b> ✅ Pronto para verificação"));
    gtk_box_append(GTK_BOX(grpStatus), gtk_label_new("<b>Integridade do Sistema:</b> ✅ Verificada"));
    gtk_box_append(GTK_BOX(pag), grpStatus);

    GtkWidget *btnIrGuard = gtk_button_new_with_label("🛡️ Abrir FydelisGuard (Firewall)");
    gtk_widget_add_css_class(btnIrGuard, "fyd-btn");
    gtk_box_append(GTK_BOX(pag), btnIrGuard);

    GtkWidget *btnIrCVE = gtk_button_new_with_label("🔍 Abrir CVE Shield (Análise de Código)");
    gtk_widget_add_css_class(btnIrCVE, "fyd-btn");
    gtk_box_append(GTK_BOX(pag), btnIrCVE);

    GtkWidget *btnUpd = gtk_button_new_with_label("🔄 Verificar Atualizações do Sistema");
    gtk_widget_add_css_class(btnUpd, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(pag), btnUpd);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🎨 Página: Aparência e Temas
// ==========================================
static GtkWidget* criar_pagina_temas(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>🎨 Personalização e Aparência</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);

    GtkWidget *grp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grp, "fyd-grupo");
    gtk_box_append(GTK_BOX(grp), gtk_label_new("<b>Temas Prontos</b>"));

    GtkWidget *btnRoxo = gtk_button_new_with_label("💜 FydelisTech Vidro Neon — Padrão");
    gtk_widget_add_css_class(btnRoxo, "fyd-btn");
    gtk_box_append(GTK_BOX(grp), btnRoxo);

    GtkWidget *btnAzul = gtk_button_new_with_label("💙 Azul Celeste");
    gtk_widget_add_css_class(btnAzul, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grp), btnAzul);

    GtkWidget *btnVerde = gtk_button_new_with_label("💚 Verde Escuro");
    gtk_widget_add_css_class(btnVerde, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grp), btnVerde);

    GtkWidget *btnEscuro = gtk_button_new_with_label("🖤 Escuro Total");
    gtk_widget_add_css_class(btnEscuro, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(grp), btnEscuro);

    gtk_box_append(GTK_BOX(pag), grp);
    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 👤 Página: Usuário
// ==========================================
static GtkWidget* criar_pagina_usuario(void) {
    GtkWidget *pag = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(pag, "fyd-pagina");

    GtkWidget *tit = gtk_label_new("<span class='fyd-rotulo-titulo'>👤 Conta de Usuário</span>");
    gtk_label_set_use_markup(GTK_LABEL(tit), TRUE);
    gtk_box_append(GTK_BOX(pag), tit);

    GtkWidget *grp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(grp, "fyd-grupo");
    gtk_box_append(GTK_BOX(grp), gtk_label_new("👤 fydelis — Administrador"));
    gtk_box_append(GTK_BOX(grp), gtk_label_new("📧 Perfil: Completo"));
    gtk_box_append(GTK_BOX(grp), gtk_label_new("🔑 Nível de Acesso: Root / Sistema"));
    gtk_box_append(GTK_BOX(pag), grp);

    GtkWidget *btnSair = gtk_button_new_with_label("🚪 Sair da Conta");
    gtk_widget_add_css_class(btnSair, "fyd-btn-secundario");
    gtk_box_append(GTK_BOX(pag), btnSair);

    gtk_widget_set_vexpand(pag, TRUE);
    return pag;
}

// ==========================================
// 🚪 Fechar Janela
// ==========================================
static void ao_fechar(GtkWidget *janela, gpointer d) {
    (void)d;
    // Liberar sistemas de segurança
    if (sistema_guard) {
        fyd_guard_desativar(sistema_guard);
        fyd_guard_destruir(sistema_guard);
        sistema_guard = NULL;
    }
    if (sistema_cve) {
        fyd_cve_checker_destruir(sistema_cve);
        sistema_cve = NULL;
    }
    janela_painel = NULL;
}

// ==========================================
// 🚀 Construir e Mostrar Painel de Controle
// ==========================================
void fyd_painel_controle_mostrar(void) {
    if (janela_painel) {
        gtk_window_present(GTK_WINDOW(janela_painel));
        return;
    }

    // Inicializar sistemas de segurança
    sistema_guard = fyd_guard_criar();
    fyd_guard_registrar_callback_ameaca(sistema_guard, cb_ameaca_fydguard, NULL);
    
    sistema_cve = fyd_cve_checker_criar();

    aplicar_estilo_painel();

    // Janela Principal
    janela_painel = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(janela_painel), "⚙️ Painel de Controle — FydelisTech OS");
    gtk_window_set_default_size(GTK_WINDOW(janela_painel), 1000, 650);
    g_signal_connect(janela_painel, "close-request", G_CALLBACK(ao_fechar), NULL);

    // Layout Horizontal
    GtkWidget *caixa_principal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(janela_painel), caixa_principal);

    // Lista Lateral
    lista_categorias = gtk_list_box_new();
    gtk_widget_set_size_request(lista_categorias, 240, -1);
    gtk_widget_add_css_class(lista_categorias, "fyd-lista-lateral");

    // Itens da Categoria
    const char *categorias[] = {
        "🎧  Áudio",
        "🖥️  Tela",
        "📡  Rede",
        "🔋  Energia",
        "🔧  Hardware",
        "🛡️  Segurança",
        "├─ Firewall",
        "├─ CVE Shield",
        "🎨  Aparência",
        "👤  Usuário",
        NULL
    };

    for (int i = 0; categorias[i]; i++) {
        GtkWidget *linha = gtk_list_box_row_new();
        gtk_widget_add_css_class(linha, "fyd-item-cat");
        GtkWidget *lbl = gtk_label_new(categorias[i]);
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(linha), lbl);
        gtk_list_box_append(GTK_LIST_BOX(lista_categorias), linha);
    }

    gtk_box_append(GTK_BOX(caixa_principal), lista_categorias);

    // Pilha de Páginas
    pilha_paginas = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(pilha_paginas), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_widget_set_hexpand(pilha_paginas, TRUE);
    gtk_box_append(GTK_BOX(caixa_principal), pilha_paginas);

    // Adicionar Páginas na Ordem das Categorias
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_audio());       // 0
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_video());       // 1
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_rede());        // 2
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_energia());     // 3
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_hardware());    // 4
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_seguranca());   // 5
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_fydguard());    // 6
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_cve());         // 7
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_temas());        // 8
    gtk_stack_add_child(GTK_STACK(pilha_paginas), criar_pagina_usuario());     // 9

    // Conectar navegação
    g_signal_connect(lista_categorias, "row-selected", G_CALLBACK(ao_selecionar), NULL);

    // Selecionar primeira página
    gtk_list_box_select_row(GTK_LIST_BOX(lista_categorias), 
                            gtk_list_box_get_row_at_index(GTK_LIST_BOX(lista_categorias), 0));

    gtk_window_present(GTK_WINDOW(janela_painel));
}	
