#ifndef FYD_CVE_PANEL_H
#define FYD_CVE_PANEL_H

#include <gtk/gtk.h>
#include "fyd_cve_check.h"

#ifdef __cplusplus
extern "C" {
#endif

// ✅ Tipos declarados aqui para o .c enxergar
typedef enum {
    FYD_CVE_CRITICA,
    FYD_CVE_ALTA,
    FYD_CVE_MEDIA,
    FYD_CVE_BAIXA,
    FYD_CVE_SEGURANCA
} FydCVENivel;

typedef struct FydCVEPainel FydCVEPainel;

// Funções públicas
GtkWidget* fyd_cve_painel_novo(void);
void fyd_cve_painel_atualizar_contadores(FydCVEPainel *p, int crit, int alto, int med);
void fyd_cve_painel_adicionar_evento(FydCVEPainel *p, const gchar *data, 
        FydCVENivel risco, const gchar *cve, const gchar *mod, const gchar *msg);
void fyd_cve_painel_definir_status(FydCVEPainel *p, const gchar *texto, const gchar *cor);
void fyd_cve_painel_limpar_lista(FydCVEPainel *p);
void fyd_cve_painel_verificar_arquivo(FydCVEPainel *painel, FYDCVEChecker *checker,
                                      const char *caminho, const char *linguagem);

#ifdef __cplusplus
}
#endif

#endif

