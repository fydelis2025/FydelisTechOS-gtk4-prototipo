#include "fyd_bluetooth_bridge.h"
#include "../fyd_ponte.h"
#include <string.h>
#include <stdio.h>

bool bluetooth_inicializar(void) {
    return fyd_bt_inicializar() == 0;
}

bool bluetooth_disponivel(void) {
    return fyd_bt_detectar() == 1;
}

int bluetooth_escanear(BtDispositivoUI* buffer, int max_dispositivos) {
    if (!buffer || max_dispositivos <= 0) return 0;
    
    // ✅ Ajusta conforme nome real da struct no seu sistema
    // Usando dados temporários até confirmar nomes exatos
    for (int i = 0; i < max_dispositivos && i < 2; i++) {
        snprintf(buffer[i].endereco, sizeof(buffer[i].endereco),
                 i == 0 ? "12:34:56:78:9A:BC" : "FE:DC:BA:98:76:54");
        snprintf(buffer[i].nome, sizeof(buffer[i].nome),
                 i == 0 ? "Fydelis Audio" : "Fydelis Keyboard");
        buffer[i].sinal = 70 + i * 10;
        buffer[i].conectado = false;
    }
    return (max_dispositivos >= 2) ? 2 : max_dispositivos;
}

bool bluetooth_conectar(const char* endereco) {
    if (!endereco) return false;
    uint8_t bin[6];
    if (sscanf(endereco, "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX",
               &bin[0], &bin[1], &bin[2], &bin[3], &bin[4], &bin[5]) != 6)
        return false;
    extern int fyd_bt_conectar(const uint8_t*);
    return fyd_bt_conectar(bin) == 0;
}

bool bluetooth_conectar_por_nome(const char* nome) {
    if (!nome) return false;
    extern int fyd_bt_conectar_por_nome(const char*);
    return fyd_bt_conectar_por_nome(nome) == 0;
}

bool bluetooth_desconectar(void) {
    // ✅ confirma nome real no seu fyd_ponte.h
    extern int sys_bt_desconectar(void);
    return sys_bt_desconectar() == 0;
}

bool bluetooth_enviar_texto(const char* mensagem) {
    if (!mensagem) return false;
    extern int fyd_bt_enviar_dados(const void*, uint32_t);
    return fyd_bt_enviar_dados(mensagem, strlen(mensagem)) >= 0;
}

int bluetooth_receber_texto(char* buffer_out, int max_len) {
    if (!buffer_out || max_len <= 0) return 0;
    extern int fyd_bt_receber_dados(void*, uint32_t);
    buffer_out[0] = '\0';
    return fyd_bt_receber_dados(buffer_out, max_len);
}

void bluetooth_bdaddr_para_string(const uint8_t* end, char* buffer_out) {
    if (!end || !buffer_out) return;
    sprintf(buffer_out, "%02X:%02X:%02X:%02X:%02X:%02X",
            end[0], end[1], end[2], end[3], end[4], end[5]);
}

