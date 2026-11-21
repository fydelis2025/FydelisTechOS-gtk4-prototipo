#ifndef FYD_PONTE_H
#define FYD_PONTE_H

#include <stddef.h>
#include <stdint.h>
#include "fyd_desktop.h"
#include "fyd_usuarios.h"

#include "syscalls/numeros.h"

// ========== TODAS AS SYSCALLS / ESTRUTURAS ==========
#include "syscalls/security_syscalls.h"
#include "syscalls/rede_syscalls.h"       // rede cabeada + DHCP
#include "syscalls/lang_syscalls.h"
#include "syscalls/keyboard_syscalls.h"
#include "syscalls/disktools_syscalls.h"   // disco, formatação, verificação
#include "syscalls/partition_syscalls.h"    // partições
#include "syscalls/usb_syscalls.h"         // USB
#include "syscalls/user_syscalls.h"        // usuários, login, permissões
#include "syscalls/virtual_syscalls.h"
#include "syscalls/graphics.h"
#include "syscalls/bt_syscalls.h"
#include "syscalls/mouse_syscalls.h"
#include "syscalls/syscalls_wifi.h"        // Wi-Fi

// ========== NÍVEIS DE LOG ==========
#define FYD_LOG_INFO     0
#define FYD_LOG_AVISO    1
#define FYD_LOG_ERRO     2
#define FYD_LOG_CRITICO  3

// ========== CHAMADA BASE ==========
long fyd_syscall(long numero, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);
int  fyd_ponte_init(void);

// ==================================================
// 🔌 REDE — Cabeada + Wi-Fi + DHCP
// ==================================================
// Rede Cabeada
long        fyd_rede_conectar(const char* ip, int porta);
long        fyd_rede_enviar(int sock, const void* dados, long tam);
long        fyd_rede_receber(int sock, void* buf, long tam);
uint32_t    fyd_ler_ip(void);
const char* fyd_ler_nome_rede(void);
int         fyd_dhcp_iniciar(void);

// Wi-Fi
static inline int            fyd_wifi_detectar_placa(void)        { return fyd_sys_wifi_detectar(); }
static inline int            fyd_wifi_inicializar(void)            { return fyd_sys_wifi_inicializar(); }
static inline const FydWifiRede* fyd_wifi_escanear(int* qtd)       { return fyd_sys_wifi_scan(qtd); }
static inline int            fyd_wifi_conectar(const char* ssid)  { return fyd_sys_wifi_conectar(ssid); }
static inline int            fyd_wifi_conectar_completo(const char* ssid, const char* senha)
                                                                   { return fyd_sys_wifi_conectar_completo(ssid, senha); }
static inline int            fyd_wifi_desconectar(void)           { return fyd_sys_wifi_desconectar(); }
static inline int            fyd_wifi_esta_conectado(void)         { return fyd_sys_wifi_conectado(); }
static inline const char*    fyd_wifi_ler_ssid(void)               { return fyd_sys_wifi_ssid(); }
static inline int            fyd_wifi_ler_status(void)             { return fyd_sys_wifi_status(); }
static inline void           fyd_wifi_definir_senha(const char* s) { fyd_sys_wifi_definir_senha(s); }

// ==================================================
// 💾 DISCO E PARTICIONAMENTO
// ==================================================
long fyd_arquivo_abrir(const char* caminho, long modo);
long fyd_arquivo_ler(int desc, void* buf, long tam);
long fyd_arquivo_escrever(int desc, const void* dados, long tam);
long fyd_arquivo_fechar(int desc);

uint64_t fyd_disco_espaco_livre(const char* caminho);
uint64_t fyd_disco_espaco_total(const char* caminho);
int      fyd_disco_verificar_setores(const char* dispositivo);
int      fyd_disco_reparar_tabela(const char* dispositivo);
int      fyd_disco_formatar(const char* dispositivo, const char* sistema_arquivos);

// Partições (Atualizado para ParticaoInfo e assinatura correta)
int      fyd_particao_listar(const char* disco, ParticaoInfo* saida, int max);
int      fyd_particao_criar(const char* disco, int tipo, uint64_t inicio, uint64_t setores);
int      fyd_particao_apagar(const char* disco, int indice);
int      fyd_particao_montar(const char* particao, const char* ponto_montagem);
int      fyd_particao_desmontar(const char* ponto_montagem);

// ==================================================
// 🖥️ GRÁFICO / FRAMEBUFFER
// ==================================================
long        fyd_gfx_iniciar(void);
long        fyd_gfx_atualizar(const void* buf, int w, int h);
int         fyd_vesa_detectar(void);
int         fyd_vesa_inicializar(int largura, int altura, int bpp);
int         fyd_framebuffer_mapear(void);
void        fyd_desenhar_pixel(int x, int y, uint32_t cor);
void        fyd_preencher_retangulo(int x, int y, int w, int h, uint32_t cor);
ModoVideo*  fyd_obter_modo_video(void);

// ==================================================
// 🔐 SEGURANÇA / CRIPTO
// ==================================================
long     fyd_seg_cripto(const void* entrada, void* saida, long tam);
int      fyd_aes256_criptografar(const void* in, void* out, int tam, const uint8_t* chave);
int      fyd_aes256_descriptografar(const void* in, void* out, int tam, const uint8_t* chave);
int      fyd_gerar_chave_aleatoria(uint8_t* saida, int tamanho);
int      fyd_fg_bloquear_ip(const char* ip);
int      fyd_fg_desbloquear_ip(const char* ip);
int      fyd_verificar_integridade(void);

// ==================================================
// 📝 LOG
// ==================================================
long fyd_log(int nivel, const char* mensagem);

// ==================================================
// 🔵 BLUETOOTH
// ==================================================
int            fyd_bt_detectar(void);
int            fyd_bt_inicializar(void);
BtDispositivo* fyd_bt_escanear(void);
int            fyd_bt_conectar(const uint8_t* endereco);
int            fyd_bt_conectar_por_nome(const char* nome);
int            fyd_bt_enviar_dados(const void* dados, uint32_t tam);
int            fyd_bt_receber_dados(void* buffer, uint32_t tamMax);

// ==================================================
// 🖱️ MOUSE PS/2
// ==================================================
int      fyd_mouse_detectar(void);
int      fyd_mouse_inicializar(void);
void     fyd_mouse_ler_posicao(int32_t* x, int32_t* y);
uint32_t fyd_mouse_ler_botoes(void);
void     fyd_mouse_definir_posicao(int32_t x, int32_t y);
void     fyd_mouse_estado_completo(MouseEstado* out);
int      fyd_mouse_para_pixels_x(int32_t x_texto, int32_t largura_tela);
int      fyd_mouse_para_pixels_y(int32_t y_texto, int32_t altura_tela);

// ==================================================
// ⌨️ IDIOMA E TECLADO
// ==================================================
void                fyd_lang_definir(uint8_t lang_id);
const PacoteIdioma* fyd_lang_atual(void);
uint8_t             fyd_lang_id_atual(void);
uint8_t             fyd_kb_ler_tecla(void);
uint8_t             fyd_kb_layout_atual(void);
void                fyd_kb_layout_ptbr(void);
void                fyd_kb_layout_us(void);

// ==================================================
// 🔌 USB
// ==================================================
int fyd_usb_listar(UsbDispositivo* lista, int max_dispositivos);
int fyd_usb_ler_hid(uint8_t endereco, uint8_t endpoint, void* buffer, size_t tamanho);
int fyd_usb_escrever_hid(uint8_t endereco, uint8_t endpoint, const void* dados, size_t tamanho);

// ==================================================
// 👤 USUÁRIOS E PERMISSÕES
// ==================================================
// core/fyd_ponte.h — Usuários
//int         fyd_usuario_criar(const char* nome, const char* senha, int nivel_acesso);
int         fyd_usuario_apagar(const char* nome);
int         fyd_usuario_login(const char* nome, const char* senha);
int         fyd_usuario_logout(void);
const char* fyd_usuario_atual(void);          // ✅ Função — igual ao usuarios.h
int         fyd_usuario_nivel(void);
int         fyd_permissao_verificar(int nivel);


// ==================================================
// 🖥️ VIRTUALIZAÇÃO
// ==================================================
int fyd_vm_criar(const char* nome, uint64_t ram_bytes, int nucleos_cpu);
int fyd_vm_iniciar(int id_vm);
int fyd_vm_parar(int id_vm);
int fyd_vm_destruir(int id_vm);

#endif
