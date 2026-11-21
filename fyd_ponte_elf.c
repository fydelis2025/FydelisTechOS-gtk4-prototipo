// ==========================================
// PONTE ELF — Implementação sem libc
// ==========================================
#include "fyd_ponte_elf.h"

// Chamada direta ao kernel
static long fyd_syscall(long num, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "a"(num), "D"(a1), "S"(a2), "d"(a3)
        : "rcx", "r11", "memory"
    );
    return ret;
}

int fyd_iniciar(void) {
    // TODO: inicialização mínima
    return 0; // Sucesso
}

void fyd_terminar(void) {
    fyd_syscall(FYD_SYS_EXIT, 0, 0, 0);
    __builtin_unreachable();
}

