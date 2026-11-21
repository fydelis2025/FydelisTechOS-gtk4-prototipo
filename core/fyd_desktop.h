#ifndef FYDELIS_DESKTOP_H
#define FYDELIS_DESKTOP_H

#include <gtk/gtk.h>
#include <glib.h>
#include "fyd_usuarios.h"
#include <stdbool.h> 

// Cores e Tema
typedef struct {
    gchar *fundo;
    gchar *roxo_escuro;
    gchar *roxo_medio;
    gchar *roxo_neon;
    gchar *roxo_claro;
    gchar *texto;
    gchar *texto_claro;
    gchar *vidro;
    gchar *borda_neon;
    gint altura_barra;
} FydCores;

// Telas do Sistema
typedef enum {
    FYD_TELA_SPLASH,
    FYD_TELA_LOGIN,
    FYD_TELA_DESKTOP
} FydTela;

// Janela Principal
typedef struct {
    GtkApplication *app;
    GtkWidget *janela;
    GtkWidget *stack;
    GtkWidget *btn_iniciar;
    GtkWidget *barra_superior;
    GtkWidget *menu;
} FydDesktop;

// Barra de Status
typedef struct {
    GtkWidget *lbl_cpu;
    GtkWidget *lbl_mem;
    GtkWidget *lbl_disco;
    GtkWidget *lbl_relogio;
    GtkWidget *lbl_rede;
    GtkWidget *lbl_bateria;
    guint temporizador;
} FydPainelSistema;

// Dados para Desenho de Ícones
typedef struct {
    gchar *cor_principal;
    gchar *tipo;
} FydDesenhoDados;

// ========== VARIÁVEIS GLOBAIS ==========
extern FydCores             fyd_cor;
extern FydDesktop          *fyd_area;
extern FydPainelSistema    *painel;

// ========== FUNÇÕES DE TELA ==========
GtkWidget* fyd_criar_tela_inicial(void);
GtkWidget* fyd_criar_tela_login(void);
GtkWidget* fyd_criar_area_trabalho(void);
GtkWidget* fyd_criar_menu(void);
GtkWidget* fyd_criar_barra_sistema(void);
GtkWidget* fyd_criar_painel_sistema(void);

// ========== FUNÇÕES DE AÇÃO ==========
void fyd_tela_mudar(FydTela nova_tela);
void fyd_menu_alternar(GtkWidget *botao, gpointer dados);
void fyd_explorador_abrir(GtkWidget *item, gpointer dados);
void fyd_painel_controle_abrir(GtkWidget *item, gpointer dados);

void fyd_abrir_escritor(GtkWidget *btn, gpointer dados);
void fyd_abrir_calc(GtkWidget *btn, gpointer dados);
void fyd_abrir_apresentacao(GtkWidget *btn, gpointer dados);

// ========== SISTEMA ==========
void fyd_carregar_tema(void);
void fyd_aplicar_estilos(void);
bool fyd_usuario_inicializar(void);

int fyd_iniciar(int argc, char **argv);

// ========== ÍCONES ==========
GtkWidget* fyd_icone_carregar(const gchar *tipo, gint tamanho);
GtkWidget* fyd_icone_por_arquivo(const gchar *caminho, gint tamanho);

// ==========================================
// 📁 ASSOCIAÇÃO DE EXTENSÕES — v4.0
// Arquivo: /fyd/associacoes/fyd_extensoes.fydassoc
// ==========================================
void        fyd_associar_inicializar(void);
gboolean    fyd_associar_abrir(const gchar *caminho_arquivo);
const gchar*fyd_associar_icone(const gchar *caminho);
const gchar*fyd_associar_nome(const gchar *caminho);
GList*      fyd_associar_listar(void);

#endif
