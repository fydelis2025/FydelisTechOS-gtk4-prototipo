// ==========================================
// PONTE ELF — Sem GTK · Sem libc
// Para o binário nativo do kernel
// ==========================================
#ifndef FYD_PONTE_ELF_H
#define FYD_PONTE_ELF_H

#include <stddef.h>
#include <stdint.h>

// Chamadas de sistema do FydelisTech OS
// Mesmos números que no kernel Assembly
#define FYD_SYS_EXIT     1
#define FYD_SYS_WRITE    2
#define FYD_SYS_READ     3
#define FYD_SYS_GETTIME  4

// Protótipos da ponte
int fyd_iniciar(void);
void fyd_terminar(void);

#endif // FYD_PONTE_ELF_H

