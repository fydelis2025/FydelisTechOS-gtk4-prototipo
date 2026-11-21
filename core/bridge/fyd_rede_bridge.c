#include "fyd_rede_bridge.h"
#include "../fyd_ponte.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>   // ✅ para malloc

// ✅ Substitui strdup por implementação própria
static char* duplicar_string(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

char* ui_rede_ler_nome_interface(void) {
    const char* nome_bruto = fyd_ler_nome_rede();
    if (!nome_bruto) return NULL;
    return duplicar_string(nome_bruto);  // ✅ não usa strdup
}

// ... resto das funções ...

int ui_rede_listar_wifi(RedeWiFi* buffer, int max_redes) {
    if (!buffer || max_redes <= 0) return 0;
    int qtd = 0;
    const FydWifiRede* lista = fyd_wifi_escanear(&qtd);  // ✅ retorna ponteiro simples, não duplo
    for (int i = 0; i < qtd && i < max_redes; i++) {
        strncpy(buffer[i].ssid, lista[i].ssid, 63);
        buffer[i].ssid[63] = '\0';
        snprintf(buffer[i].bssid, sizeof(buffer[i].bssid), "%s", "00:00:00:00:00:00");
        buffer[i].sinal = 75;
        strncpy(buffer[i].seguranca, "WPA2-Personal", 31);
        buffer[i].seguranca[31] = '\0';
    }
    return qtd;
}

