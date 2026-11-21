#ifndef FYD_DRIVER_INSTALADOR_H
#define FYD_DRIVER_INSTALADOR_H

#include <glib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FydDriverInstalador FydDriverInstalador;

typedef void (*FydProgressoCb)(int porcentagem, gpointer usuario);
typedef void (*FydConcluidoCb)(bool sucesso, const gchar *mensagem, gpointer usuario);

FydDriverInstalador* fyd_driver_instalador_obter(void);

void fyd_driver_instalador_set_callbacks(
    FydDriverInstalador *inst,
    FydProgressoCb cb_progresso,
    FydConcluidoCb cb_concluido,
    gpointer dados_usuario
);

void fyd_driver_instalador_baixar(
    FydDriverInstalador *inst,
    const gchar *url,
    const gchar *id_dispositivo
);
void fyd_driver_instalador_instalar(
    FydDriverInstalador *inst,
    const gchar *caminho_arquivo,
    const gchar *id_dispositivo
);

bool fyd_driver_instalador_precisa_reiniciar(FydDriverInstalador *inst);

void fyd_driver_instalador_limpar(FydDriverInstalador *inst);

#ifdef __cplusplus
}
#endif

#endif

