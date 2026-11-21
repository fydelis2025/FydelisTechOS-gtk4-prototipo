#ifndef MOUNTMANAGER_H
#define MOUNTMANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char letra[8];          // "C:", "D:"
    char caminhoFisico[128]; // "\\.\PhysicalDrive0#1"
    char sistemaArquivos[16];// "FAT32", "exFAT"
    uint64_t tamanhoTotal;
    uint64_t espacoLivre;
    bool eRemovivel;
    bool eSistema;
} UnidadeMontada;

void mountmanager_inicializar_sistema(void);
bool mountmanager_montar_unidade(const char* disco, int particao, const char* sistemaArq, uint64_t tamanho, char* letra_out, int max_len);
bool mountmanager_desmontar_unidade(const char* letra);
int mountmanager_listar_unidades(UnidadeMontada* buffer, int max_unidades);

#ifdef __cplusplus
}
#endif

#endif // MOUNTMANAGER_H
