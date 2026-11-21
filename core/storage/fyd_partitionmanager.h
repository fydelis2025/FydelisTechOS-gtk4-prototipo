#ifndef PARTITIONMANAGER_H
#define PARTITIONMANAGER_H

#include "fyd_disktools.h"

#ifdef __cplusplus
extern "C" {
#endif

int partitionmanager_listar_discos(DiscoInfo* buffer, int max_discos);
bool partitionmanager_criar_particao(const char* disco, uint64_t tamanhoBytes, SistemaArquivos sistemaArq);
bool partitionmanager_excluir_particao(const char* caminhoFisico);
bool partitionmanager_formatar_particao(const char* caminhoFisico, SistemaArquivos sistemaArq, const char* etiqueta);

#ifdef __cplusplus
}
#endif

#endif // PARTITIONMANAGER_H
