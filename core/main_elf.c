// ==========================================
// FYDELISTECH OS — ENTRADA ELF NATIVO
// Sem libc · Sem GTK · Rodando direto no kernel
// ==========================================
#include "fyd_ponte_elf.h"

#define FB_ADDR  0x700100000000ULL

// Ponto de entrada — chamado pelo carregador do kernel
void _start(void) {
    // Inicializa a ponte com o kernel
    if (fyd_iniciar() < 0) {
        // Erro — trava
        for (;;) { __asm__ volatile ("hlt"); }
    }

    // ✅ Aqui vai a lógica do ambiente sem GTK
    // Por enquanto fica em espera
    for (;;) { __asm__ volatile ("hlt"); }
}

