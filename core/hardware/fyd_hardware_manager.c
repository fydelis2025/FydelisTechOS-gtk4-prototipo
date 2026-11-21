#include "fyd_hardware_manager.h"
#include "syscalls/syscalls.h"
#include <string.h>
#include <stdio.h>

struct FydHardwareManager {
    GList *dispositivos;
    guint id_monitoramento;
    FydListaAtualizadaCb cb_atualizada;
    gpointer dados_usuario;
};

static FydHardwareManager *instancia_global = NULL;

FydHardwareManager* fyd_hardware_manager_obter(void) {
    if (!instancia_global) {
        instancia_global = g_new0(FydHardwareManager, 1);
        instancia_global->dispositivos = NULL;
        instancia_global->id_monitoramento = 0;
    }
    return instancia_global;
}

void fyd_hardware_manager_set_callback(FydHardwareManager *hm,
    FydListaAtualizadaCb cb, gpointer dados_usuario) {
    if (!hm) return;
    hm->cb_atualizada = cb;
    hm->dados_usuario = dados_usuario;
}

static const gchar* classificar_por_classe(gint codigo_classe) {
    switch (codigo_classe) {
        case FYD_DEV_VIDEO:      return "Vídeo";
        case FYD_DEV_AUDIO:      return "Áudio";
        case FYD_DEV_REDE:       return "Rede";
        case FYD_DEV_USB:        return "USB";
        case FYD_DEV_ARMAZENAMENTO: return "Armazenamento";
        case FYD_DEV_IMPRESSORA: return "Impressora";
        case FYD_DEV_ENTRADA:    return "Entrada";
        case FYD_DEV_BIOS:       return "Sistema";
        default:                 return "Outro";
    }
}

static const gchar* icone_para_tipo(const gchar *tipo) {
    if (strcmp(tipo, "Vídeo") == 0) return "🎬";
    if (strcmp(tipo, "Áudio") == 0) return "🔊";
    if (strcmp(tipo, "Rede") == 0) return "🌐";
    if (strcmp(tipo, "USB") == 0) return "🔌";
    if (strcmp(tipo, "Armazenamento") == 0) return "💽";
    if (strcmp(tipo, "Impressora") == 0) return "🖨️";
    if (strcmp(tipo, "Entrada") == 0) return "⌨️";
    return "🔧";
}

static void liberar_dispositivo(gpointer d, gpointer usuario) {
    (void)usuario;
    FydDispositivo *dev = d;
    if (!dev) return;
    g_free(dev->nome);
    g_free(dev->fabricante);
    g_free(dev->id);
    g_free(dev->tipo);
    g_free(dev->status);
    g_free(dev);
}

static void limpar_tudo(FydHardwareManager *hm) {
    g_list_foreach(hm->dispositivos, liberar_dispositivo, NULL);
    g_list_free(hm->dispositivos);
    hm->dispositivos = NULL;
}

bool fyd_hardware_manager_inicializar(FydHardwareManager *hm) {
    fyd_hardware_manager_escanear(hm);
    return true;
}

void fyd_hardware_manager_escanear(FydHardwareManager *hm) {
    if (!hm) return;
    limpar_tudo(hm);

    FydDevLista lista;
    int ret = sys_fyd_dev_listar(&lista);

    if (ret != 0) {
        g_print("[Fyd] Kernel respondeu: sem dispositivos ou erro\n");
        if (hm->cb_atualizada) hm->cb_atualizada(hm->dados_usuario);
        return;
    }

    for (size_t i = 0; i < lista.quantidade; i++) {
        FydDevEntrada *ent = &lista.itens[i];

        FydDispositivo *dev = g_new0(FydDispositivo, 1);
        dev->id = g_strdup(ent->id);
        dev->nome = g_strdup(ent->nome);
        dev->fabricante = g_strdup(ent->fabricante);
        dev->classe = ent->classe;
        dev->tipo = g_strdup(classificar_por_classe(ent->classe));
        dev->status = g_strdup(ent->status);
        dev->tem_driver = ent->tem_driver;

        hm->dispositivos = g_list_append(hm->dispositivos, dev);
    }

    sys_fyd_dev_liberar(&lista);

    if (hm->cb_atualizada)
        hm->cb_atualizada(hm->dados_usuario);
}

const GList* fyd_hardware_manager_lista(FydHardwareManager *hm) {
    return hm ? hm->dispositivos : NULL;
}

static gboolean cb_verificar_hardware(gpointer dados) {
    FydHardwareManager *hm = dados;
    fyd_hardware_manager_escanear(hm);
    return G_SOURCE_CONTINUE;
}

void fyd_hardware_manager_iniciar_monitoramento(FydHardwareManager *hm, int ms) {
    if (!hm) return;
    if (hm->id_monitoramento > 0)
        g_source_remove(hm->id_monitoramento);
    hm->id_monitoramento = g_timeout_add(ms, cb_verificar_hardware, hm);
}

void fyd_hardware_manager_parar_monitoramento(FydHardwareManager *hm) {
    if (!hm && instancia_global) hm = instancia_global;
    if (!hm) return;
    if (hm->id_monitoramento > 0) {
        g_source_remove(hm->id_monitoramento);
        hm->id_monitoramento = 0;
    }
}

void fyd_hardware_manager_limpar(FydHardwareManager *hm) {
    if (!hm) hm = instancia_global;
    if (!hm) return;
    fyd_hardware_manager_parar_monitoramento(hm);
    limpar_tudo(hm);
    g_free(hm);
    instancia_global = NULL;
}

