#ifndef REDE_BRIDGE_H
#define REDE_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    uint64_t bytesRx;
    uint64_t bytesTx;
    uint64_t pacotesRx;
    uint64_t pacotesTx;
} RedeEstatisticas;

typedef struct {
    char ssid[64];
    char bssid[20];
    int sinal;
    char seguranca[32];
} RedeWiFi;

// ✅ NOME NOVO: prefixo "ui_" para NUNCA conflitar com syscalls
char* ui_rede_ler_nome_interface(void);
char* ui_rede_ler_mac(void);
char* ui_rede_ler_ip(void);
char* ui_rede_ler_mascara(void);
char* ui_rede_ler_gateway(void);
void  ui_rede_ler_dns(char* dns1_out, char* dns2_out);
bool  ui_rede_link_ativo(void);

RedeEstatisticas ui_rede_ler_estatisticas(void);
int  ui_rede_listar_wifi(RedeWiFi* buffer, int max_redes);
bool ui_rede_conectar_wifi(const char* ssid);
bool ui_rede_desconectar_wifi(void);
bool ui_rede_tem_wifi_disponivel(void);
bool ui_rede_pingar(const char* ipOuHost);

uint32_t rede_ip_string_para_uint(const char* ip);
void     rede_ip_uint_para_string(uint32_t ip, char* buffer_out);
void     rede_mac_para_string(const uint8_t* mac, char* buffer_out);

#ifdef __cplusplus
extern "C" {
}
#endif
#endif

