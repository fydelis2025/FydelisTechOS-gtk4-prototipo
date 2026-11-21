#include "syscalls/disktools_syscalls.h"
#include "syscalls/partition_syscalls.h"
#include "../fyd_ponte.h"                 // Ponte de syscalls do utilizador
#include "syscalls/numeros.h"        // Cabeçalho oficial correto do kernel

// === FALLBACKS DE MACROS DE SYSCALL (caso ainda não estejam no header oficial) ===
#ifndef SYS_DISK_OBTER_TAMANHO
#define SYS_DISK_OBTER_TAMANHO 30
#endif
#ifndef SYS_DISK_CLONAR
#define SYS_DISK_CLONAR 31
#endif
#ifndef SYS_DISK_CRIAR_IMG
#define SYS_DISK_CRIAR_IMG 32
#endif
#ifndef SYS_DISK_REST_IMG
#define SYS_DISK_REST_IMG 33
#endif
#ifndef SYS_PART_FAT32
#define SYS_PART_FAT32 34
#endif
#ifndef SYS_PART_EXFAT
#define SYS_PART_EXFAT 35
#endif
#ifndef SYS_DISK_CHECK_BAD
#define SYS_DISK_CHECK_BAD 36
#endif
#ifndef SYS_DISK_INIT
#define SYS_DISK_INIT 37
#endif
#ifndef SYS_PART_LISTAR
#define SYS_PART_LISTAR 38
#endif
#ifndef SYS_PART_CRIAR
#define SYS_PART_CRIAR 39
#endif
#ifndef SYS_PART_REMOVER
#define SYS_PART_REMOVER 40
#endif
#ifndef SYS_PART_FORMATAR
#define SYS_PART_FORMATAR 41
#endif

extern long fyd_syscall(long numero, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);


// === CLONAGEM E DISCO ===
uint64_t obter_tamanho_disco(void) {
    return (uint64_t)fyd_syscall(SYS_DISK_OBTER_TAMANHO, 0, 0, 0, 0, 0, 0);
}

int disco_ler_setor(const char* disco, uint64_t lba, void* buffer) {
    return (int)fyd_syscall(SYS_DISK_LER, (long)disco, (long)lba, (long)buffer, 0, 0, 0);
}

int disco_escrever_setor(const char* disco, uint64_t lba, const void* buffer) {
    return (int)fyd_syscall(SYS_DISK_ESCREVER, (long)disco, (long)lba, (long)buffer, 0, 0, 0);
}

int clonar_disco(const char* origem, const char* destino) {
    return (int)fyd_syscall(SYS_DISK_CLONAR, (long)origem, (long)destino, 0, 0, 0, 0);
}

int criar_imagem(const char* disco, const char* arquivo_img) {
    return (int)fyd_syscall(SYS_DISK_CRIAR_IMG, (long)disco, (long)arquivo_img, 0, 0, 0, 0);
}

int restaurar_imagem(const char* arquivo_img, const char* disco) {
    return (int)fyd_syscall(SYS_DISK_REST_IMG, (long)arquivo_img, (long)disco, 0, 0, 0, 0);
}

// === FORMATAÇÃO E PARTIÇÕES ===
int formatar_fat32(const char* dispositivo, uint64_t lba, uint64_t setores) {
    return (int)fyd_syscall(SYS_PART_FAT32, (long)dispositivo, (long)lba, (long)setores, 0, 0, 0);
}

int formatar_exfat(const char* dispositivo, uint64_t lba, uint64_t setores) {
    return (int)fyd_syscall(SYS_PART_EXFAT, (long)dispositivo, (long)lba, (long)setores, 0, 0, 0);
}

int verificar_setores_defeituosos(const char* disco) {
    return (int)fyd_syscall(SYS_DISK_CHECK_BAD, (long)disco, 0, 0, 0, 0, 0);
}

int inicializar_disco(void) {
    return (int)fyd_syscall(SYS_DISK_INIT, 0, 0, 0, 0, 0, 0);
}

int listar_particoes(const char* disco, void* buffer, int max) {
    return (int)fyd_syscall(SYS_PART_LISTAR, (long)disco, (long)buffer, (long)max, 0, 0, 0);
}

int criar_particao(const char* disco, int tipo, uint64_t lba_inicio, uint64_t setores) {
    return (int)fyd_syscall(SYS_PART_CRIAR, (long)disco, (long)tipo, (long)lba_inicio, (long)setores, 0, 0);
}

int remover_particao(const char* disco, int numero) {
    return (int)fyd_syscall(SYS_PART_REMOVER, (long)disco, (long)numero, 0, 0, 0, 0);
}

int formatar_unidade(int indice_letra, const char* sistema) {
    return (int)fyd_syscall(SYS_PART_FORMATAR, (long)indice_letra, (long)sistema, 0, 0, 0, 0);
}
