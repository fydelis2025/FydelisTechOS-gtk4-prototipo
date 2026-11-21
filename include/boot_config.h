#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define BOOT_CAMINHO "/boot/fyd_boot.cfg"
#define MAX_ENTRADAS 5

typedef enum {
    BOOT_ACAO_GRAFICO,
    BOOT_ACAO_TERMINAL,
    BOOT_ACAO_SEGURO
} BootAcao;

typedef struct {
    char rotulo[128];
    char comando[64];
    char descricao[256];
    BootAcao acao;
} BootEntrada;

typedef struct {
    char kernel[128];
    char modo_video[32];
    char resolucao[32];
    int tempo_espera;
    
    char padrao[64];
    BootEntrada entradas[MAX_ENTRADAS];
    int qtd_entradas;
    
    bool usar_troca;
    char tamanho_troca[16];
    bool hibernacao;
    
    bool tela_splash;
    char carregar_drivers[32];
    char log_nivel[16];
    char imagem_fundo[256];
} BootConfig;

// Carrega e faz o parse completo do fyd_boot.cfg
int boot_carregar(const char *caminho, BootConfig *cfg);

// Descobre qual é a ação padrão
BootAcao boot_obter_acao_padrao(BootConfig *cfg);

#endif

