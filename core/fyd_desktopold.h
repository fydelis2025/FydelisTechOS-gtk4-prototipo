#ifndef FYDELIS_DESKTOP_H
#define FYDELIS_DESKTOP_H

#include <gtk/gtk.h>
#include <glib.h>
#include "../usuarios/fyd_usuarios.h"

// ==========================================
// 🎨 CORES — Carregadas de .fydcores
// ==========================================
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

extern FydCores fyd_cor;

// ==========================================
// 📱 TELAS DO SISTEMA
// ==========================================
typedef enum {
    FYD_TELA_SPLASH,
    FYD_TELA_LOGIN,
    FYD_TELA_DESKTOP
} FydTela;

// ==========================================
// 🖥️ ÁREA DE TRABALHO
// ==========================================
typedef struct {
    GtkApplication *app;
    GtkWidget *janela;
    GtkWidget *stack;          // Container de troca de telas
    GtkWidget *btn_iniciar;    // Botão "F"
    GtkWidget *menu;           // Menu popover
} FydDesktop;

// ==========================================
// 📊 PAINEL DO SISTEMA
// ==========================================
typedef struct {
    GtkWidget *lbl_cpu;
    GtkWidget *lbl_mem;
    GtkWidget *lbl_disco;
    GtkWidget *lbl_relogio;
    GtkWidget *lbl_rede;
    GtkWidget *lbl_bateria;
    guint temporizador;        // ID do timer de atualização
} FydPainelSistema;

// ==========================================
// 🌐 VARIÁVEIS GLOBAIS
// ==========================================
extern FydDesktop        *fyd_area;
extern FydPainelSistema  *painel;

// ==========================================
// 🔧 FUNÇÕES GERAIS
// ==========================================
void fyd_carregar_cores(void);
void fyd_restaurar_tema_padrao(void);
void fyd_iniciar(int argc, char **argv);
void fyd_tela_mudar(FydTela tela);
void fyd_carregar_estilos(void);

// ==========================================
// 🧱 COMPONENTES DA INTERFACE
// ==========================================
GtkWidget* fyd_criar_area_trabalho(void);
GtkWidget* fyd_criar_barra_sistema(void);
GtkWidget* fyd_criar_menu(void);
GtkWidget* fyd_criar_splash(void);
GtkWidget* fyd_criar_login(void);

// ==========================================
// 🔘 CALLBACKS — Botões e Ações
// ==========================================
void fyd_menu_alternar(GtkWidget *botao, gpointer dados);
void fyd_abrir_writer(GtkWidget *btn, gpointer dados);
void fyd_abrir_calc(GtkWidget *btn, gpointer dados);
void fyd_abrir_slide(GtkWidget *btn, gpointer dados);
void fyd_explorador_abrir(GtkWidget *item, gpointer dados);
void fyd_painel_controle_abrir(GtkWidget *item, gpointer dados);

// ==========================================
// 📂 ASSOCIAÇÕES — .fydassoc
// ==========================================
void      fyd_associar_inicializar(void);
gboolean  fyd_associar_abrir(const gchar *caminho);
const gchar* fyd_associar_icone(const gchar *caminho);
const gchar* fyd_associar_nome(const gchar *caminho);

// ==========================================
// ⚙️ REGISTRO — .fydreg
// ==========================================
void      fyd_registro_carregar(void);
const gchar* fyd_registro_ler(const gchar *chave, const gchar *padrao);
gint      fyd_registro_ler_int(const gchar *chave, gint padrao);
void      fyd_registro_salvar(const gchar *chave, const gchar *valor);

// ==========================================
// 🖼️ ÍCONES
// ==========================================
GtkWidget* fyd_icone_carregar(const gchar *tipo, gint tamanho);
GtkWidget* fyd_icone_por_arquivo(const gchar *caminho, gint tamanho);

#endif // !FYDELIS_DESKTOP_H
