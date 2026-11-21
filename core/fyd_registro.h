#ifndef FYD_REGISTRO_H
#define FYD_REGISTRO_H

#include <stddef.h>
#include <glib.h>

// Caminhos principais
#define FYD_RAIZ          "/fyd"
#define FYD_BINARIOS      "/fyd/bin"
#define FYD_LIB           "/fyd/lib"
#define FYD_APPS          "/fyd/apps"
#define FYD_CONFIG        "/fyd/config"
#define FYD_MENU          "/fyd/menu"
#define FYD_REGISTRO      "/fyd/registro"
#define FYD_ASSOC         "/fyd/associacoes"
#define FYD_TEMAS         "/fyd/temas"
#define FYD_DRIVERS       "/fyd/drivers"
#define FYD_TERMINAL      "/fyd/terminal"
#define FYD_LIXEIRA       "/fyd/lixeira"
#define FYD_SECURE        "/secure"
#define FYD_USUARIOS      "/usuarios"

// Versão
#define FYD_VERSAO        "4.2"
#define FYD_CODIGO        "FydelisTechOS-2026-11-03"

// ==========================================
// Funções — base com seção separada
// ==========================================
const char* fyd_reg_ler(const char* secao, const char* chave, const char* padrao);
int         fyd_reg_ler_int(const char* secao, const char* chave, int padrao);
int         fyd_reg_inicializar(void);

// ==========================================
// Funções — chave composta "secao/chave"
// ==========================================
const gchar* fyd_registro_ler_dir(const gchar *chave_completa, const gchar *padrao);
gint         fyd_registro_ler_int_dir(const gchar *chave_completa, gint padrao);
gboolean     fyd_registro_ler_bool_dir(const gchar *chave_completa, gboolean padrao);
int          fyd_registro_salvar(const gchar *chave_completa, const gchar *valor);

// ==========================================
// ✅ Macro inteligente — ACEITA 2 OU 3 argumentos
// ==========================================
#define FYD_REG_DISP(a,b,c,d,...)  d
#define FYD_REG_3(s,c,d)           fyd_reg_ler(s,c,d)
#define FYD_REG_2(s,c)             fyd_reg_ler(s,c,"")
#define fyd_registro_ler(...)      FYD_REG_DISP(__VA_ARGS__,FYD_REG_3,FYD_REG_2)(__VA_ARGS__)

#define FYD_REG_INT_DISP(a,b,c,d,...)  d
#define FYD_REG_INT_3(s,c,d)           fyd_reg_ler_int(s,c,d)
#define FYD_REG_INT_2(s,c)             fyd_registro_ler_int_dir(s "/" c, c)
#define fyd_registro_ler_int(...)      FYD_REG_INT_DISP(__VA_ARGS__,FYD_REG_INT_3,FYD_REG_INT_2)(__VA_ARGS__)

#endif
