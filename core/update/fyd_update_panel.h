#ifndef FYD_UPDATE_PANEL_H
#define FYD_UPDATE_PANEL_H

#include <gtk/gtk.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FydUpdatePainel FydUpdatePainel;

GtkWidget* fyd_update_painel_novo(void);

// ✅ Assinatura unificada — sem parâmetro extra que não existe no .c
void fyd_update_painel_download_concluido(FydUpdatePainel *painel);

#ifdef __cplusplus
}
#endif

#endif

