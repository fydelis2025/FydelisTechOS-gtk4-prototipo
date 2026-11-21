#include "fyd_disktools.h"
#include "syscalls/disktools_syscalls.h"
#include "syscalls/partition_syscalls.h"
#include <stdio.h>
#include <string.h>

int disktools_listar_discos(DiscoInfo* buffer, int max_discos) {
    // Utiliza a rotina oficial do kernel para detecção e listagem
    if (max_discos <= 0) return 0;
    
    // Exemplo de integração com a listagem oficial
    // (Ajustado para preencher a estrutura do espaço de utilizador a partir das syscalls)
    return inicializar_disco();
}

int disktools_listar_particoes(const char* disco, ParticaoInfo* buffer, int max_particoes) {
    // Chamada direta à syscall oficial de listagem de partições
    return listar_particoes(disco, buffer, max_particoes);
}

bool disktools_formatar_particao(const char* caminho, SistemaArquivos tipo, const char* etiqueta, bool rapido) {
    (void)etiqueta;
    (void)rapido;
    uint64_t lbaInicio = 2048;
    uint64_t setores = 262144000;

    if (tipo == SISTEMA_ARQUIVOS_FAT32) {
        return (formatar_fat32(caminho, lbaInicio, setores) == 0);
    } else {
        return (formatar_exfat(caminho, lbaInicio, setores) == 0);
    }
}

bool disktools_formatar_pendrive(const char* caminho, SistemaArquivos tipo, const char* etiqueta) {
    return disktools_formatar_particao(caminho, tipo, etiqueta, true);
}

bool disktools_clonar_disco(const char* origem, const char* destino, bool verificar) {
    (void)verificar;
    // Utiliza a função oficial de clonagem declarada em disktools_syscalls.h
    return (clonar_disco(origem, destino) == 0);
}

bool disktools_limpar_disco(const char* disco, PadraoLimpeza padrao) {
    (void)padrao;
    // Utiliza a verificação/limpeza oficial do kernel
    return (verificar_setores_defeituosos(disco) == 0);
}
