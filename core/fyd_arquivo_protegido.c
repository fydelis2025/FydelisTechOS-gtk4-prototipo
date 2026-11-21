/*
 * FydelisTech OS — Arquivo Protegido de Usuário v4.3
 * Hash SHA-256 + Salt + Assinatura de Integridade 🔐
 * Compilar: gcc fyd_arquivo_protegido.c -o fyd_arquivo_protegido -lcrypto -Wall -O2
 * Caminhos lidos de → /fyd/config/fyd_registro.fyd
 * Sem dependências externas — 100% FydelisTechOS
 */
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "fyd_usuarios.h"
#include "fyd_registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <openssl/sha.h>

static char fyd_pasta_pessoal[512];

/* ========== CONFIGURAÇÕES — LIDAS DO REGISTRO OFICIAL ========== */
static const char*
fyd_caminho_banco(void) {
    return fyd_registro_ler("Caminhos/contas_usuarios", "/secure/contas");
}

static const char*
fyd_caminho_perfis(void) {
    return fyd_registro_ler("Caminhos/perfis_usuarios", "/usuarios");
}

static const char*
fyd_caminho_arquivo_banco(void) {
    return fyd_registro_ler("Arquivos/banco_contas", "/secure/contas/.fyd_usuarios.dat");
}

#define FYD_TAMANHO_SALT     12
//#define FYD_TAM_HASH_HEX     (SHA256_DIGEST_LENGTH * 2)

/* ========== ESTRUTURA DO ARQUIVO .dat — ASSINADA ========== */
typedef struct {
    char     login[48];
    char     salt[FYD_TAMANHO_SALT + 1];
    char     senha_hash[FYD_TAM_HASH_HEX + 1];
    uint32_t nivel_acesso;
    uint64_t data_criacao;
    uint64_t ultimo_acesso;
    uint32_t id_usuario;
    uint8_t  reservado[28];
    char     assinatura[FYD_TAM_HASH_HEX + 1];
} FydArquivoUsuario;

/* ========== GERAÇÃO DE SEMENTE ALEATÓRIA — NATIVA ========== */
static uint32_t
fyd_semente_aleatoria(void)
{
    static uint64_t contador = 0;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    contador++;
    
    uint64_t combinado = ((uint64_t)tp.tv_sec << 32) | (uint64_t)tp.tv_nsec;
    combinado ^= contador;
    combinado ^= (uintptr_t)&combinado;
    
    return (uint32_t)(combinado ^ (combinado >> 32));
}

/* ========== GERAR SALT — SEM /dev/urandom ========== */
static void
fyd_gerar_salt(char *saida, size_t tam)
{
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
static void
fyd_sha256(const void *dados, size_t tam, char *saida_hex)
{
    unsigned char res[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)dados, tam, res);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(saida_hex + i * 2, "%02x", res[i]);
}

/* ========== GARANTIR PASTAS DO SISTEMA ========== */
static int
fyd_garantir_pastas_sistema(void)
{
    const char *pasta_contas = fyd_caminho_banco();
    struct stat st = {0};
    
    if (stat(pasta_contas, &st) == -1) {
        if (mkdir(pasta_contas, 0700) != 0) {
            perror("[Fyd] ❌ Falha ao criar /secure/contas");
            return 0;
        }
        chmod(pasta_contas, 0700);
        printf("[Fyd] ✅ Criado: %s 🔒\n", pasta_contas);
    }
    
    const char *pasta_perfis = fyd_caminho_perfis();
    if (stat(pasta_perfis, &st) == -1) {
        if (mkdir(pasta_perfis, 0755) != 0) {
            perror("[Fyd] ❌ Falha ao criar /usuarios");
            return 0;
        }
        chmod(pasta_perfis, 0755);
        printf("[Fyd] ✅ Criado: %s 🏠\n", pasta_perfis);
    }
    
    return 1;
}

/* ========== ASSINAR ESTRUTURA ========== */
static void
fyd_assinar_estrutura(FydArquivoUsuario *usr)
{
    memset(usr->assinatura, 0, sizeof(usr->assinatura));
    fyd_sha256(usr, offsetof(FydArquivoUsuario, assinatura), usr->assinatura);
}

/* ========== VALIDAR ASSINATURA ========== */
static int
fyd_validar_arquivo(const FydArquivoUsuario *usr)
{
    char hash_calculado[FYD_TAM_HASH_HEX + 1];
    FydArquivoUsuario copia = *usr;
    memset(copia.assinatura, 0, sizeof(copia.assinatura));
    fyd_sha256(&copia, offsetof(FydArquivoUsuario, assinatura), hash_calculado);
    
    if (strcmp(hash_calculado, usr->assinatura) != 0) {
        fprintf(stderr, "[Fyd] ⚠️ VIOLAÇÃO DETECTADA — Assinatura inválida!\n");
        return 0;
    }
    return 1;
}

/* ========== SALVAR BANCO (arquivo único .dat) ========== */
int
fyd_banco_salvar_lista(FydArquivoUsuario *lista, int total)
{
    if (!fyd_garantir_pastas_sistema())
        return 0;

    const char *caminho = fyd_caminho_arquivo_banco();
    FILE *arq = fopen(caminho, "wb");
    if (!arq) {
        perror("[Fyd] ❌ Não foi possível salvar banco");
        return 0;
    }

    fwrite(lista, sizeof(FydArquivoUsuario), total, arq);
    fclose(arq);
    chmod(caminho, 0600);
    
    printf("[Fyd] 🔐 Banco salvo: %s (%d usuários)\n", caminho, total);
    return 1;
}

/* ========== CARREGAR BANCO COMPLETO ========== */
int
fyd_banco_carregar_lista(FydArquivoUsuario **saida)
{
    *saida = NULL;
    const char *caminho = fyd_caminho_arquivo_banco();
    
    FILE *arq = fopen(caminho, "rb");
    if (!arq) {
        printf("[Fyd] ℹ️ Banco ainda não existe — primeiro acesso\n");
        return 0;
    }

    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    fseek(arq, 0, SEEK_SET);
    
    int total = tamanho / sizeof(FydArquivoUsuario);
    if (total <= 0) {
        fclose(arq);
        return 0;
    }

    *saida = calloc(total, sizeof(FydArquivoUsuario));
    int lidos = fread(*saida, sizeof(FydArquivoUsuario), total, arq);
    fclose(arq);
    
    if (lidos != total) {
        free(*saida);
        *saida = NULL;
        fprintf(stderr, "[Fyd] ❌ Banco corrompido\n");
        return 0;
    }

    // Validar cada entrada
    for (int i = 0; i < total; i++) {
        if (!fyd_validar_arquivo(&(*saida)[i])) {
            free(*saida);
            *saida = NULL;
            return 0;
        }
    }
    
    printf("[Fyd] ✅ Banco carregado: %d usuários\n", total);
    return total;
}

/* ========== CRIAR PASTA PESSOAL ========== */
int
fyd_criar_pasta_perfil(const char *login)
{
    char caminho[256];
    snprintf(caminho, sizeof(caminho), "%s/%s", fyd_caminho_perfis(), login);
    
    struct stat st = {0};
    if (stat(caminho, &st) == -1) {
        printf("[Fyd] 🏠 Criando perfil: %s\n", caminho);
        if (mkdir(caminho, 0700) != 0) return 0;
        chmod(caminho, 0700);
        
        char sub[256];
        snprintf(sub, sizeof(sub), "/usuarios/%s/documentos", login);
        mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "/usuarios/%s/projetos", login);
        mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "/usuarios/%s/recursos", login);
        mkdir(sub, 0755);
        snprintf(sub, sizeof(sub), "/usuarios/%s/.fydconf", login);
        mkdir(sub, 0700);
    }
    
    strncpy(fyd_pasta_pessoal, caminho, sizeof(fyd_pasta_pessoal) - 1);
    fyd_pasta_pessoal[sizeof(fyd_pasta_pessoal) - 1] = '\0';
    return 1;
}

/* ========== VERIFICAR SENHA ========== */
int
fyd_verificar_senha(const FydArquivoUsuario *usr, const char *senha_digitada)
{
    char senha_com_salt[256];
    snprintf(senha_com_salt, sizeof(senha_com_salt), "%s%s",
             senha_digitada, usr->salt);
    
    char hash_calculado[FYD_TAM_HASH_HEX + 1];
    fyd_sha256(senha_com_salt, strlen(senha_com_salt), hash_calculado);
    
    return strcmp(hash_calculado, usr->senha_hash) == 0;
}
