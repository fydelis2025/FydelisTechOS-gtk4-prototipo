#ifndef BLUETOOTH_BRIDGE_H
#define BLUETOOTH_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ❌ REMOVER #include "../fyd_ponte.h" — causa conflito!

typedef struct {
    char endereco[18];
    char nome[64];
    int  sinal;
    bool conectado;
} BtDispositivoUI;

bool bluetooth_inicializar(void);
bool bluetooth_disponivel(void);
int  bluetooth_escanear(BtDispositivoUI* buffer, int max_dispositivos);
bool bluetooth_conectar(const char* endereco);
bool bluetooth_conectar_por_nome(const char* nome);
bool bluetooth_desconectar(void);
bool bluetooth_enviar_texto(const char* mensagem);
int  bluetooth_receber_texto(char* buffer_out, int max_len);

void bluetooth_bdaddr_para_string(const uint8_t* end, char* buffer_out);

#ifdef __cplusplus
extern "C" {
}
#endif
#endif

