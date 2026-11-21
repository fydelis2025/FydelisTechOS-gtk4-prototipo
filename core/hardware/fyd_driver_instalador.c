#include "fyd_driver_instalador.h"
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct FydDriverInstalador {
    gchar *id_atual;
    gchar *pasta_destino;
    gchar *caminho_arquivo;
    GOutputStream *fluxo_arquivo;
    GSocketClient *cliente;
    GInputStream *fluxo_entrada;
    gint64 tamanho_total;
    gint64 tamanho_baixado;
    
    FydProgressoCb cb_progresso;
    FydConcluidoCb cb_concluido;
    gpointer dados_usuario;
    
    bool precisa_reiniciar;
    GSubprocess *processo_instalador;
};

static FydDriverInstalador *instancia_global = NULL;

FydDriverInstalador* fyd_driver_instalador_obter(void) {
    if (!instancia_global) {
        instancia_global = g_new0(FydDriverInstalador, 1);
        instancia_global->pasta_destino = g_build_filename(
            g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD),
            "FydelisDrivers", NULL
        );
        g_mkdir_with_parents(instancia_global->pasta_destino, 0755);
    }
    return instancia_global;
}

void fyd_driver_instalador_set_callbacks(
    FydDriverInstalador *inst,
    FydProgressoCb cb_progresso,
    FydConcluidoCb cb_concluido,
    gpointer dados_usuario
) {
    if (!inst) return;
    inst->cb_progresso = cb_progresso;
    inst->cb_concluido = cb_concluido;
    inst->dados_usuario = dados_usuario;
}

static void fyd_download_concluido(FydDriverInstalador *inst, bool sucesso, const gchar *erro) {
    if (inst->fluxo_arquivo) {
        g_object_unref(inst->fluxo_arquivo);
        inst->fluxo_arquivo = NULL;
    }
    
    if (sucesso) {
        if (inst->cb_concluido) {
            inst->cb_concluido(true, inst->caminho_arquivo, inst->dados_usuario);
        }
    } else {
        if (inst->caminho_arquivo) {
            unlink(inst->caminho_arquivo);
            g_free(inst->caminho_arquivo);
            inst->caminho_arquivo = NULL;
        }
        if (inst->cb_concluido) {
            inst->cb_concluido(false, erro ? erro : "Erro no download", inst->dados_usuario);
        }
    }
}

static gboolean fyd_processar_dados(gpointer dados) {
    FydDriverInstalador *inst = (FydDriverInstalador*)dados;
    
    gchar buffer[4096];
    gssize lidos = g_input_stream_read(inst->fluxo_entrada, buffer, sizeof(buffer), NULL, NULL);
    
    if (lidos > 0) {
        g_output_stream_write(inst->fluxo_arquivo, buffer, lidos, NULL, NULL);
        inst->tamanho_baixado += lidos;
        
        if (inst->tamanho_total > 0 && inst->cb_progresso) {
            int pct = (int)((inst->tamanho_baixado * 100) / inst->tamanho_total);
            inst->cb_progresso(pct, inst->dados_usuario);
        }
        return G_SOURCE_CONTINUE;
    }
    
    if (lidos == 0) {
        fyd_download_concluido(inst, true, NULL);
    } else {
        fyd_download_concluido(inst, false, "Falha na leitura da conexão");
    }
    
    if (inst->fluxo_entrada)
        g_object_unref(inst->fluxo_entrada);
    inst->fluxo_entrada = NULL;
    return G_SOURCE_REMOVE;
}

void fyd_driver_instalador_baixar(
    FydDriverInstalador *inst,
    const gchar *url,
    const gchar *id_dispositivo
) {
    if (!inst || !url) return;
    
    g_free(inst->id_atual);
    inst->id_atual = g_strdup(id_dispositivo);
    inst->tamanho_baixado = 0;
    inst->tamanho_total = 0;
    
    gchar **partes = g_strsplit(url, "/", -1);
    gchar *nome_arq = partes ? g_strdup(partes[g_strv_length(partes) - 1]) : g_strdup("driver.tmp");
    g_strfreev(partes);
    
    g_free(inst->caminho_arquivo);
    inst->caminho_arquivo = g_build_filename(inst->pasta_destino, nome_arq, NULL);
    g_free(nome_arq);
    
    GFile *arq = g_file_new_for_path(inst->caminho_arquivo);
    inst->fluxo_arquivo = G_OUTPUT_STREAM(g_file_replace(arq, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL));
    g_object_unref(arq);
    
    if (!inst->fluxo_arquivo) {
        fyd_download_concluido(inst, false, "Não foi possível criar o arquivo");
        return;
    }
    
    GFile *arquivo_remoto = g_file_new_for_uri(url);
    inst->fluxo_entrada = G_INPUT_STREAM(g_file_read(arquivo_remoto, NULL, NULL));
    g_object_unref(arquivo_remoto);
    
    if (!inst->fluxo_entrada) {
        fyd_download_concluido(inst, false, "Não foi possível conectar");
        return;
    }
    
    g_timeout_add(50, fyd_processar_dados, inst);
}

static void fyd_monitorar_processo(GPid pid, gint status, gpointer dados) {
    FydDriverInstalador *inst = (FydDriverInstalador*)dados;
    
    bool ok = (g_spawn_check_wait_status(status, NULL));
    gchar *msg;
    
    if (ok) {
        msg = g_strdup("✅ Instalação concluída!");
        inst->precisa_reiniciar = false;
    } else {
        msg = g_strdup_printf("❌ Erro na instalação (código: %d)", status);
    }
    
    if (inst->cb_concluido) {
        inst->cb_concluido(ok, msg, inst->dados_usuario);
    }
    
    g_free(msg);
    g_spawn_close_pid(pid);
}

void fyd_driver_instalador_instalar(
    FydDriverInstalador *inst,
    const gchar *caminho_arquivo,
    const gchar *id_dispositivo
) {
    if (!inst || !caminho_arquivo) return;
    
    g_free(inst->id_atual);
    inst->id_atual = g_strdup(id_dispositivo);
    inst->precisa_reiniciar = false;
    
    gboolean exe = g_str_has_suffix(caminho_arquivo, ".exe");
    gboolean msi = g_str_has_suffix(caminho_arquivo, ".msi");
    
    gchar **argv = NULL;
    if (exe) {
        argv = g_new0(gchar*, 3);
        argv[0] = g_strdup(caminho_arquivo);
        argv[1] = g_strdup("/silent");
    } else if (msi) {
        argv = g_new0(gchar*, 4);
        argv[0] = g_strdup("msiexec");
        argv[1] = g_strdup("/i");
        argv[2] = g_strdup(caminho_arquivo);
        argv[3] = g_strdup("/quiet");
    } else {
        if (inst->cb_concluido) {
            inst->cb_concluido(false, "Formato não suportado", inst->dados_usuario);
        }
        return;
    }
    
    GPid pid;
    g_spawn_async(NULL, argv, NULL,
                   G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
                   NULL, NULL, &pid, NULL);
    g_strfreev(argv);
    
    g_child_watch_add(pid, fyd_monitorar_processo, inst);
}

bool fyd_driver_instalador_precisa_reiniciar(FydDriverInstalador *inst) {
    return inst ? inst->precisa_reiniciar : false;
}

void fyd_driver_instalador_limpar(FydDriverInstalador *inst) {
    if (!inst) inst = instancia_global;
    if (!inst) return;
    
    g_free(inst->id_atual);
    g_free(inst->pasta_destino);
    g_free(inst->caminho_arquivo);
    if (inst->fluxo_arquivo) g_object_unref(inst->fluxo_arquivo);
    if (inst->fluxo_entrada) g_object_unref(inst->fluxo_entrada);
    
    memset(inst, 0, sizeof(*inst));
    g_free(inst);
    instancia_global = NULL;
}
