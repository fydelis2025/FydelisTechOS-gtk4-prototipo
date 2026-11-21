#include "fyd_cve_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <glib.h>  // ✅ Adicionado para g_strcasecmp

#define MAX_REGRAS_INICIAL 64
#define TAM_LINHA 4096

struct FYDCVEChecker {
    FYDCVERegra* regras;
    int capacidade;
    int quantidade;
};

static const char* extrair_valor_json(const char* linha, const char* chave, char* destino, size_t tamDest) {
    destino[0] = '\0';
    const char* ptr = strstr(linha, chave);
    if (!ptr) return NULL;
    
    ptr += strlen(chave);
    while (*ptr && *ptr != '"') ptr++;
    if (*ptr == '"') ptr++;
    
    const char* fim = strchr(ptr, '"');
    if (!fim) return NULL;
    
    size_t len = fim - ptr;
    if (len >= tamDest) len = tamDest - 1;
    memcpy(destino, ptr, len);
    destino[len] = '\0';
    return destino;
}

FYDCVEChecker* fyd_cve_checker_criar(void) {
    FYDCVEChecker* checker = (FYDCVEChecker*)malloc(sizeof(FYDCVEChecker));
    if (!checker) return NULL;

    checker->capacidade = MAX_REGRAS_INICIAL;
    checker->quantidade = 0;
    checker->regras = (FYDCVERegra*)calloc(checker->capacidade, sizeof(FYDCVERegra));
    
    if (!checker->regras) {
        free(checker);
        return NULL;
    }
    return checker;
}

void fyd_cve_checker_destruir(FYDCVEChecker* checker) {
    if (!checker) return;
    free(checker->regras);
    free(checker);
}

bool fyd_cve_checker_carregar_regras(FYDCVEChecker* checker, const char* caminhoJson) {
    if (!checker || !caminhoJson) return false;

    FILE* arq = fopen(caminhoJson, "r");
    if (!arq) {
        fprintf(stderr, "⚠️ [FYD_CVE] Não foi possível abrir: %s\n", caminhoJson);
        return false;
    }

    checker->quantidade = 0;
    char linha[TAM_LINHA];
    FYDCVERegra regraTemp = {{0}};
    bool dentroRegra = false;

    while (fgets(linha, sizeof(linha), arq)) {
        if (strstr(linha, "{")) {
            memset(&regraTemp, 0, sizeof(regraTemp));
            dentroRegra = true;
        }

        if (dentroRegra) {
            extrair_valor_json(linha, "\"id\"", regraTemp.id, sizeof(regraTemp.id));
            extrair_valor_json(linha, "\"nome\"", regraTemp.nome, sizeof(regraTemp.nome));
            extrair_valor_json(linha, "\"padrao_assembly\"", regraTemp.padraoAssembly, sizeof(regraTemp.padraoAssembly));
            extrair_valor_json(linha, "\"padrao_cpp\"", regraTemp.padraoCpp, sizeof(regraTemp.padraoCpp));
            extrair_valor_json(linha, "\"padrao_perl\"", regraTemp.padraoPerl, sizeof(regraTemp.padraoPerl));
            extrair_valor_json(linha, "\"padrao\"", regraTemp.padraoCpp, sizeof(regraTemp.padraoCpp));
            extrair_valor_json(linha, "\"risco\"", regraTemp.risco, sizeof(regraTemp.risco));
            extrair_valor_json(linha, "\"acao\"", regraTemp.acao, sizeof(regraTemp.acao));
            extrair_valor_json(linha, "\"mensagem\"", regraTemp.mensagem, sizeof(regraTemp.mensagem));
        }

        if (strstr(linha, "}") && dentroRegra) {
            dentroRegra = false;
            if (regraTemp.id[0] && checker->quantidade < checker->capacidade) {
                checker->regras[checker->quantidade++] = regraTemp;
            }
        }
    }

    fclose(arq);
    printf("✅ [FYD_CVE] Carregadas %d regras de %s\n", checker->quantidade, caminhoJson);
    return true;
}

int fyd_cve_checker_quantidade_regras(const FYDCVEChecker* checker) {
    return checker ? checker->quantidade : 0;
}

bool fyd_cve_checker_obter_regra(const FYDCVEChecker* checker, int indice, FYDCVERegra* regraOut) {
    if (!checker || !regraOut || indice < 0 || indice >= checker->quantidade)
        return false;
    *regraOut = checker->regras[indice];
    return true;
}

static bool executar_regex(const char* padrao, const char* codigo, 
                           char* trechoOut, size_t tamMax) {
    trechoOut[0] = '\0';
    if (!padrao || !*padrao || !codigo) return false;

    regex_t regex;
    if (regcomp(&regex, padrao, REG_EXTENDED | REG_ICASE) != 0)
        return false;

    regmatch_t match;
    bool achou = (regexec(&regex, codigo, 1, &match, 0) == 0);
    
    if (achou && trechoOut && tamMax > 0) {
        int len = match.rm_eo - match.rm_so;
        if (len >= (int)tamMax) len = tamMax - 1;
        memcpy(trechoOut, codigo + match.rm_so, len);
        trechoOut[len] = '\0';
    }
    
    regfree(&regex);
    return achou;
}

static FYDResultadoVerificacao verificar(FYDCVEChecker* checker, 
        const char* codigo, const char* campoPadrao) {
    FYDResultadoVerificacao res = {false, {{0}}, {0}}; // ✅ Chaves aninhadas
    if (!checker || !codigo || !campoPadrao) return res;

    for (int i = 0; i < checker->quantidade; i++) {
        const char* padrao = NULL;
        if (strcmp(campoPadrao, "assembly") == 0) padrao = checker->regras[i].padraoAssembly;
        else if (strcmp(campoPadrao, "cpp") == 0) padrao = checker->regras[i].padraoCpp;
        else if (strcmp(campoPadrao, "perl") == 0) padrao = checker->regras[i].padraoPerl;
        
        if (!padrao || !*padrao) continue;
        
        if (executar_regex(padrao, codigo, res.trechoEncontrado, sizeof(res.trechoEncontrado))) {
            res.encontrado = true;
            res.regra = checker->regras[i];
            return res;
        }
    }
    return res;
}

FYDResultadoVerificacao fyd_cve_checker_verificar_assembly(FYDCVEChecker* c, const char* cod) {
    return verificar(c, cod, "assembly");
}

FYDResultadoVerificacao fyd_cve_checker_verificar_cpp(FYDCVEChecker* c, const char* cod) {
    return verificar(c, cod, "cpp");
}

FYDResultadoVerificacao fyd_cve_checker_verificar_perl(FYDCVEChecker* c, const char* cod) {
    return verificar(c, cod, "perl");
}

FYDResultadoVerificacao fyd_cve_checker_verificar_codigo(FYDCVEChecker* c, const char* cod, const char* lang) {
    if (!lang) {
        FYDResultadoVerificacao res = {false, {{0}}, {0}};
        return res;
    }
    
    // ✅ Usando g_ascii_strcasecmp (não depreciado)
    if (g_ascii_strcasecmp(lang, "assembly") == 0 || g_ascii_strcasecmp(lang, "asm") == 0)
        return verificar(c, cod, "assembly");
    if (g_ascii_strcasecmp(lang, "cpp") == 0 || g_ascii_strcasecmp(lang, "c++") == 0 || g_ascii_strcasecmp(lang, "c") == 0)
        return verificar(c, cod, "cpp");
    if (g_ascii_strcasecmp(lang, "perl") == 0)
        return verificar(c, cod, "perl");
    
    FYDResultadoVerificacao res = {false, {{0}}, {0}};
    return res;
}

