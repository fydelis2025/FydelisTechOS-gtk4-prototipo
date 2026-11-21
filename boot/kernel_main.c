#include <stdint.h>

#define VGA_BUFFER 0xB8000
#define COR_FUNDO  0x1F   // Roxo escuro
#define COR_TEXTO  0x0F   // Branco
#define COR_DESTAQ 0x0D   // Rosa neon
#define COR_SUCESSO 0x0A  // Verde

static void vga_limpar(void) {
    volatile uint16_t *vga = (volatile uint16_t*)VGA_BUFFER;
    for (int i = 0; i < 80*25; i++)
        vga[i] = (COR_FUNDO << 8) | ' ';
}

static void vga_escreve(int linha, const char *texto, uint8_t cor) {
    volatile uint16_t *vga = (volatile uint16_t*)VGA_BUFFER;
    int pos = linha * 80;
    while (*texto)
        vga[pos++] = ((uint16_t)cor << 8) | *texto++;
}

void __attribute__((naked, noreturn)) kernel_entry(void) {
    vga_limpar();
    
    vga_escreve(3,  "   ████████╗██╗   ██╗███████╗██████╗ ██╗     ██╗███████╗███████╗", COR_DESTAQ);
    vga_escreve(4,  "   ╚══██╔══╝██║   ██║██╔════╝██╔══██╗██║     ██║██╔════╝██╔════╝", COR_DESTAQ);
    vga_escreve(5,  "      ██║   ██║   ██║█████╗  ██████╔╝██║     ██║█████╗  ███████╗", COR_DESTAQ);
    vga_escreve(6,  "      ██║   ╚██╗ ██╔╝██╔══╝  ██╔══██╗██║     ██║██╔══╝  ╚════██║", COR_DESTAQ);
    vga_escreve(7,  "      ██║    ╚████╔╝ ███████╗██║  ██║███████╗██║███████╗███████║", COR_DESTAQ);
    vga_escreve(8,  "      ╚═╝     ╚═══╝  ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝╚══════╝╚══════╝", COR_DESTAQ);
    
    vga_escreve(11, "[1/6] Inicializando hardware... ✅", COR_TEXTO);
    vga_escreve(12, "[2/6] Configurando memória...   ✅", COR_TEXTO);
    vga_escreve(13, "[3/6] Carregando núcleo...       ✅", COR_TEXTO);
    vga_escreve(14, "[4/6] Iniciando serviços...      ✅", COR_TEXTO);
    vga_escreve(15, "[5/6] Preparando interface...    ✅", COR_TEXTO);
    vga_escreve(16, "[6/6] Sistema pronto!            ✅", COR_SUCESSO);
    
    vga_escreve(19, "🚀 FydelisTech OS — Ambiente Gráfico em execução!", COR_SUCESSO);
    vga_escreve(22, "📍 Salvador • Bahia 🇧🇷", COR_DESTAQ);
    
    __asm__ __volatile__ ("cli; hlt; jmp .");
}

