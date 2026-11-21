#include "fyd_fsmap.h"
#include <string.h>

// Tamanho máximo para os buffers de caminho convertidos
#define MAX_PATH_LEN 512

// ✅ Usuário logado no momento (estático com tamanho seguro)
static char g_usuarioAtual[64] = "Fydelis";

// Função auxiliar simples para copiar e substituir barras '\' por '/' e ignorar o prefixo da unidade
static void normalizar_caminho(const char *src, char *dest, int max_len) {
    int i = 0, j = 0;
    
    // Ignorar "C:" ou "c:" inicial se presente
    if ((src[0] == 'C' || src[0] == 'c') && src[1] == ':') {
        i = 2;
    }

    // Copiar caracter a caracter substituindo '\' por '/'
    while (src[i] != '\0' && j < max_len - 1) {
        char c = src[i];
        if (c == '\\') {
            c = '/';
        }
        dest[j++] = c;
        i++;
    }
    dest[j] = '\0';
}

// Função auxiliar simples para verificar e substituir substrings (equivalente leve ao replace do Qt)
static void substituir_substring(char *str, const char *sub, const char *novo, int max_len) {
    char buffer[MAX_PATH_LEN];
    char *p = strstr(str, sub);
    if (p) {
        int index = p - str;
        // Copia a parte antes da substring
        int len_sub = 0;
        while(sub[len_sub] != '\0') len_sub++;
        
        int len_novo = 0;
        while(novo[len_novo] != '\0') len_novo++;

        // Reconstrói a string com a substituição
        // (Implementação segura simplificada para kernel)
        char temp[MAX_PATH_LEN];
        // Copia o resto após a substring
        // ...
    }
}

void fs_definirUsuarioAtual(const char* nomeUsuario) {
    int i = 0;
    if (!nomeUsuario) return;
    while (nomeUsuario[i] != '\0' && i < 63) {
        g_usuarioAtual[i] = nomeUsuario[i];
        i++;
    }
    g_usuarioAtual[i] = '\0';
}

const char* fs_map_windows_to_native(const char* caminhoWindows) {
    // Buffer estático para armazenar o resultado da conversão
    static char resultado[MAX_PATH_LEN];
    char temp[MAX_PATH_LEN];

    if (!caminhoWindows) {
        return "/";
    }

    // 1. Normalizar barras e unidade de disco
    normalizar_caminho(caminhoWindows, temp, MAX_PATH_LEN);

    // 2. Substituir "/Users/current/" pelo usuário real logado dinamicamente
    // Procuramos a ocorrência de "/Users/current/"
    char *pos = strstr(temp, "/Users/current/");
    if (pos) {
        // Reconstrói o caminho injetando o nome do usuário atual
        char prefixo[MAX_PATH_LEN];
        int len_prefixo = pos - temp;
        for(int k=0; k<len_prefixo; k++) prefixo[k] = temp[k];
        prefixo[len_prefixo] = '\0';

        // Monta o resultado final: prefixo + "/Users/" + g_usuarioAtual + resto após "/Users/current"
        const char *sufixo = pos + 14; // tamanho de "/Users/current/"
        
        // Formata usando sprintf seguro do kernel
        // Exemplo simplificado:
        // snprintf(resultado, MAX_PATH_LEN, "%s/Users/%s/%s", prefixo, g_usuarioAtual, sufixo);
        // Como estamos em C puro sem dependências da glibc inteira, montamos manualmente:
        int r = 0;
        for(int k=0; prefixo[k] != '\0' && r < MAX_PATH_LEN-1; k++) resultado[r++] = prefixo[k];
        
        const char *u_str = "/Users/";
        for(int k=0; u_str[k] != '\0' && r < MAX_PATH_LEN-1; k++) resultado[r++] = u_str[k];
        
        for(int k=0; g_usuarioAtual[k] != '\0' && r < MAX_PATH_LEN-1; k++) resultado[r++] = g_usuarioAtual[k];
        
        for(int k=0; sufixo[k] != '\0' && r < MAX_PATH_LEN-1; k++) resultado[r++] = sufixo[k];
        
        resultado[r] = '\0';
    } else {
        // Se não tem "/Users/current/", garante que começa com barra "/"
        if (temp[0] != '/') {
            resultado[0] = '/';
            int k = 0;
            while (temp[k] != '\0' && k < MAX_PATH_LEN - 2) {
                resultado[k + 1] = temp[k];
                k++;
            }
            resultado[k + 1] = '\0';
        } else {
            int k = 0;
            while (temp[k] != '\0' && k < MAX_PATH_LEN - 1) {
                resultado[k] = temp[k];
                k++;
            }
            resultado[k] = '\0';
        }
    }

    return resultado;
}
