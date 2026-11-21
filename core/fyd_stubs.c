/**
 * @file fyd_stubs.c
 * @brief Stubs — Funções Temporárias para Ligação
 * Substituídas pelo kernel real quando pronto
 * FydelisTech OS v4.3
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ==========================================
// 📡 Rede / DHCP
// ==========================================
uint32_t dhcp_ip_atual(void) { return 0; }
const char* rede_ler_iface_nome(void) { return "wlan0"; }
int dhcp_cliente_iniciar(void) { return -1; }

// ==========================================
// 📶 Wi-Fi
// ==========================================
typedef struct {
    char ssid[32];
    int canal;
    int forca;
} WifiRede;

int wifi_detectar_placa(void) { return -1; }
int wifi_inicializar(void) { return -1; }
WifiRede* sys_wifi_scan(void) { return NULL; }
int sys_wifi_conectar(const char *ssid) { (void)ssid; return -1; }
int wifi_conectar_completo(const char *ssid, const char *senha) {
    (void)ssid; (void)senha; return -1;
}
int sys_wifi_desconectar(void) { return -1; }
int wifi_ler_status(void) { return 0; }
const char* wifi_ler_ssid(void) { return NULL; }
void wifi_definir_senha(const char *senha) { (void)senha; }

// ==========================================
// 💾 Disco / Partições
// ==========================================
uint64_t calcular_espaco_livre(const char *caminho) { (void)caminho; return 0; }
uint64_t calcular_espaco_total(const char *caminho) { (void)caminho; return 0; }
int reparar_tabela_particoes(const char *disco) { (void)disco; return -1; }

// ==========================================
// 🖱️ USB / Mouse
// ==========================================
typedef struct {
    uint16_t vid;
    uint16_t pid;
    char nome[64];
} UsbDispositivo;

int usb_listar(UsbDispositivo *lista, int max) { (void)lista; (void)max; return 0; }
int usb_ler_hid(unsigned char end, unsigned char ep, void *dados, size_t tam) {
    (void)end; (void)ep; (void)dados; (void)tam; return -1;
}
int usb_escrever(unsigned char end, unsigned char ep, const void *dados, size_t tam) {
//    ↑ NOME CERTO — igual ao que a ponte chama!
    (void)end; (void)ep; (void)dados; (void)tam; return -1;
}
int mouse_detectar(void) { return -1; }
int mouse_inicializar(void) { return -1; }

// ==========================================
// 👤 Usuários — ✅ NOMES ALINHADOS com fyd_ponte.c
// ==========================================
int sys_login(const char *usuario, const char *senha) { (void)usuario; (void)senha; return -1; }
int sys_criar_usuario(const char *nome, const char *senha, int nivel) {
    (void)nome; (void)senha; (void)nivel; return -1;
}
int sys_remover_usuario(const char *nome) { (void)nome; return -1; }

// ==========================================
// 📶 Bluetooth
// ==========================================
int sys_bt_detectar(void) { return -1; }
int sys_bt_inicializar(void) { return -1; }
int bridge_bt_conectar_por_nome(const char *nome) { (void)nome; return -1; }
int sys_bt_desconectar(void) { return -1; }
int sys_bt_enviar(const void *dados, int tam) { (void)dados; (void)tam; return -1; }
int sys_bt_receber(void *buffer, int max) { (void)buffer; (void)max; return -1; }

// ==========================================
// 🖥️ VESA / Gráficos
// ==========================================
int vesa_detectar(void) { return -1; }
int vesa_inicializar(int w, int h, int bpp) { (void)w; (void)h; (void)bpp; return -1; }

// ==========================================
// 🔐 Criptografia
// ==========================================
int aes256_criptografar(const void *in, void *out, int tam, const uint8_t *chave) {
    (void)in; (void)out; (void)tam; (void)chave; return -1;
}
int gerar_chave_aleatoria(uint8_t *saida, int tam) { (void)saida; (void)tam; return -1; }

// ==========================================
// ⌨️ Teclado / Idioma
// ==========================================
uint8_t get_key(void) { return 0; }
void set_layout_ptbr(void) { }

// ==========================================
// ✅ FUNÇÕES DO SISTEMA — usadas pela interface
// ==========================================
long fyd_log(int nivel, const char *mensagem) {
    (void)nivel; (void)mensagem;
    return 0;
}

long fyd_gfx_iniciar(void) {
    return -1;
}

int fyd_painel_controle_abrir(void) {
    return -1;
}

int fyd_bt_conectar(const char *endereco) {
    (void)endereco;
    return -1;
}

int fyd_bt_enviar_dados(const void *dados, int tam) {
    return sys_bt_enviar(dados, tam);
}

int fyd_bt_receber_dados(void *buffer, int max) {
    return sys_bt_receber(buffer, max);
}

int sys_fyd_dev_listar(void *lista, int max) {
    (void)lista; (void)max;
    return 0;
}

int sys_fyd_dev_liberar(void *lista) {
    (void)lista;
    return 0;
}

