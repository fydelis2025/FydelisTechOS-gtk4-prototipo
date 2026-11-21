#include <stdint.h>

// Declaração da função de lógica
extern void kernel_corpo(void);

// Ponto de entrada — o bootloader salta para cá
void __attribute__((naked, noreturn)) kernel_entry(void) {
    volatile uint16_t *vga = (volatile uint16_t*)0xB8000;
    
    // Limpa a tela
    for (int i = 0; i < 80*25; i++) {
        vga[i] = 0x0020;
    }
    
    // Mensagem de confirmação
    const char *msg1 = "✅ Kernel FydelisTech — Carregado!";
    for (int i = 0; msg1[i]; i++) {
        vga[i] = (0x0F << 8) | msg1[i];
    }
    
    // Chama a lógica principal
    kernel_corpo();
    
    // Trava se voltar
    const char *erro = "⚠️ Retorno inesperado!";
    for (int i = 0; erro[i]; i++) {
        vga[80 + i] = (0x0C << 8) | erro[i];
    }
    __asm__ __volatile__ ("cli; hlt; jmp .");
}

