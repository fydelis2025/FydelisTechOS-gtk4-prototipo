#include "fyd_ponte.h"
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ==========================================
// CHAMADA DE SISTEMA BASE — x86_64
// ==========================================
long fyd_syscall(long numero, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a" (ret)
        : "a" (numero), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4), "r" (arg5), "r" (arg6)
        : "rcx", "r11", "memory"
    );
    return ret;
}

int fyd_ponte_init(void) {
    long ret = fyd_syscall(SYS_INIT, 0, 0, 0, 0, 0, 0);
    return (ret >= 0) ? 0 : -1;
}

// ==========================================
// ARQUIVOS
// ==========================================
long fyd_arquivo_abrir(const char* caminho, long modo) {
    return fyd_syscall(SYS_DISK_ABRIR, (long)caminho, modo, 0, 0, 0, 0);
}

long fyd_arquivo_ler(int desc, void* buf, long tam) {
    return fyd_syscall(SYS_DISK_LER, desc, (long)buf, tam, 0, 0, 0);
}

long fyd_arquivo_escrever(int desc, const void* dados, long tam) {
    return fyd_syscall(SYS_DISK_ESCREVER, desc, (long)dados, tam, 0, 0, 0);
}

long fyd_arquivo_fechar(int desc) {
    return fyd_syscall(SYS_DISK_FECHAR, desc, 0, 0, 0, 0, 0);
}

// ==========================================
// REDE
// ==========================================
long fyd_rede_conectar(const char* ip, int porta) {
    return fyd_syscall(SYS_REDE_CONECTAR, (long)ip, porta, 0, 0, 0, 0);
}

long fyd_rede_enviar(int sock, const void* dados, long tam) {
    return fyd_syscall(SYS_REDE_ENVIAR, sock, (long)dados, tam, 0, 0, 0);
}

long fyd_rede_receber(int sock, void* buf, long tam) {
    return fyd_syscall(SYS_REDE_RECEBER, sock, (long)buf, tam, 0, 0, 0);
}

uint32_t fyd_ler_ip(void) {
    extern uint32_t dhcp_ip_atual(void);
    return dhcp_ip_atual();
}

const char* fyd_ler_nome_rede(void) {
    extern const char* rede_ler_iface_nome(void);
    return rede_ler_iface_nome();
}

int fyd_dhcp_iniciar(void) {
    extern int dhcp_cliente_iniciar(void);
    return dhcp_cliente_iniciar();
}

// ==========================================
// WI-FI — Usa declarações do .h, não redeclara
// ==========================================
int fyd_sys_wifi_detectar(void) {
    extern int wifi_detectar_placa(void);
    return wifi_detectar_placa();
}

int fyd_sys_wifi_inicializar(void) {
    extern int wifi_inicializar(void);
    return wifi_inicializar();
}

const FydWifiRede* fyd_sys_wifi_scan(int* out_qtd) {
    *out_qtd = 0;
    WifiRede* lista = sys_wifi_scan(); // ← JÁ vem do .h!
    if (!lista) return NULL;
    
    int qtd = 0;
    for (int i = 0; i < 32 && lista[i].ssid[0]; i++) qtd++;
    *out_qtd = qtd;
    return (const FydWifiRede*)lista;
}

int fyd_sys_wifi_conectar(const char* ssid) {
    return sys_wifi_conectar(ssid); // ← Já declarado no .h
}

int fyd_sys_wifi_conectar_completo(const char* ssid, const char* senha) {
    extern int wifi_conectar_completo(const char*, const char*);
    return wifi_conectar_completo(ssid, senha);
}

int fyd_sys_wifi_desconectar(void) {
    return sys_wifi_desconectar(); // ← Já declarado no .h
}

int fyd_sys_wifi_status(void) {
    extern int wifi_ler_status(void);
    return wifi_ler_status();
}

const char* fyd_sys_wifi_ssid(void) {
    extern const char* wifi_ler_ssid(void);
    return wifi_ler_ssid();
}

void fyd_sys_wifi_definir_senha(const char* senha) {
    extern void wifi_definir_senha(const char*);
    wifi_definir_senha(senha);
}

// ==========================================
// DISCO
// ==========================================
uint64_t fyd_disco_espaco_livre(const char* caminho) {
    extern uint64_t calcular_espaco_livre(const char*);
    return calcular_espaco_livre(caminho);
}

uint64_t fyd_disco_espaco_total(const char* caminho) {
    extern uint64_t calcular_espaco_total(const char*);
    return calcular_espaco_total(caminho);
}

int fyd_disco_reparar_tabela(const char* dispositivo) {
    extern int reparar_tabela_particoes(const char*);
    return reparar_tabela_particoes(dispositivo);
}

// ==========================================
// USB
// ==========================================
int fyd_usb_listar(UsbDispositivo* lista, int max) {
    extern int usb_listar(UsbDispositivo*, int);
    return usb_listar(lista, max);
}

int fyd_usb_ler_hid(uint8_t end, uint8_t ep, void* buf, size_t tam) {
    extern int usb_ler_hid(uint8_t, uint8_t, void*, size_t);
    return usb_ler_hid(end, ep, buf, tam);
}

int fyd_usb_escrever_hid(uint8_t end, uint8_t ep, const void* dados, size_t tam) {
    extern int usb_escrever(uint8_t, uint8_t, const void*, size_t);
    return usb_escrever(end, ep, dados, tam);
}

// ==========================================
// USUÁRIOS — Apenas repassa ao kernel
// ==========================================
bool fyd_ponte_usuario_autenticar(const char* nome, const char* senha) {
    extern int sys_login(const char*, const char*);
    return sys_login(nome, senha) == 0;
}

bool fyd_ponte_usuario_criar(const char* nome, const char* senha, uint32_t nivel) {
    extern int sys_criar_usuario(const char*, const char*, int);
    return sys_criar_usuario(nome, senha, (int)nivel) == 0;
}

int fyd_ponte_usuario_apagar(const char* nome) {
    extern int sys_remover_usuario(const char*);
    return sys_remover_usuario(nome);
}


// ==========================================
// BLUETOOTH
// ==========================================
int fyd_bt_detectar(void) {
    extern int sys_bt_detectar(void);
    return sys_bt_detectar();
}

int fyd_bt_inicializar(void) {
    extern int sys_bt_inicializar(void);
    return sys_bt_inicializar();
}

int fyd_bt_conectar_por_nome(const char* nome) {
    extern int bridge_bt_conectar_por_nome(const char*);
    return bridge_bt_conectar_por_nome(nome);
}

// ==========================================
// MOUSE
// ==========================================
int fyd_mouse_detectar(void) {
    extern int mouse_detectar(void);
    return mouse_detectar();
}

int fyd_mouse_inicializar(void) {
    extern int mouse_inicializar(void);
    return mouse_inicializar();
}

// ==========================================
// GRÁFICO
// ==========================================
int fyd_vesa_detectar(void) {
    extern int vesa_detectar(void);
    return vesa_detectar();
}

int fyd_vesa_inicializar(int w, int h, int bpp) {
    extern int vesa_inicializar(int, int, int);
    return vesa_inicializar(w, h, bpp);
}

// ==========================================
// CRIPTOGRAFIA
// ==========================================
int fyd_aes256_criptografar(const void* in, void* out, int tam, const uint8_t* chave) {
    extern int aes256_criptografar(const void*, void*, int, const uint8_t*);
    return aes256_criptografar(in, out, tam, chave);
}

int fyd_gerar_chave_aleatoria(uint8_t* buf, int tam) {
    extern int gerar_chave_aleatoria(uint8_t*, int);
    return gerar_chave_aleatoria(buf, tam);
}

// ==========================================
// IDIOMA E TECLADO
// ==========================================
uint8_t fyd_kb_ler_tecla(void) {
    extern uint8_t get_key(void);
    return get_key();
}

void fyd_kb_layout_ptbr(void) {
    extern void set_layout_ptbr(void);
    set_layout_ptbr();
}

