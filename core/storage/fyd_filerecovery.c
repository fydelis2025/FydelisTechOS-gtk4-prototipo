#include "fyd_filerecovery.h"
#include <stdio.h>
#include <string.h>

int filerecovery_escanear_unidade(const char* letraUnidade, ArquivoPerdido* buffer, int max_arquivos) {
    (void)letraUnidade;
    if (max_arquivos < 4) return 0;

    const char* nomes[] = {
        "Projeto_Fydelis.fy",
        "Relatorio_Seguranca.docx",
        "Captura_Rede_001.png",
        "Backup_Sistema.img"
    };

    for (int i = 0; i < 4 && i < max_arquivos; i++) {
        snprintf(buffer[i].nomeOriginal, sizeof(buffer[i].nomeOriginal), "%s", nomes[i]);
        buffer[i].tamanho = (1 + i) * 256LL * 1024;
        buffer[i].clusterInicio = 512 + i * 128;
        buffer[i].probabilidade = 85 - (i * 10);
        buffer[i].recuperavel = buffer[i].probabilidade > 30;
    }

    return 4;
}

bool filerecovery_recuperar_arquivo(const ArquivoPerdido* arquivo, const char* destino) {
    char caminhoFinal[512];
    snprintf(caminhoFinal, sizeof(caminhoFinal), "%s/%s", destino, arquivo->nomeOriginal);
    FILE* f = fopen(caminhoFinal, "wb");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}
