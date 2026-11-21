#ifndef FYD_PANIC_EXCECAO_H
#define FYD_PANIC_EXCECAO_H

#ifdef __cplusplus
extern "C" {
#endif

// ==========================================
// ESTRUTURA — ORDEM IGUAL ao empilhamento em idt.asm!
// ==========================================
struct RegsContexto {
    // 16 registradores gerais — empilhados por isr_comum
    unsigned long long rax;
    unsigned long long rbx;
    unsigned long long rcx;
    unsigned long long rdx;
    unsigned long long rsi;
    unsigned long long rdi;
    unsigned long long rbp;
    unsigned long long rsp;
    unsigned long long r8;
    unsigned long long r9;
    unsigned long long r10;
    unsigned long long r11;
    unsigned long long r12;
    unsigned long long r13;
    unsigned long long r14;
    unsigned long long r15;
    
    // Empilhados manualmente antes da chamada
    unsigned long long rip;
    unsigned long long rflags;
    unsigned long long error_code;
    unsigned long long vetor_excecao;
};

// ✅ Nome padronizado
void fyd_panic_excecao(struct RegsContexto* registros);

#ifdef __cplusplus
}
#endif

#endif // FYD_PANIC_EXCECAO_H
