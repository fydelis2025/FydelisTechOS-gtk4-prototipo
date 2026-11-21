/**
 * @file fyd_registro.c
 * @brief Editor de Registro — .fydreg
 * ⚠️ SISTEMA PRÓPRIO · NÃO É LINUX
 * Caminho oficial: /fyd/config/fyd_registro.fyd
 */
#include "fyd_desktop.h"
#include <stdio.h>
#include <string.h>

static GHashTable *registro = NULL;

// ==========================================
// 📂 CAMINHO OFICIAL — v4.2
// ==========================================
static const gchar *caminho_registro = "/fyd/config/fyd_registro.fyd";
static const gchar *caminho_dev = "./fyd/config/fyd_registro.fyd";

// ==========================================
// 🔍 Descobrir qual caminho usar
// ==========================================
static const gchar* fyd_caminho_registro(void) {
    static gboolean inicializado = FALSE;
    static gchar caminho[512] = "";
    
    if (inicializado) return caminho;
    inicializado = TRUE;
    
    FILE *teste = fopen(caminho_registro, "r");
    if (teste) {
        fclose(teste);
        g_strlcpy(caminho, caminho_registro, sizeof caminho);
        g_message("[Registro] Usando caminho do sistema: %s", caminho);
        return caminho;
    }
    
    teste = fopen(caminho_dev, "r");
    if (teste) {
        fclose(teste);
        g_strlcpy(caminho, caminho_dev, sizeof caminho);
        g_message("[Registro] Usando caminho de desenvolvimento: %s", caminho);
        return caminho;
    }
    
    g_strlcpy(caminho, caminho_dev, sizeof caminho);
    g_warning("[Registro] Não encontrado, criando em: %s", caminho);
    return caminho;
}

// ==========================================
// 📂 CARREGAR REGISTRO NA MEMÓRIA
// ==========================================
void fyd_registro_carregar(void) {
    if (registro) g_hash_table_destroy(registro);
    registro = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    
    const gchar *caminho = fyd_caminho_registro();
    FILE *arq = fopen(caminho, "r");
    if (!arq) {
        g_warning("[Registro] Arquivo não encontrado: %s\n", caminho);
        return;
    }
    
    gchar linha[512], secao[128] = "";
    while (fgets(linha, sizeof linha, arq)) {
        linha[strcspn(linha, "\r\n")] = 0;
        if (!linha[0] || linha[0] == '#') continue;
        
        if (linha[0] == '[') {
            sscanf(linha, "[%127[^]]]", secao);
            continue;
        }
        
        gchar *igual = strchr(linha, '=');
        if (!igual || !secao[0]) continue;
        
        *igual = 0;
        gchar *chave = g_strdup_printf("%s/%s", secao, g_strstrip(linha));
        gchar *valor = g_strdup(g_strstrip(igual + 1));
        
        g_hash_table_insert(registro, chave, valor);
    }
    fclose(arq);
    g_print("[Registro] ✅ Carregadas %d chaves de: %s\n", 
             g_hash_table_size(registro), caminho);
}

// ==========================================
// 🔍 fyd_reg_ler(secao, chave, padrao) — bate com o .h
// ==========================================
const char* fyd_reg_ler(const char *secao, const char *chave, const char *padrao) {
    if (!registro) fyd_registro_carregar();
    
    gchar chave_completa[256];
    snprintf(chave_completa, sizeof(chave_completa), "%s/%s", secao, chave);
    
    const gchar *v = g_hash_table_lookup(registro, chave_completa);
    return v ? v : padrao;
}

// ==========================================
// 🔍 fyd_reg_ler_int(secao, chave, padrao)
// ==========================================
int fyd_reg_ler_int(const char *secao, const char *chave, int padrao) {
    const char *v = fyd_reg_ler(secao, chave, NULL);
    return v ? atoi(v) : padrao;
}

// ==========================================
// ⚙️ fyd_reg_inicializar()
// ==========================================
int fyd_reg_inicializar(void) {
    fyd_registro_carregar();
    return registro ? 1 : 0;
}

// ==========================================
// 🔍 Leitura direta — chave composta
// ==========================================
const gchar* fyd_registro_ler_dir(const gchar *chave, const gchar *padrao) {
    if (!registro) fyd_registro_carregar();
    const gchar *v = g_hash_table_lookup(registro, chave);
    return v ? v : padrao;
}

gint fyd_registro_ler_int_dir(const gchar *chave, gint padrao) {
    if (!registro) fyd_registro_carregar();
    const gchar *v = g_hash_table_lookup(registro, chave);
    return v ? atoi(v) : padrao;
}

gboolean fyd_registro_ler_bool_dir(const gchar *chave, gboolean padrao) {
    return fyd_registro_ler_int_dir(chave, padrao ? 1 : 0) != 0;
}

// ==========================================
// ✏️ fyd_registro_salvar(chave_completa, valor)
// ==========================================
int fyd_registro_salvar(const gchar *chave, const gchar *valor) {
    if (!registro) fyd_registro_carregar();
    
    g_hash_table_insert(registro, g_strdup(chave), g_strdup(valor));
    
    const gchar *caminho = fyd_caminho_registro();
    FILE *arq = fopen(caminho, "w");
    if (!arq) {
        g_warning("[Registro] ❌ Não foi possível salvar em: %s", caminho);
        return 0;
    }
    
    fprintf(arq, "# ==========================================\n");
    fprintf(arq, "# FYDELISTECH OS — REGISTRO DO SISTEMA v4.2\n");
    fprintf(arq, "# Caminho: %s\n", caminho);
    fprintf(arq, "# ==========================================\n\n");
    
    GList *chaves = g_hash_table_get_keys(registro);
    GList *c;
    gchar secao_atual[128] = "";
    
    for (c = chaves; c; c = c->next) {
        gchar *k = (gchar*)c->data;
        gchar *v = g_hash_table_lookup(registro, k);
        
        gchar *barra = strchr(k, '/');
        if (!barra) continue;
        
        gint tam_secao = barra - k;
        gchar nova_secao[128];
        g_strlcpy(nova_secao, k, tam_secao + 1);
        gchar *nome_chave = barra + 1;
        
        if (strcmp(nova_secao, secao_atual) != 0) {
            fprintf(arq, "[%s]\n", nova_secao);
            g_strlcpy(secao_atual, nova_secao, sizeof secao_atual);
        }
        fprintf(arq, "%s = %s\n", nome_chave, v);
    }
    
    g_list_free(chaves);
    fclose(arq);
    g_print("[Registro] 💾 Salvo: %s = %s\n", chave, valor);
    return 1;
}
