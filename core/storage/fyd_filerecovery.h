#ifndef FILERECOVERY_H
#define FILERECOVERY_H

#include <stdint.h>
#include <stdbool.h>
#include "fyd_disktools.h"

#ifdef __cplusplus
extern "C" {
#endif

int filerecovery_escanear_unidade(const char* letraUnidade, ArquivoPerdido* buffer, int max_arquivos);
bool filerecovery_recuperar_arquivo(const ArquivoPerdido* arquivo, const char* destino);

#ifdef __cplusplus
}
#endif

#endif // FILERECOVERY_H
