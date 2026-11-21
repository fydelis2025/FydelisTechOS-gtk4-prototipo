/**
 * @file fyd_iniciar.c
 * @brief Ponto de Entrada — FydelisTech OS v4.2
 * ✅ Integração com fyd_ponte.h
 * ✅ Fallback automático para desenvolvimento
 * Salvador • Bahia 🇧🇷
 */

#include "fyd_desktop.h"
#include "fyd_ponte.h"

// ========== ÚNICA DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
FydCores            fyd_cor  = {0};
FydDesktop         *fyd_area = NULL;
FydPainelSistema   *painel   = NULL;

// ========== AÇÕES DE ABERTURA ==========
void fyd_abrir_escritor(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    g_print("[Fyd] 📝 Abrindo Escritor...\n");
}

void fyd_abrir_calc(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    g_print("[Fyd] 📊 Abrindo Planilha...\n");
}

void fyd_abrir_apresentacao(GtkWidget *btn, gpointer dados) {
    (void)btn; (void)dados;
    g_print("[Fyd] 📽️ Abrindo Apresentação...\n");
}

// ========== TROCA DE TELA ==========
void fyd_tela_mudar(FydTela nova_tela) {
    if (!fyd_area || !fyd_area->stack) return;
    
    const gchar *alvo = NULL;
    switch (nova_tela) {
        case FYD_TELA_SPLASH:  alvo = "splash";  break;
        case FYD_TELA_LOGIN:   alvo = "login";   break;
        case FYD_TELA_DESKTOP: alvo = "desktop"; break;
    }
    
    if (alvo) gtk_stack_set_visible_child_name(GTK_STACK(fyd_area->stack), alvo);
}

// ========== INICIALIZAÇÃO DO SISTEMA / PONTE ==========
static void fyd_inicializar_sistema(void) {
#if defined(__FYD_OS__)
    // 🔌 Modo Nativo — inicializa ponte de syscalls
    int ponte_status = fyd_ponte_init();
    if (ponte_status == 0) {
        fyd_log(FYD_LOG_INFO, "Ponte de syscalls ativa ✅");
        g_print("[Sistema] ✅ FydelisTech OS — Modo Nativo\n");
    } else {
        fyd_log(FYD_LOG_ERRO, "Falha na ponte de syscalls ❌");
        g_print("[Sistema] ⚠️ Ponte não respondeu\n");
    }
#else
    // ⚙️ Modo Desenvolvimento — Linux
    g_print("[Sistema] ⚙️ Modo Desenvolvimento (Linux fallback)\n");
    g_print("[Sistema] ⚙️ Syscalls serão usadas quando compilado com -D__FYD_OS__\n");
#endif
}

// ========== INICIALIZAÇÃO DA JANELA ==========
static void fyd_ao_iniciar(GtkApplication *app, gpointer dados) {
    (void)dados;
    
    // 🔌 Primeiro: inicializa sistema/ponte
    fyd_inicializar_sistema();
    
    fyd_area = g_new0(FydDesktop, 1);
    fyd_area->app = app;
    
    fyd_carregar_tema();
    fyd_aplicar_estilos();
    fyd_usuario_inicializar();

    GtkWidget *janela = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(janela), "FydelisTech OS");
    gtk_window_set_decorated(GTK_WINDOW(janela), FALSE);
    gtk_window_fullscreen(GTK_WINDOW(janela));
    fyd_area->janela = janela;

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 600);
    fyd_area->stack = stack;

    // ✅ Telas na ordem correta
    gtk_stack_add_named(GTK_STACK(stack), fyd_criar_tela_inicial(), "splash");
    gtk_stack_add_named(GTK_STACK(stack), fyd_criar_tela_login(),  "login");
    gtk_stack_add_named(GTK_STACK(stack), fyd_criar_area_trabalho(), "desktop");

    gtk_window_set_child(GTK_WINDOW(janela), stack);
    gtk_window_present(GTK_WINDOW(janela));
}

// ========== MAIN ==========
int fyd_iniciar(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.fydelis.desktop", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(fyd_ao_iniciar), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
