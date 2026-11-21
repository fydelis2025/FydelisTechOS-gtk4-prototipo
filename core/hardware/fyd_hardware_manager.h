#ifndef FYD_HARDWARE_MANAGER_H
#define FYD_HARDWARE_MANAGER_H

#include <glib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    gchar *nome;
    gchar *fabricante;
    gchar *id;
    gchar *tipo;
    gchar *status;
    bool   tem_driver;
    gint   classe;
} FydDispositivo;

typedef struct FydHardwareManager FydHardwareManager;

typedef void (*FydListaAtualizadaCb)(gpointer usuario);

FydHardwareManager* fyd_hardware_manager_obter(void);

bool fyd_hardware_manager_inicializar(FydHardwareManager *hm);

void fyd_hardware_manager_escanear(FydHardwareManager *hm);

const GList* fyd_hardware_manager_lista(FydHardwareManager *hm);

void fyd_hardware_manager_iniciar_monitoramento(FydHardwareManager *hm, int intervalo_ms);
void fyd_hardware_manager_parar_monitoramento(FydHardwareManager *hm);

void fyd_hardware_manager_set_callback(FydHardwareManager *hm,
    FydListaAtualizadaCb cb, gpointer dados_usuario);

void fyd_hardware_manager_limpar(FydHardwareManager *hm);

#ifdef __cplusplus
}
#endif

#endif

