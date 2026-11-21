/* ========== COMPILAR COM: ==========
gcc fyd_usuario.c -o fyd_usuario -lcrypto -Wall -O2
===================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "fyd_usuarios.h"


/* ========== CONFIGURAÇÕES DE SEGURANÇA — CAMINHO NATIVO FYDELISTECH ========== */
// ✅ Atualizado para a estrutura oficial do sistema
#define FYD_DIRETORIO_BASE   "/secure/contas/"
#define FYD_PREFIXO_ARQUIVO  ".fyd_usuario_"
#define FYD_EXTENSAO         ".dat"
#define FYD_TAM_SALT         16    // Sal único por conta
#define FYD_TAM_CHAVE        32    // AES-256 = 32 bytes
#define FYD_TAM_IV           12    // GCM recomendado: 12 bytes
#define FYD_TAM_TAG_GCM      16    // Verificação de integridade/autenticidade

/* ========== ESTRUTURA DO ARQUIVO CRIPTOGRAFADO ========== */
typedef struct {
    uint8_t  salt[FYD_TAM_SALT];          // Único por conta
    uint8_t  iv[FYD_TAM_IV];              // Único por gravação
    uint8_t  tag_gcm[FYD_TAM_TAG_GCM];    // Prova de integridade
    uint32_t tamanho_dados;               // Tamanho do conteúdo
    uint8_t  dados_cripto[sizeof(FydUsuario)];  // Conteúdo cifrado
} FydArquivoCripto;


/* ========== DERIVAÇÃO DE CHAVE ========== */
static void
fyd_derivar_chave(const char *senha_maestra,
                  const uint8_t *salt,
                  uint8_t *chave_saida)
{
    PKCS5_PBKDF2_HMAC(senha_maestra,
                      strlen(senha_maestra),
                      salt,
                      FYD_TAM_SALT,
                      100000,  // 100 mil iterações → proteção contra força bruta
                      EVP_sha256(),
                      FYD_TAM_CHAVE,
                      chave_saida);
}

/* ========== GERAÇÃO DE NÚMEROS ALEATÓRIOS SEGUROS ========== */
static int
fyd_aleatorio_seguro(uint8_t *dest, size_t tam)
{
    return RAND_bytes(dest, tam) == 1;
}

/* ========== CRIPTOGRAFIA AES-256-GCM ========== */
static int
fyd_criptografar(const uint8_t *chave,
                 const uint8_t *plano,
                 size_t        tam_plano,
                 FydArquivoCripto *saida)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;

    // Gera IV único
    if (!fyd_aleatorio_seguro(saida->iv, FYD_TAM_IV)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    EVP_EncryptInit_ex(ctx, NULL, NULL, chave, saida->iv);

    int tam_escrito = 0, total = 0;
    EVP_EncryptUpdate(ctx, saida->dados_cripto, &tam_escrito, plano, tam_plano);
    total = tam_escrito;

    EVP_EncryptFinal_ex(ctx, saida->dados_cripto + tam_escrito, &tam_escrito);
    total += tam_escrito;
    saida->tamanho_dados = total;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, FYD_TAM_TAG_GCM, saida->tag_gcm);
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}

/* ========== DESCRIPTOGRAFIA AES-256-GCM ========== */
static int
fyd_descriptografar(const uint8_t *chave,
                    const FydArquivoCripto *entrada,
                    uint8_t *plano_saida,
                    size_t  *tam_saida)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    EVP_DecryptInit_ex(ctx, NULL, NULL, chave, entrada->iv);
    EVP_DecryptInit_ex(ctx, NULL, NULL, chave, entrada->iv);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, FYD_TAM_TAG_GCM, (void*)entrada->tag_gcm);

    int tam_ler = 0, ok = 0;
    ok = EVP_DecryptUpdate(ctx, plano_saida, &tam_ler,
                           entrada->dados_cripto, entrada->tamanho_dados);
    *tam_saida = tam_ler;

    int final_ok = EVP_DecryptFinal_ex(ctx, plano_saida + tam_ler, &tam_ler);
    ok = ok && final_ok;
    *tam_saida += tam_ler;

    EVP_CIPHER_CTX_free(ctx);

    if (!ok) {
        fprintf(stderr, "\n[Fydelis] ⚠️ ARQUIVO VIOLADO OU CHAVE INVÁLIDA\n");
        memset(plano_saida, 0, sizeof(FydUsuario));
        return 0;
    }
    return 1;
}

/* ========== CAMINHO DO ARQUIVO ========== */
static void
fyd_caminho_usuario(uid_t uid, char *saida, size_t tam_saida)
{
    snprintf(saida, tam_saida, "%s%s%d%s",
             FYD_DIRETORIO_BASE, FYD_PREFIXO_ARQUIVO,
             (int)uid, FYD_EXTENSAO);
}

/* ========== GARANTIR DIRETÓRIO PROTEGIDO ========== */
static int
fyd_garantir_diretorio_base(void)
{
    struct stat st = {0};
    if (stat(FYD_DIRETORIO_BASE, &st) == -1) {
        if (mkdir(FYD_DIRETORIO_BASE, 0700) != 0) {
            fprintf(stderr, "[Fydelis] ❌ Falha ao criar diretório: %s\n",
                    FYD_DIRETORIO_BASE);
            return 0;
        }
        chmod(FYD_DIRETORIO_BASE, 0700); // Apenas root/proprietário acessa
        printf("[Fydelis] ✅ Diretório protegido criado: %s\n", FYD_DIRETORIO_BASE);
    }
    return 1;
}

/* ========== SALVAR USUÁRIO → DISCO CRIPTOGRAFADO ========== */
int
fyd_salvar_usuario(uid_t               uid,
                   const FydUsuario   *usr,
                   const char         *senha_maestra)
{
    if (!fyd_garantir_diretorio_base())
        return 0;

    char caminho[512];
    fyd_caminho_usuario(uid, caminho, sizeof(caminho));

    FydArquivoCripto arq = {0};

    // Sal aleatório único
    if (!fyd_aleatorio_seguro(arq.salt, FYD_TAM_SALT))
        return 0;

    uint8_t chave[FYD_TAM_CHAVE];
    fyd_derivar_chave(senha_maestra, arq.salt, chave);

    if (!fyd_criptografar(chave, (const uint8_t*)usr, sizeof(FydUsuario), &arq)) {
        memset(chave, 0, sizeof(chave));
        return 0;
    }

    FILE *f = fopen(caminho, "wb");
    if (!f) {
        perror("[Fydelis] ❌ Falha ao gravar");
        memset(chave, 0, sizeof(chave));
        return 0;
    }

    fwrite(&arq, sizeof(arq), 1, f);
    fclose(f);

    // Bloqueia permissões: só o dono lê/escreve
    chmod(caminho, 0600);
    chown(caminho, uid, getgid());

    // Limpa chave da memória — muito importante!
    memset(chave, 0, sizeof(chave));

    printf("[Fydelis] 🔐 Usuário salvo: %s\n", caminho);
    return 1;
}

/* ========== CARREGAR USUÁRIO ← DESCRIPTOGRAFAR ========== */
int
fyd_carregar_usuario(uid_t         uid,
                     FydUsuario   *destino,
                     const char   *senha_maestra)
{
    char caminho[512];
    fyd_caminho_usuario(uid, caminho, sizeof(caminho));

    FILE *f = fopen(caminho, "rb");
    if (!f) {
        fprintf(stderr, "[Fydelis] ⚠️ Arquivo não encontrado: %s\n", caminho);
        return 0;
    }

    FydArquivoCripto arq;
    if (fread(&arq, sizeof(arq), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "[Fydelis] ❌ Arquivo corrompido\n");
        return 0;
    }
    fclose(f);

    uint8_t chave[FYD_TAM_CHAVE];
    fyd_derivar_chave(senha_maestra, arq.salt, chave);

    size_t tam_lido;
    int ok = fyd_descriptografar(chave, &arq, (uint8_t*)destino, &tam_lido);

    memset(chave, 0, sizeof(chave));
    return ok;
}
