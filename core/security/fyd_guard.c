#include "fyd_guard.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_BLACKLIST 256
#define MAX_EVENTOS 100

FydGuard* fyd_guard_criar(void) {
    FydGuard* guard = (FydGuard*)calloc(1, sizeof(FydGuard));
    if (!guard) return NULL;
    guard->limite_pacotes_por_minuto = 60;
    pthread_mutex_init(&guard->mutex, NULL);
    return guard;
}

void fyd_guard_destruir(FydGuard* guard) {
    if (!guard) return;
    pthread_mutex_destroy(&guard->mutex);
    free(guard);
}

bool fyd_guard_inicializar(FydGuard* guard, bool modoProtecaoMaxima) {
    if (!guard) return false;
    pthread_mutex_lock(&guard->mutex);
    guard->ativo = true;
    guard->protecao_maxima = modoProtecaoMaxima;
    guard->contador_bloqueados = 0;
    guard->contador_permitidos = 0;
    guard->qtd_lista_negra = 0;
    guard->evento_inicio = 0;
    guard->evento_qtd = 0;
    if (guard->cb_ativacao)
        guard->cb_ativacao(guard->user_data_estado);
    pthread_mutex_unlock(&guard->mutex);
    return true;
}

void fyd_guard_desativar(FydGuard* guard) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->ativo = false;
    if (guard->cb_desativacao)
        guard->cb_desativacao(guard->user_data_estado);
    pthread_mutex_unlock(&guard->mutex);
}

bool fyd_guard_esta_ativo(const FydGuard* guard) {
    if (!guard) return false;
    return guard->ativo;
}

static void registrar_evento_interno(FydGuard* guard, uint32_t ip, 
                                      FGBlockReason motivo, const char* detalhe, 
                                      FGVeredito acao) {
    if (!guard) return;
    FGEvento* ev = &guard->eventos[(guard->evento_inicio + guard->evento_qtd) % MAX_EVENTOS];
    ev->horario = time(NULL);
    ev->ip = ip;
    ev->motivo = motivo;
    ev->acaoTomada = acao;
    snprintf(ev->detalhe, sizeof(ev->detalhe), "%s", detalhe ? detalhe : "");

    if (guard->evento_qtd < MAX_EVENTOS)
        guard->evento_qtd++;
    else
        guard->evento_inicio = (guard->evento_inicio + 1) % MAX_EVENTOS;
}

FGVeredito fyd_guard_inspecionar_pacote(FydGuard* guard, const FGPacoteInfo* pacote) {
    if (!guard || !guard->ativo || !pacote)
        return FG_VERD_Permitir;

    pthread_mutex_lock(&guard->mutex);

    for (int i = 0; i < guard->qtd_lista_negra; i++) {
        if (guard->lista_negra[i].ip == pacote->ipOrigem) {
            guard->contador_bloqueados++;
            registrar_evento_interno(guard, pacote->ipOrigem, 
                FG_BL_ListaNegraManual, guard->lista_negra[i].motivo, FG_VERD_Bloquear);
            if (guard->cb_ameaca)
                guard->cb_ameaca(FG_BL_ListaNegraManual, pacote, guard->user_data_ameaca);
            pthread_mutex_unlock(&guard->mutex);
            return FG_VERD_Bloquear;
        }
    }

    if (pacote->saltos > 15) {
        guard->contador_bloqueados++;
        registrar_evento_interno(guard, pacote->ipOrigem, 
            FG_BL_SaltosExcedidos, "Muitos saltos intermediários detectados", FG_VERD_Bloquear);
        if (guard->cb_ameaca)
            guard->cb_ameaca(FG_BL_SaltosExcedidos, pacote, guard->user_data_ameaca);
        pthread_mutex_unlock(&guard->mutex);
        return FG_VERD_Bloquear;
    }

    if (guard->protecao_maxima && !pacote->temAssinatura) {
        guard->contador_bloqueados++;
        registrar_evento_interno(guard, pacote->ipOrigem, 
            FG_BL_SemAssinaturaValida, "Modo máximo: assinatura obrigatória", FG_VERD_Desafiar);
        if (guard->cb_ameaca)
            guard->cb_ameaca(FG_BL_SemAssinaturaValida, pacote, guard->user_data_ameaca);
        pthread_mutex_unlock(&guard->mutex);
        return FG_VERD_Desafiar;
    }

    guard->contador_permitidos++;
    pthread_mutex_unlock(&guard->mutex);
    return FG_VERD_Permitir;
}

void fyd_guard_adicionar_lista_negra(FydGuard* guard, uint32_t ip, const char* motivo) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    if (guard->qtd_lista_negra < MAX_BLACKLIST) {
        guard->lista_negra[guard->qtd_lista_negra].ip = ip;
        snprintf(guard->lista_negra[guard->qtd_lista_negra].motivo, 
                 sizeof(guard->lista_negra[0].motivo), "%s", motivo ? motivo : "Bloqueio manual");
        guard->qtd_lista_negra++;
    }
    pthread_mutex_unlock(&guard->mutex);
}

void fyd_guard_remover_lista_negra(FydGuard* guard, uint32_t ip) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    for (int i = 0; i < guard->qtd_lista_negra; i++) {
        if (guard->lista_negra[i].ip == ip) {
            for (int j = i; j < guard->qtd_lista_negra - 1; j++)
                guard->lista_negra[j] = guard->lista_negra[j + 1];
            guard->qtd_lista_negra--;
            break;
        }
    }
    pthread_mutex_unlock(&guard->mutex);
}

void fyd_guard_definir_limite_taxa(FydGuard* guard, int pacotesPorMinuto) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->limite_pacotes_por_minuto = pacotesPorMinuto;
    pthread_mutex_unlock(&guard->mutex);
}

void fyd_guard_definir_modo_protecao(FydGuard* guard, bool maximo) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->protecao_maxima = maximo;
    pthread_mutex_unlock(&guard->mutex);
}

int fyd_guard_pacotes_bloqueados(const FydGuard* guard) {
    if (!guard) return 0;
    return guard->contador_bloqueados;
}

int fyd_guard_pacotes_permitidos(const FydGuard* guard) {
    if (!guard) return 0;
    return guard->contador_permitidos;
}

void fyd_guard_registrar_callback_ameaca(FydGuard* guard, 
        FGAmeacaCallback cb, void* user_data) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->cb_ameaca = cb;
    guard->user_data_ameaca = user_data;
    pthread_mutex_unlock(&guard->mutex);
}

void fyd_guard_registrar_callback_limite(FydGuard* guard, 
        FGLimiteCallback cb, void* user_data) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->cb_limite = cb;
    guard->user_data_limite = user_data;
    pthread_mutex_unlock(&guard->mutex);
}

void fyd_guard_registrar_callbacks_estado(FydGuard* guard, 
        FGGericoCallback ativacao, FGGericoCallback desativacao, void* user_data) {
    if (!guard) return;
    pthread_mutex_lock(&guard->mutex);
    guard->cb_ativacao = ativacao;
    guard->cb_desativacao = desativacao;
    guard->user_data_estado = user_data;
    pthread_mutex_unlock(&guard->mutex);
}

