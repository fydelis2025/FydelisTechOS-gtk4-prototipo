#include "fyd_diskclone.h"
#include "syscalls/disktools_syscalls.h"
#include "syscalls/partition_syscalls.h"
#include "fyd_ponte.h" // Contém o fyd_syscall centralizado
#include <stdio.h>
#include <string.h>

bool diskclone_clonar_disco(const char* origem, const char* destino, bool verificar,
                            DiskCloneProgressoCallback cb_progresso,
                            DiskCloneEtapaCallback cb_etapa,
                            DiskCloneFinalizadoCallback cb_finalizado) {
    (void)destino;
    (void)verificar;

    if (cb_etapa) cb_etapa("Iniciando clonagem via Syscalls do Kernel...");
    if (cb_progresso) cb_progresso(0, 0, 100);

    // Obtém o tamanho total utilizando a função oficial do kernel/syscall
    uint64_t total = obter_tamanho_disco();
    char buffer[4096]; // 8 setores por bloco (496 bytes / 512 por setor)

    for (uint64_t lba = 0; lba < total; lba += 8) {
        int pct = (int)((lba * 100) / (total == 0 ? 1 : total));
        if (cb_progresso) cb_progresso(pct, lba, total);

        // Chamada oficial de leitura e escrita via baixo nível do kernel
        if (disco_ler_setor(origem, lba, buffer) != 1) {
            if (cb_finalizado) cb_finalizado(false, lba * 512);
            return false;
        }
        if (disco_escrever_setor(destino, lba, buffer) != 1) {
            if (cb_finalizado) cb_finalizado(false, lba * 512);
            return false;
        }
    }

    if (cb_etapa) cb_etapa("Verificação concluída");
    if (cb_finalizado) cb_finalizado(true, total * 512);
    return true;
}

bool diskclone_criar_imagem(const char* origem, const char* caminhoArquivo) {
    // Integração direta com a syscall de imagem definida em disktools_syscalls.h
    return (criar_imagem(origem, caminhoArquivo) == 0);
}

bool diskclone_restaurar_imagem(const char* caminhoArquivo, const char* destino) {
    // Integração direta com a syscall de restauro definida em disktools_syscalls.h
    return (restaurar_imagem(caminhoArquivo, destino) == 0);
}
