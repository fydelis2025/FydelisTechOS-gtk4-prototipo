#include "fyd_panic_excessao.h"

// ==========================================
// Saída direta na VRAM — sem depender de nada
// ==========================================
static void vga_print(int linha, int coluna, const char *str, unsigned char atributo) {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    int offset = linha * 80 + coluna;
    for (int i = 0; str[i] != '\0'; i++) {
        vga[offset + i] = ((unsigned short)atributo << 8) | (unsigned short)str[i];
    }
}

// ==========================================
// Conversão para hexadecimal — sem libc
// ==========================================
static void u64_para_hex(unsigned long long val, char *buffer) {
    const char *digitos = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    for (int i = 17; i >= 2; i--) {
        buffer[i] = digitos[val & 0xF];
        val >>= 4;
    }
    buffer[18] = '\0';
}

// ==========================================
// Nomes amigáveis das exceções da x86_64
// ==========================================
static const char* nome_excecao(unsigned long long vetor) {
    switch (vetor) {
        case 0:  return "Divisao por Zero (#DE)";
        case 1:  return "Depuracao (#DB)";
        case 2:  return "NMI — Interrupcao Nao Mascaravel";
        case 3:  return "Ponto de Parada (#BP)";
        case 4:  return "Estouro de Ponto Flutuante (#OF)";
        case 5:  return "Limite Excedido (#BR)";
        case 6:  return "Instrucao Invalida (#UD)";
        case 7:  return "Coprocessador Indisponivel (#NM)";
        case 8:  return "Falha Dupla (#DF) ⚠️";
        case 10: return "Erro de Tarefa (#TS)";
        case 11: return "Segmento Ausente (#NP)";
        case 12: return "Falha de Pilha (#SS)";
        case 13: return "Violacao de Protecao Geral (#GP)";
        case 14: return "Falha de Pagina (#PF)";
        case 16: return "Erro de Ponto Flutuante (#MF)";
        case 17: return "Falha de Alinhamento (#AC)";
        case 18: return "Erro de Maquina (#MC)";
        case 19: return "Excecao SIMD/XMM (#XF)";
        default: return "Excecao Desconhecida";
    }
}

// ==========================================
// PANIC — Tela Vermelha do Kernel
// Chamado pelo Assembly quando ocorre uma excecao
// ==========================================
void fyd_panic_excecao(struct RegsContexto* regs) {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    const unsigned char FUNDO_VERMELHO = 0x4F;

    // Limpa toda a tela com fundo vermelho
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = FUNDO_VERMELHO << 8 | ' ';
    }

    // Cabeçalho
    vga_print(2, 2, "=== FYDELISTECH OS — KERNEL PANIC ===", FUNDO_VERMELHO);
    vga_print(4, 2, "Motivo: ", FUNDO_VERMELHO);
    vga_print(4, 10, nome_excecao(regs->vetor_excecao), FUNDO_VERMELHO);

    // Detalhe específico para Falha de Página
    if (regs->vetor_excecao == 14) {
        vga_print(5, 2, "Acesso a endereco invalido ou sem permissao.", FUNDO_VERMELHO);
    }

    // Dump de registradores
    char buf[32];
    vga_print(7, 2, "Registradores:", FUNDO_VERMELHO);

    u64_para_hex(regs->rip, buf);
    vga_print(9,  4, "RIP = ", FUNDO_VERMELHO); vga_print(9,  10, buf, FUNDO_VERMELHO);

    u64_para_hex(regs->rsp, buf);
    vga_print(10, 4, "RSP = ", FUNDO_VERMELHO); vga_print(10, 10, buf, FUNDO_VERMELHO);

    u64_para_hex(regs->rax, buf);
    vga_print(11, 4, "RAX = ", FUNDO_VERMELHO); vga_print(11, 10, buf, FUNDO_VERMELHO);

    u64_para_hex(regs->rbx, buf);
    vga_print(12, 4, "RBX = ", FUNDO_VERMELHO); vga_print(12, 10, buf, FUNDO_VERMELHO);

    u64_para_hex(regs->error_code, buf);
    vga_print(13, 4, "COD = ", FUNDO_VERMELHO); vga_print(13, 10, buf, FUNDO_VERMELHO);

    u64_para_hex(regs->vetor_excecao, buf);
    vga_print(14, 4, "VET = ", FUNDO_VERMELHO); vga_print(14, 10, buf, FUNDO_VERMELHO);

    vga_print(17, 2, "Sistema parado. Verifique o RIP para localizar o codigo.", FUNDO_VERMELHO);

    // Trava com interrupcoes desabilitadas
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
