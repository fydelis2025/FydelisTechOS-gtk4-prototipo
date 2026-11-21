#ifndef DISKCLONE_H
#define DISKCLONE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DiskCloneProgressoCallback)(int percentual, uint64_t setorAtual, uint64_t totalSetores);
typedef void (*DiskCloneEtapaCallback)(const char* mensagem);
typedef void (*DiskCloneFinalizadoCallback)(bool sucesso, uint64_t bytesCopiados);

bool diskclone_clonar_disco(const char* origem, const char* destino, bool verificar,
                            DiskCloneProgressoCallback cb_progresso,
                            DiskCloneEtapaCallback cb_etapa,
                            DiskCloneFinalizadoCallback cb_finalizado);

bool diskclone_criar_imagem(const char* origem, const char* caminhoArquivo);
bool diskclone_restaurar_imagem(const char* caminhoArquivo, const char* destino);

#ifdef __cplusplus
}
#endif

#endif // DISKCLONE_H
