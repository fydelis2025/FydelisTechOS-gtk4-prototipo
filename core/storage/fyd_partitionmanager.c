#include "fyd_partitionmanager.h"
#include "syscalls/disktools_syscalls.h"
#include "syscalls/partition_syscalls.h"
#include <string.h>

int partitionmanager_listar_discos(DiscoInfo* buffer, int max_discos) {
    return disktools_listar_discos(buffer, max_discos);
}

bool partitionmanager_criar_particao(const char* disco, uint64_t tamanhoBytes, SistemaArquivos sistemaArq) {
    int tipoSistema = (sistemaArq == SISTEMA_ARQUIVOS_FAT32) ? 1 : 0;
    uint64_t setores = tamanhoBytes / 512;
    
    // Chamada à syscall oficial de criação de partição
    int resultado = criar_particao(disco, tipoSistema, 2048, setores);
    return (resultado == 0);
}

bool partitionmanager_excluir_particao(const char* caminhoFisico) {
    if (strstr(caminhoFisico, "PhysicalDrive0#1") != NULL) {
        return false; // Proteção da partição do sistema
    }
    
    // Chamada à syscall oficial de remoção (assumindo índice numérico extraído do caminho)
    int resultado = remover_particao(caminhoFisico, 1);
    return (resultado == 0);
}

bool partitionmanager_formatar_particao(const char* caminhoFisico, SistemaArquivos sistemaArq, const char* etiqueta) {
    (void)etiqueta;
    if (strstr(caminhoFisico, "PhysicalDrive0#1") != NULL) {
        return false;
    }
    
    const char* sistemaStr = (sistemaArq == SISTEMA_ARQUIVOS_FAT32) ? "FAT32" : "exFAT";
    int resultado = formatar_unidade(0, sistemaStr);
    return (resultado == 0);
}
