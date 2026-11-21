// ==========================================
// LAUNCHER — Conecta Kernel .asm → FydelisShell
// FydelisTech OS v2.0
// ==========================================
#include "powershell/powershell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_LINHA 1024

// ✅ Ponto de entrada chamado pelo SEU kernel
int shell_main(void) {
    fydelishell_init();
    
    char linha[TAM_LINHA];
    while (1) {
        fydelishell_mostrar_prompt();
        
        if (!fgets(linha, TAM_LINHA, stdin)) break;
        
        // Remover nova linha
        size_t len = strlen(linha);
        if (len > 0 && linha[len-1] == '\n') linha[len-1] = 0;
        if (len > 1 && linha[len-2] == '\r') linha[len-2] = 0;
        
        if (strlen(linha) == 0) continue;
        
        // Parsear e executar
        ComandoPS cmd;
        if (fydelishell_parse(linha, &cmd)) {
            fydelishell_executar(&cmd);
        }
    }
    
    return 0;
}
