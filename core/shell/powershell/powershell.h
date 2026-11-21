// ==========================================
// FYDELISHELL — PowerShell Nativo do Zero
// FydelisTech OS v2.0 · Salvador 🇧🇷
// ==========================================
#ifndef FYDELISHELL_H
#define FYDELISHELL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROMPT_NOME "FydelisPS"
#define VERSAO "2.0.0"

// 📌 Tipo de comando
typedef enum {
    CMD_GET,
    CMD_SET,
    CMD_RUN,
    CMD_PIPE,
    CMD_REDIRECT
} TipoComando;

// 📌 Comando parseado
typedef struct {
    char nome[64];
    char parametros[256][128];
    int qtdParams;
    TipoComando tipo;
    char saidaArquivo[256];
    bool temPipe;
} ComandoPS;

// ✅ Inicializar o shell
bool fydelishell_init(void);

// ✅ Mostrar prompt
void fydelishell_mostrar_prompt(void);

// ✅ Parsear linha de comando (sintaxe PowerShell)
bool fydelishell_parse(const char* linha, ComandoPS* cmd);

// ✅ Executar comando
int fydelishell_executar(ComandoPS* cmd);

// ✅ Comandos nativos — equivalentes ao PowerShell
int cmd_get_item(int argc, char* argv[]);    // Get-Item
int cmd_set_item(int argc, char* argv[]);    // Set-Item
int cmd_get_childitem(int argc, char* argv[]);// Get-ChildItem (ls/dir)
int cmd_new_item(int argc, char* argv[]);    // New-Item
int cmd_remove_item(int argc, char* argv[]); // Remove-Item
int cmd_copy_item(int argc, char* argv[]);   // Copy-Item
int cmd_move_item(int argc, char* argv[]);   // Move-Item
int cmd_clear(int argc, char* argv[]);       // Clear-Host
int cmd_exit(int argc, char* argv[]);        // Exit
int cmd_help(int argc, char* argv[]);        // Get-Help

#endif // FYDELISHELL_H