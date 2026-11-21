/**
 * @file fyd_usuarios.c
 * @brief FydelisTechOS — Sistema de Usuários v4.3
 * 🔐 Banco: /secure/contas/.fyd_usuarios.dat
 * 🏠 Perfis: /usuarios/
 * SHA-256 + Salt + Assinatura de Integridade
 */
#define _POSIX_C_SOURCE 200809L

#include "fyd_usuarios.h"
#include "fyd_registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <openssl/sha.h>

// ==========================================
// 🔒 DADOS GLOBAIS
// ==========================================
FydUsuario fyd_usuario_dados = {0};
char       fyd_pasta_pessoal[256] = "/usuarios/convidado";

#define FYD_TAM_SALT  12

/* ========== ESTRUTURA INTERNA ========== */
typedef struct {
    char     login[FYD_TAMANHO_LOGIN];
    char     salt[FYD_TAM_SALT + 1];
    char     senha_hash[FYD_TAMANHO_SENHA];
    uint32_t nivel_acesso;
    uint64_t data_criacao;
    uint64_t ultimo_acesso;
    uint32_t id_usuario;
    uint8_t  reservado[28];
    int      ativo;
    char     assinatura[FYD_TAM_HASH_HEX + 1];
} FydArquivoUsuario;

static FydArquivoUsuario *banco_dados = NULL;
static int banco_total = 0;

/* ========== CAMINHOS ========== */
static const char* fyd_caminho_banco(void) {
    return fyd_registro_ler("Arquivos/banco_contas", "/secure/contas/.fyd_usuarios.dat");
}
static const char* fyd_caminho_perfis(void) {
    return fyd_registro_ler("Caminhos/perfis_usuarios", "/usuarios");
}

/* ========== ALEATORIEDADE ========== */
static uint32_t fyd_semente_aleatoria(void) {
    static uint64_t contador = 0;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    contador++;
    uint64_t combinado = ((uint64_t)tp.tv_sec << 32) | (uint64_t)tp.tv_nsec;
    combinado ^= contador;
    combinado ^= (uintptr_t)&combinado;
    return (uint32_t)(combinado ^ (combinado >> 32));
}

/* ========== GERAR SALT ========== */
static void fyd_gerar_salt(char *saida, size_t tam) {
    static const char *caracteres =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    uint32_t semente = fyd_semente_aleatoria();
    for (size_t i = 0; i < tam - 1; i++) {
        semente = semente * 1103515245 + 12345;
        saida[i] = caracteres[semente % 62];
    }
    saida[tam - 1] = '\0';
}

/* ========== HASH SHA-256 ========== */
static void fyd_sha256(const void *dados, size_t tam, char *saida_hex) {
    unsigned char res[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)dados, tam, res);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(saida_hex + i * 2, "%02x", res[i]);
}

/* ========== ASSINATURA ========== */
static void fyd_assinar(FydArquivoUsuario *usr) {
    memset(usr->assinatura, 0, sizeof(usr->assinatura));
    fyd_sha256(usr, offsetof(FydArquivoUsuario, assinatura), usr->assinatura);
}
static int fyd_validar(const FydArquivoUsuario *usr) {
    char hash_calc[FYD_TAM_HASH_HEX + 1];
    FydArquivoUsuario copia = *usr;
    memset(copia.assinatura, 0, sizeof(copia.assinatura));
    fyd_sha256(&copia, offsetof(FydArquivoUsuario, assinatura), hash_calc);
    return strcmp(hash_calc, usr->assinatura) == 0;
}

/* ========== GARANTIR PASTAS ========== */
static int fyd_garantir_pastas(void) {
    const char *pasta_secure = "/secure";
    struct stat st = {0};
    if (stat(pasta_secure, &st) == -1) { mkdir(pasta_secure, 0700); chmod(pasta_secure, 0700); }
    const char *pasta_contas = "/secure/contas";
    if (stat(pasta_contas, &st) == -1) { mkdir(pasta_contas, 0700); chmod(pasta_contas, 0700); }
    const char *pasta_perfis = fyd_caminho_perfis();
    if (stat(pasta_perfis, &st) == -1) { mkdir(pasta_perfis, 0755); chmod(pasta_perfis, 0755); }
    return 1;
}

/* ========== SALVAR/CARREGAR ========== */
static int fyd_banco_salvar(void) {
    if (!banco_dados || banco_total <= 0) return 0;
    const char *caminho = fyd_caminho_banco();
    FILE *f = fopen(caminho, "wb");
    if (!f) { perror("[Fyd] ❌ Erro ao salvar"); return 0; }
    fwrite(banco_dados, sizeof(FydArquivoUsuario), banco_total, f);
    fclose(f); chmod(caminho, 0600);
    printf("[Fyd] 🔐 Banco salvo: %s (%d usuários)\n", caminho, banco_total);
    return 1;
}
static int fyd_banco_carregar(void) {
    if (banco_dados) { free(banco_dados); banco_dados = NULL; }
    banco_total = 0;
    const char *caminho = fyd_caminho_banco();
    FILE *f = fopen(caminho, "rb");
    if (!f) { printf("[Fyd] ℹ️ Primeiro acesso — banco vazio\n"); return 0; }
    fseek(f, 0, SEEK_END); long tam = ftell(f); fseek(f, 0, SEEK_SET);
    banco_total = tam / sizeof(FydArquivoUsuario);
    if (banco_total <= 0) { fclose(f); return 0; }
    banco_dados = calloc(banco_total, sizeof(FydArquivoUsuario));
    int lidos = fread(banco_dados, sizeof(FydArquivoUsuario), banco_total, f);
    fclose(f);
    if (lidos != banco_total) { free(banco_dados); banco_dados = NULL; banco_total = 0; return 0; }
    for (int i = 0; i < banco_total; i++) {
        if (!fyd_validar(&banco_dados[i])) {
            fprintf(stderr, "[Fyd] ⚠️ VIOLAÇÃO no usuário %d — rejeitado!\n", i+1);
            free(banco_dados); banco_dados = NULL; banco_total = 0;
            return 0;
        }
    }
    printf("[Fyd] ✅ Banco carregado: %d usuários\n", banco_total);
    return banco_total;
}

/* ========== BUSCA ========== */
static FydArquivoUsuario* fyd_buscar(const char *login) {
    for (int i = 0; i < banco_total; i++)
        if (strcmp(banco_dados[i].login, login) == 0)
            return &banco_dados[i];
    return NULL;
}
static uint32_t fyd_proximo_id(void) {
    uint32_t maior = 0;
    for (int i = 0; i < banco_total; i++)
        if (banco_dados[i].id_usuario > maior)
            maior = banco_dados[i].id_usuario;
    return maior + 1;
}

/* ========== CRIAR PASTA ========== */
static int fyd_criar_pasta(const char *login) {
    char caminho[256];
    snprintf(caminho, sizeof(caminho), "%s/%s", fyd_caminho_perfis(), login);
    struct stat st = {0};
    if (stat(caminho, &st) == -1) {
        mkdir(caminho, 0700); chmod(caminho, 0700);
        char sub[256];
        snprintf(sub, sizeof(sub), "%s/%s/documentos", fyd_caminho_perfis(), login); mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "%s/%s/projetos", fyd_caminho_perfis(), login); mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "%s/%s/recursos", fyd_caminho_perfis(), login); mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "%s/%s/.fydconf", fyd_caminho_perfis(), login); mkdir(sub, 0700);
    }
    strncpy(fyd_pasta_pessoal, caminho, sizeof(fyd_pasta_pessoal)-1);
    fyd_pasta_pessoal[sizeof(fyd_pasta_pessoal)-1] = '\0';
    return 1;
}

// ==========================================
// ✅ FUNÇÕES PÚBLICAS — Assinaturas do .h
// ==========================================

bool fyd_usuario_inicializar(void) {
    printf("[Fyd] FydelisTechOS — Sistema de Usuários v4.3\n");
    printf("[Fyd] Banco: %s\n", fyd_caminho_banco());
    fyd_garantir_pastas();
    fyd_banco_carregar();
    return true;
}

bool fyd_usuario_criar(const char *login, const char *senha, uint32_t nivel) {
    if (!login || !senha || !login[0] || !senha[0]) return false;
    if (strlen(login) >= FYD_TAMANHO_LOGIN) return false;
    if (nivel > FYD_NIVEL_ADMIN) return false;
    fyd_garantir_pastas();
    if (fyd_buscar(login)) {
        printf("[Fyd] ⚠️ Usuário '%s' já existe\n", login);
        return false;
    }
    banco_dados = realloc(banco_dados, (banco_total + 1) * sizeof(FydArquivoUsuario));
    FydArquivoUsuario *novo = &banco_dados[banco_total];
    memset(novo, 0, sizeof(*novo));
    strncpy(novo->login, login, FYD_TAMANHO_LOGIN - 1);
    novo->login[FYD_TAMANHO_LOGIN - 1] = '\0';
    novo->nivel_acesso = nivel;
    novo->id_usuario = fyd_proximo_id();
    novo->data_criacao = time(NULL);
    novo->ultimo_acesso = time(NULL);
    fyd_gerar_salt(novo->salt, sizeof(novo->salt));
    char temp[256];
    snprintf(temp, sizeof(temp), "%s%s", senha, novo->salt);
    fyd_sha256(temp, strlen(temp), novo->senha_hash);
    fyd_assinar(novo);
    banco_total++;
    if (!fyd_banco_salvar()) return false;
    fyd_criar_pasta(login);
    printf("[Fyd] ✅ Cadastrado: %s | ID: %u\n", login, novo->id_usuario);
    return true;
}

bool fyd_usuario_autenticar(const char *login, const char *senha) {
    if (!login || !senha) return false;
    FydArquivoUsuario *usr = fyd_buscar(login);
    if (!usr) { printf("[Fyd] ❌ Usuário não encontrado\n"); return false; }
    char temp[256], hash_calc[FYD_TAMANHO_SENHA];
    snprintf(temp, sizeof(temp), "%s%s", senha, usr->salt);
    fyd_sha256(temp, strlen(temp), hash_calc);
    if (strcmp(hash_calc, usr->senha_hash) != 0) {
        printf("[Fyd] ❌ Senha incorreta\n"); return false;
    }
    usr->ultimo_acesso = time(NULL);
    fyd_assinar(usr);
    fyd_banco_salvar();
    // Copiar para dados globais
    strncpy(fyd_usuario_dados.login, usr->login, FYD_TAMANHO_LOGIN - 1);
    fyd_usuario_dados.login[FYD_TAMANHO_LOGIN - 1] = '\0';
    strncpy(fyd_usuario_dados.senha_hash, usr->senha_hash, FYD_TAMANHO_SENHA - 1);
    fyd_usuario_dados.senha_hash[FYD_TAMANHO_SENHA - 1] = '\0';
    fyd_usuario_dados.nivel_acesso = usr->nivel_acesso;
    fyd_usuario_dados.id = usr->id_usuario;
    fyd_usuario_dados.ativo = 1;
    fyd_criar_pasta(login);
    fyd_registro_salvar("Usuario/ultimo_usuario", login);
    printf("[Fyd] ✅ Bem-vindo, %s!\n", login);
    return true;
}

bool fyd_usuario_alterar_senha(const char *login, const char *antiga, const char *nova) {
    if (!login || !antiga || !nova || !nova[0]) return false;
    FydArquivoUsuario *usr = fyd_buscar(login);
    if (!usr) return false;
    char temp[256], hash_verif[FYD_TAMANHO_SENHA];
    snprintf(temp, sizeof(temp), "%s%s", antiga, usr->salt);
    fyd_sha256(temp, strlen(temp), hash_verif);
    if (strcmp(hash_verif, usr->senha_hash) != 0) return false;
    fyd_gerar_salt(usr->salt, sizeof(usr->salt));
    snprintf(temp, sizeof(temp), "%s%s", nova, usr->salt);
    fyd_sha256(temp, strlen(temp), usr->senha_hash);
    fyd_assinar(usr);
    fyd_banco_salvar();
    printf("[Fyd] ✅ Senha atualizada\n");
    return true;
}

bool fyd_usuario_excluir(const char *login) {
    FydArquivoUsuario *usr = fyd_buscar(login);
    if (!usr) return false;
    usr->ativo = 0;
    fyd_assinar(usr);
    fyd_banco_salvar();
    printf("[Fyd] ✅ Usuário '%s' desativado\n", login);
    return true;
}

int fyd_usuario_listar(FydUsuario **lista) {
    if (!lista) return -1;
    *lista = calloc(banco_total, sizeof(FydUsuario));
    for (int i = 0; i < banco_total; i++) {
        strncpy((*lista)[i].login, banco_dados[i].login, FYD_TAMANHO_LOGIN - 1);
        (*lista)[i].login[FYD_TAMANHO_LOGIN - 1] = '\0';
        strncpy((*lista)[i].senha_hash, banco_dados[i].senha_hash, FYD_TAMANHO_SENHA - 1);
        (*lista)[i].senha_hash[FYD_TAMANHO_SENHA - 1] = '\0';
        (*lista)[i].nivel_acesso = banco_dados[i].nivel_acesso;
        (*lista)[i].id = banco_dados[i].id_usuario;
        (*lista)[i].ativo = 1;
    }
    return banco_total;
}

void fyd_usuario_liberar_lista(FydUsuario *lista, int total) {
    (void)total; free(lista);
}

// ✅ Compatibilidade — funções antigas que o sistema pode chamar
int fyd_usuario_apagar(const char *login) {
    return fyd_usuario_excluir(login) ? 1 : 0;
}
int fyd_usuario_login(const char *login, const char *senha) {
    return fyd_usuario_autenticar(login, senha) ? 1 : 0;
}
int fyd_usuario_logout(void) {
    memset(&fyd_usuario_dados, 0, sizeof(fyd_usuario_dados));
    return 1;
}
const char* fyd_usuario_atual(void) {
    return fyd_usuario_dados.login;
}
int fyd_usuario_nivel(void) {
    return (int)fyd_usuario_dados.nivel_acesso;
}
int fyd_permissao_verificar(int nivel_requerido) {
    return (int)fyd_usuario_dados.nivel_acesso >= nivel_requerido ? 1 : 0;
}
