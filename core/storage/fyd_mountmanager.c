#include "fyd_mountmanager.h"
#include <string.h>
#include <stdio.h>

#define MAX_UNIDADES 32
static UnidadeMontada g_unidades[MAX_UNIDADES];
static int g_qtd_unidades = 0;

void mountmanager_inicializar_sistema(void) {
    UnidadeMontada c;
    snprintf(c.letra, sizeof(c.letra), "C:");
    snprintf(c.caminhoFisico, sizeof(c.caminhoFisico), "\\\\.\\PhysicalDrive0#1");
    snprintf(c.sistemaArquivos, sizeof(c.sistemaArquivos), "exFAT");
    c.tamanhoTotal = 500ULL * 1024 * 1024 * 1024;
    c.espacoLivre = 450ULL * 1024 * 1024 * 1024;
    c.eRemovivel = false;
    c.eSistema = true;

    g_unidades[0] = c;
    g_qtd_unidades = 1;
}

bool mountmanager_montar_unidade(const char* disco, int particao, const char* sistemaArq, uint64_t tamanho, char* letra_out, int max_len) {
    if (strcmp(disco, "\\\\.\\PhysicalDrive0") == 0 && particao == 1) {
        snprintf(letra_out, max_len, "C:");
        return true;
    }

    char proxima_letra = 'D';
    for (char l = 'D'; l <= 'Z'; l++) {
        bool encontrada = false;
        for (int i = 0; i < g_qtd_unidades; i++) {
            char tmp[8];
            snprintf(tmp, sizeof(tmp), "%c:", l);
            if (strcmp(g_unidades[i].letra, tmp) == 0) {
                encontrada = true;
                break;
            }
        }
        if (!encontrada) {
            proxima_letra = l;
            break;
        }
    }

    if (g_qtd_unidades >= MAX_UNIDADES) return false;

    UnidadeMontada u;
    snprintf(u.letra, sizeof(u.letra), "%c:", proxima_letra);
    snprintf(u.caminhoFisico, sizeof(u.caminhoFisico), "%s#%d", disco, particao);
    snprintf(u.sistemaArquivos, sizeof(u.sistemaArquivos), "%s", sistemaArq);
    u.tamanhoTotal = tamanho;
    u.espacoLivre = tamanho;
    u.eRemovivel = (strstr(disco, "2") != NULL || strstr(disco, "USB") != NULL);
    u.eSistema = false;

    g_unidades[g_qtd_unidades++] = u;
    snprintf(letra_out, max_len, "%s", u.letra);
    return true;
}

bool mountmanager_desmontar_unidade(const char* letra) {
    for (int i = 0; i < g_qtd_unidades; i++) {
        if (strcmp(g_unidades[i].letra, letra) == 0) {
            if (g_unidades[i].eSistema) return false;
            for (int j = i; j < g_qtd_unidades - 1; j++) {
                g_unidades[j] = g_unidades[j + 1];
            }
            g_qtd_unidades--;
            return true;
        }
    }
    return false;
}

int mountmanager_listar_unidades(UnidadeMontada* buffer, int max_unidades) {
    int count = (g_qtd_unidades < max_unidades) ? g_qtd_unidades : max_unidades;
    for (int i = 0; i < count; i++) {
        buffer[i] = g_unidades[i];
    }
    return count;
}
