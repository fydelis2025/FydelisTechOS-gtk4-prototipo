#ifndef FYD_GUARD_H
#define FYD_GUARD_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FG_BL_Nenhum = 0,
    FG_BL_IP_ReputacaoRuim,
    FG_BL_SpoofingDetectado,
    FG_BL_TaxaExcedida,
    FG_BL_ProtocoloAnomalo,
    FG_BL_SemAssinaturaValida,
    FG_BL_SaltosExcedidos,
    FG_BL_HorarioAnomalo,
    FG_BL_TamanhoSuspeito,
    FG_BL_ListaNegraManual,
    FG_BL_CertificadoInvalido,
    FG_BL_CorrelacaoSuspeita
} FGBlockReason;

typedef enum {
    FG_VERD_Permitir = 0,
    FG_VERD_Bloquear,
    FG_VERD_Descartar,
    FG_VERD_Desafiar,
    FG_VERD_Quarentena
} FGVeredito;

typedef struct {
    uint32_t ipOrigem;
    uint32_t ipDestino;
    uint16_t portaOrigem;
    uint16_t portaDestino;
    char protocolo[32];
    int64_t tamanhoBytes;
    time_t timestamp;
    int saltos;
    bool temAssinatura;
    char assinaturaRemetente[128];
    char idDispositivo[64];
    char impressaoDigital[128];
} FGPacoteInfo;

typedef struct {
    time_t horario;
    uint32_t ip;
    FGBlockReason motivo;
    char detalhe[256];
    FGVeredito acaoTomada;
} FGEvento;

typedef void (*FGAmeacaCallback)(FGBlockReason motivo, const FGPacoteInfo* pacote, void* user_data);
typedef void (*FGLimiteCallback)(uint32_t ip, void* user_data);
typedef void (*FGGericoCallback)(void* user_data);

typedef struct FydGuard FydGuard;

struct FydGuard {
    bool ativo;
    bool protecao_maxima;
    int limite_pacotes_por_minuto;
    int contador_bloqueados;
    int contador_permitidos;

    struct {
        uint32_t ip;
        char motivo[128];
    } lista_negra[256];
    int qtd_lista_negra;

    FGEvento eventos[100];
    int evento_inicio;
    int evento_qtd;

    FGAmeacaCallback cb_ameaca;
    void* user_data_ameaca;

    FGLimiteCallback cb_limite;
    void* user_data_limite;

    FGGericoCallback cb_ativacao;
    FGGericoCallback cb_desativacao;
    void* user_data_estado;

    pthread_mutex_t mutex;
};

FydGuard* fyd_guard_criar(void);
void      fyd_guard_destruir(FydGuard* guard);

bool      fyd_guard_inicializar(FydGuard* guard, bool modoProtecaoMaxima);
void      fyd_guard_desativar(FydGuard* guard);
bool      fyd_guard_esta_ativo(const FydGuard* guard);

FGVeredito fyd_guard_inspecionar_pacote(FydGuard* guard, const FGPacoteInfo* pacote);

void      fyd_guard_adicionar_lista_negra(FydGuard* guard, uint32_t ip, const char* motivo);
void      fyd_guard_remover_lista_negra(FydGuard* guard, uint32_t ip);
void      fyd_guard_definir_limite_taxa(FydGuard* guard, int pacotesPorMinuto);
void      fyd_guard_definir_modo_protecao(FydGuard* guard, bool maximo);

int       fyd_guard_pacotes_bloqueados(const FydGuard* guard);
int       fyd_guard_pacotes_permitidos(const FydGuard* guard);

void      fyd_guard_registrar_callback_ameaca(FydGuard* guard, FGAmeacaCallback cb, void* user_data);
void      fyd_guard_registrar_callback_limite(FydGuard* guard, FGLimiteCallback cb, void* user_data);
void      fyd_guard_registrar_callbacks_estado(FydGuard* guard, FGGericoCallback ativacao, FGGericoCallback desativacao, void* user_data);

#ifdef __cplusplus
}
#endif

#endif

