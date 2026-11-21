#ifndef FYD_CVE_CHECK_H
#define FYD_CVE_CHECK_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char id[64];
    char nome[128];
    char padraoAssembly[256];
    char padraoCpp[256];
    char padraoPerl[256];
    char risco[32];
    char acao[64];
    char mensagem[256];
} FYDCVERegra;

typedef struct {
    bool encontrado;
    FYDCVERegra regra;
    char trechoEncontrado[512];
} FYDResultadoVerificacao;

typedef struct FYDCVEChecker FYDCVEChecker;

FYDCVEChecker* fyd_cve_checker_criar(void);
void fyd_cve_checker_destruir(FYDCVEChecker* checker);

bool fyd_cve_checker_carregar_regras(FYDCVEChecker* checker, const char* caminhoJson);
int fyd_cve_checker_quantidade_regras(const FYDCVEChecker* checker);
bool fyd_cve_checker_obter_regra(const FYDCVEChecker* checker, int indice, FYDCVERegra* regraOut);

FYDResultadoVerificacao fyd_cve_checker_verificar_codigo(FYDCVEChecker* checker, const char* codigo, const char* linguagem);
FYDResultadoVerificacao fyd_cve_checker_verificar_assembly(FYDCVEChecker* checker, const char* codigo);
FYDResultadoVerificacao fyd_cve_checker_verificar_cpp(FYDCVEChecker* checker, const char* codigo);
FYDResultadoVerificacao fyd_cve_checker_verificar_perl(FYDCVEChecker* checker, const char* codigo);

#ifdef __cplusplus
}
#endif

#endif
