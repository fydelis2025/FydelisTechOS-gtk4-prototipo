#include "core/fyd_desktop.h"
#include "core/fyd_ponte.h"
#include <gtk/gtk.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("[1/6] Iniciando FydelisTech OS...\n");
    fflush(stdout);

    printf("[2/6] Inicializando GTK4...\n");
    fflush(stdout);
    gtk_init();

    printf("[3/6] Aplicando estilos do sistema...\n");
    fflush(stdout);
    fyd_aplicar_estilos();

    printf("[4/6] Conectando ao kernel...\n");
    fflush(stdout);
    
    if (fyd_iniciar(argc, argv) < 0) {
        printf("[ERRO] Falha na inicialização!\n");
        return 1;
    }

    printf("[5/6] Carregando sistema de usuários...\n");
    fflush(stdout);
    if (!fyd_usuario_inicializar()) {
        printf("[ERRO] Sistema de usuários falhou!\n");
        return 1;
    }

    printf("[6/6] Abrindo ambiente gráfico — TELA CHEIA...\n");
    fflush(stdout);

    // 🖥️ JANELA — TELA CHEIA, SEM BARRA, SEM BORDA
    GtkWidget *janela = gtk_application_window_new(NULL);
    
    gtk_window_set_decorated(GTK_WINDOW(janela), FALSE);  // Sem barra de título
    gtk_window_fullscreen(GTK_WINDOW(janela));             // Tela cheia
    gtk_window_set_resizable(GTK_WINDOW(janela), FALSE);    // Trava tamanho

    gtk_widget_set_visible(janela, TRUE);

    printf("✅ FydelisTech OS — PRONTO!\n");
    fflush(stdout);

    // ✅ GTK4 — Loop de eventos
    while (g_main_context_iteration(NULL, TRUE)) {
        // Mantém a interface rodando
    }

    return 0;
}

