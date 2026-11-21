#ifndef DISKTOOLS_H
#define DISKTOOLS_H

#include <stdint.h>
#include <stdbool.h>
#include "syscalls/partition_syscalls.h" // ✅ Usa as structs oficiais do kernel (DiscoInfo e ParticaoInfo)

typedef struct {
    char nomeOriginal[128];
    char caminho[256];
    uint64_t tamanho;
    uint64_t clusterInicio;
    char tipo[16];
    int probabilidade;
    bool recuperavel;
} ArquivoPerdido; // ✅ Agora ArquivoPerdido é um tipo válido em C puro!

typedef enum {
    SISTEMA_ARQUIVOS_FAT32,
    SISTEMA_ARQUIVOS_EXFAT
} SistemaArquivos;

typedef enum {
    PADRAO_LIMPEZA_UMA_PASSADA,
    PADRAO_LIMPEZA_TRES_PASSADAS,
    PADRAO_LIMPEZA_SETE_PASSADAS
} PadraoLimpeza;

#ifdef __cplusplus
extern "C" {
#endif

int disktools_listar_discos(DiscoInfo* buffer, int max_discos);
int disktools_listar_particoes(const char* disco, ParticaoInfo* buffer, int max_particoes);
bool disktools_formatar_particao(const char* caminho, SistemaArquivos tipo, const char* etiqueta, bool rapido);
bool disktools_formatar_pendrive(const char* caminho, SistemaArquivos tipo, const char* etiqueta);
bool disktools_clonar_disco(const char* origem, const char* destino, bool verificar);
bool disktools_limpar_disco(const char* disco, PadraoLimpeza padrao);

#ifdef __cplusplus
}
#endif

#endif // DISKTOOLS_H
