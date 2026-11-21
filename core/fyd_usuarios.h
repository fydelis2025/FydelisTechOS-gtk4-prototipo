#ifndef FYD_USUARIOS_H
#define FYD_USUARIOS_H

#include <stdint.h>
#include <stdbool.h>
#include "fyd_registro.h"

#define FYD_TAMANHO_LOGIN    64
#define FYD_TAMANHO_SENHA    64
#define FYD_TAM_HASH_HEX     64

#define FYD_NIVEL_CONVIDADO  0
#define FYD_NIVEL_USUARIO    1
#define FYD_NIVEL_ADMIN      2

typedef struct {
    char     login[FYD_TAMANHO_LOGIN];
    char     senha_hash[FYD_TAMANHO_SENHA];
    uint32_t nivel_acesso;  // ✅ NOME OFICIAL
    bool      logado;
    uint32_t id;
    int      ativo;
    char     assinatura[FYD_TAM_HASH_HEX + 1];
} FydUsuario;

extern FydUsuario fyd_usuario_dados;

// ✅ TODAS ASSINATURAS OFICIAIS
bool        fyd_usuario_inicializar(void);
bool        fyd_usuario_criar(const char *login, const char *senha, uint32_t nivel);
bool        fyd_usuario_autenticar(const char *login, const char *senha);
bool        fyd_usuario_alterar_senha(const char *login, const char *antiga, const char *nova);
bool        fyd_usuario_excluir(const char *login);
int         fyd_usuario_listar(FydUsuario **lista);
void        fyd_usuario_liberar_lista(FydUsuario *lista, int total);

int         fyd_usuario_apagar(const char *login);
int         fyd_usuario_login(const char *login, const char *senha);
int         fyd_usuario_logout(void);
const char* fyd_usuario_atual(void);
int         fyd_usuario_nivel(void);
int         fyd_permissao_verificar(int nivel_requerido);

//int         fyd_registro_salvar(const char* secao, const char* chave, const char* valor);

#endif
