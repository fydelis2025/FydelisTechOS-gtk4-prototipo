#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ==========================================
// LEITOR DE ARQUIVOS .def / .fyd
// FydelisTech OS — Sem SQLite, Sem Libc Extra
// ==========================================

#define MAX_LINHA 256
#define MAX_CHAVE 64
#define MAX_VALOR 128

// Estrutura para guardar um par chave-valor
typedef struct {
    char chave[MAX_CHAVE];
    char valor[MAX_VALOR];
} FydRegistro;

// Buffer simples para até 64 entradas
static FydRegistro registros[64];
static int total_registros = 0;

// Remove espaços no início e fim da string
static void trim(char *str) {
    char *fim;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;
    fim = str + strlen(str) - 1;
    while(fim > str && isspace((unsigned char)*fim)) fim--;
    fim[1] = '\0';
}

// Carrega o arquivo .def na memória
int fyd_carregar_def(const char *caminho) {
    FILE *arq = fopen(caminho, "r");
    if(!arq) return -1;
    
    total_registros = 0;
    char linha[MAX_LINHA];
    
    while(fgets(linha, sizeof(linha), arq)) {
        char *igual = strchr(linha, '=');
        if(!igual) continue; // Linha sem = → ignora ([seção], comentários, vazio)
        
        *igual = '\0';
        char *chave = linha;
        char *valor = igual + 1;
        
        trim(chave);
        trim(valor);
        
        if(strlen(chave) == 0) continue;
        
        strncpy(registros[total_registros].chave, chave, MAX_CHAVE - 1);
        strncpy(registros[total_registros].valor, valor, MAX_VALOR - 1);
        registros[total_registros].chave[MAX_CHAVE - 1] = '\0';
        registros[total_registros].valor[MAX_VALOR - 1] = '\0';
        total_registros++;
        
        if(total_registros >= 64) break;
    }
    
    fclose(arq);
    return total_registros;
}

// Busca valor pela chave
const char* fyd_ler_valor(const char *chave) {
    for(int i = 0; i < total_registros; i++) {
        if(strcmp(registros[i].chave, chave) == 0) {
            return registros[i].valor;
        }
    }
    return NULL;
}

// Converte para inteiro
int fyd_ler_int(const char *chave, int padrao) {
    const char *v = fyd_ler_valor(chave);
    if(!v) return padrao;
    return atoi(v);
}
