/**
 * @file fyd_associar.c
 * @brief Associação de Extensões — FydelisTechOS v4.0
 * Arquivo único: /fyd/associacoes/fyd_extensoes.fydassoc
 * Formato: .ext = app | descrição | icone
 */

#include "fyd_desktop.h"
#include "fyd_registro.h"
#include <stdio.h>
#include <string.h>
#include <glib.h>

typedef struct {
    gchar *extensao;
    gchar *aplicativo;
    gchar *descricao;
    gchar *icone;
} FydAssociacao;

static GHashTable *tabela_assoc = NULL;

/* ==========================================
   📂 CAMINHO DO ARQUIVO — do registro
   ========================================== */
static const gchar* fyd_caminho_arquivo_assoc(void) {
    // Usar caminho do registro ou padrão
    const gchar *pasta = fyd_registro_ler("Caminhos/associacoes", "/fyd/associacoes");
    static char caminho[256];
    snprintf(caminho, sizeof(caminho), "%s/fyd_extensoes.fydassoc", pasta);
    return caminho;
}

/* ==========================================
   🧹 LIMPAR ESPAÇOS da string
   ========================================== */
static void fyd_limpar_espacos(gchar *str) {
    gchar *inicio = str;
    gchar *fim;
    while (*inicio == ' ' || *inicio == '\t') inicio++;
    if (*inicio == '\0') { *str = '\0'; return; }
    fim = inicio + strlen(inicio) - 1;
    while (fim > inicio && (*fim == ' ' || *fim == '\t' || *fim == '\n' || *fim == '\r'))
        *fim-- = '\0';
    if (inicio != str) memmove(str, inicio, strlen(inicio) + 1);
}

/* ==========================================
   📋 CARREGAR TUDO do arquivo único
   ========================================== */
void fyd_associar_inicializar(void) {
    if (tabela_assoc) {
        g_hash_table_destroy(tabela_assoc);
        tabela_assoc = NULL;
    }
    
    tabela_assoc = g_hash_table_new_full(
        g_str_hash,
        g_str_equal,
        g_free,
        (GDestroyNotify)g_free
    );
    
    const gchar *caminho = fyd_caminho_arquivo_assoc();
    FILE *f = fopen(caminho, "r");
    
    if (!f) {
        g_warning("[Assoc] ⚠️ Arquivo não encontrado: %s\n", caminho);
        return;
    }

    gchar linha[512];
    int contador = 0;
    
    while (fgets(linha, sizeof linha, f)) {
        linha[strcspn(linha, "\r\n")] = 0;
        
        // Ignorar comentários, seções e linhas vazias
        if (!linha[0] || linha[0] == '#' || linha[0] == '[')
            continue;

        // .extensao = app | descricao | icone
        gchar *igual = strchr(linha, '=');
        if (!igual) continue;
        
        *igual = '\0';
        gchar *ext = linha;
        gchar *valor = igual + 1;
        
        fyd_limpar_espacos(ext);
        fyd_limpar_espacos(valor);
        
        if (ext[0] != '.') continue;

        // Separar por "|"
        gchar *pipe1 = strchr(valor, '|');
        gchar *pipe2 = pipe1 ? strchr(pipe1 + 1, '|') : NULL;
        
        FydAssociacao *a = g_new0(FydAssociacao, 1);
        a->extensao = g_strdup(ext);
        
        if (pipe1 && pipe2) {
            *pipe1 = '\0';
            *pipe2 = '\0';
            gchar *app = valor;
            gchar *desc = pipe1 + 1;
            gchar *ico = pipe2 + 1;
            fyd_limpar_espacos(app);
            fyd_limpar_espacos(desc);
            fyd_limpar_espacos(ico);
            a->aplicativo = g_strdup(app);
            a->descricao  = g_strdup(desc);
            a->icone      = g_strdup(ico);
        } else if (pipe1) {
            *pipe1 = '\0';
            gchar *app = valor;
            gchar *desc = pipe1 + 1;
            fyd_limpar_espacos(app);
            fyd_limpar_espacos(desc);
            a->aplicativo = g_strdup(app);
            a->descricao  = g_strdup(desc);
            a->icone[0] = '\0';
        } else {
            gchar *app = valor;
            fyd_limpar_espacos(app);
            a->aplicativo = g_strdup(app);
            a->descricao[0] = '\0';
            a->icone[0] = '\0';
        }

        if (a->aplicativo && a->aplicativo[0]) {
            g_hash_table_insert(tabela_assoc, g_strdup(a->extensao), a);
            g_print("[Assoc] ✅ %s → %s\n", a->extensao, a->aplicativo);
            contador++;
        } else {
            g_free(a->extensao);
            g_free(a->aplicativo);
            g_free(a->descricao);
            g_free(a->icone);
            g_free(a);
        }
    }

    fclose(f);
    g_print("[Assoc] 📋 Arquivo: %s | %d extensões carregadas\n", caminho, contador);
}

/* ==========================================
   🖱️ ABRIR ARQUIVO com app registrado
   ========================================== */
gboolean fyd_associar_abrir(const gchar *caminho_arquivo) {
    if (!tabela_assoc)
        fyd_associar_inicializar();

    if (!caminho_arquivo || !caminho_arquivo[0])
        return FALSE;

    gchar *ext = strrchr(caminho_arquivo, '.');
    if (!ext) {
        g_print("[Assoc] ⚠️ Sem extensão: %s\n", caminho_arquivo);
        return FALSE;
    }

    FydAssociacao *a = g_hash_table_lookup(tabela_assoc, ext);
    if (!a) {
        g_print("[Assoc] ⚠️ Nenhum app para %s\n", ext);
        return FALSE;
    }

    // Montar comando: app "caminho_do_arquivo"
    gchar *cmd = g_strdup_printf("%s \"%s\"", a->aplicativo, caminho_arquivo);
    g_print("[Assoc] 🚀 %s\n", cmd);

    GError *erro = NULL;
    g_spawn_command_line_async(cmd, &erro);

    if (erro) {
        g_warning("[Assoc] ❌ Erro: %s\n", erro->message);
        g_error_free(erro);
        g_free(cmd);
        return FALSE;
    }

    g_free(cmd);
    return TRUE;
}

/* ==========================================
   🎨 ÍCONE para exibir
   ========================================== */
const gchar* fyd_associar_icone(const gchar *caminho) {
    if (!tabela_assoc) fyd_associar_inicializar();
    if (!caminho) return "📄";
    gchar *ext = strrchr(caminho, '.');
    if (!ext) return "📄";
    FydAssociacao *a = g_hash_table_lookup(tabela_assoc, ext);
    return (a && a->icone && a->icone[0]) ? a->icone : "📄";
}

/* ==========================================
   🏷️ NOME/DESCRIÇÃO do tipo
   ========================================== */
const gchar* fyd_associar_nome(const gchar *caminho) {
    if (!tabela_assoc) fyd_associar_inicializar();
    if (!caminho) return "Arquivo";
    gchar *ext = strrchr(caminho, '.');
    if (!ext) return "Arquivo";
    FydAssociacao *a = g_hash_table_lookup(tabela_assoc, ext);
    return (a && a->descricao && a->descricao[0]) ? a->descricao : "Arquivo";
}

/* ==========================================
   📋 LISTAR TODAS (para configurações)
   ========================================== */
GList* fyd_associar_listar(void) {
    if (!tabela_assoc) fyd_associar_inicializar();
    GList *lista = NULL;
    GHashTableIter iter;
    gpointer chave, valor;
    g_hash_table_iter_init(&iter, tabela_assoc);
    while (g_hash_table_iter_next(&iter, &chave, &valor))
        lista = g_list_prepend(lista, valor);
    return g_list_reverse(lista);
}
