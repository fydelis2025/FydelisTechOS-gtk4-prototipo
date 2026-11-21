#ifndef FSMAP_H
#define FSMAP_H

// ==========================================
// FYDELIS FILESYSTEM MAPPER (C Puro)
// Apps acham que estão no Windows → aponta para SEU SO
// De Salvador • Bahia 🇧🇷
// ==========================================

#ifdef __cplusplus
extern "C" {
#endif

// ✅ Caminhos que instaladores procuram → mapeados para o sistema
#define CAMINHO_WINDOWS          "/Windows/"
#define CAMINHO_SYSTEM32         "/Windows/System32/"
#define CAMINHO_SYSTEM64         "/Windows/System64/"
#define CAMINHO_PROGRAMFILES     "/Program Files/"
#define CAMINHO_PROGRAMFILES_X86 "/Program Files (x86)/"
#define CAMINHO_USERS            "/Users/"
#define CAMINHO_PROGRAMDATA      "/ProgramData/"

// ✅ Pastas do USUÁRIO atual
#define CAMINHO_DESKTOP          "/Users/current/Desktop/"
#define CAMINHO_DOCUMENTS        "/Users/current/Documents/"
#define CAMINHO_DOWNLOADS        "/Users/current/Downloads/"
#define CAMINHO_APPDATA          "/Users/current/AppData/"
#define CAMINHO_POWERSHELL_USER  "/Users/current/PowerShell/"

// ✅ Pastas NOSSAS — FydelisTech (prioridade máxima!)
#define CAMINHO_FYDELIS_APPS     "/Program Files/FydelisTech/"
#define CAMINHO_FYDELIS_WEB      CAMINHO_FYDELIS_APPS "FydelisWeb/"
#define CAMINHO_FYDELIS_EDITOR   CAMINHO_FYDELIS_APPS "Fydelis++/"
#define CAMINHO_FYDELIS_REMOTE   CAMINHO_FYDELIS_APPS "FydelisRemote/"

// ✅ FYDELISTECH SUITE — Aplicativos instalados
#define CAMINHO_FYDELIS_SUITE    "/Program Files/FydelisTech-Suite/"

#define CAMINHO_FYDELIS_PAINT    CAMINHO_FYDELIS_SUITE "Fydelis-Paint.exe"
#define CAMINHO_FYDELIS_REMOTE_E CAMINHO_FYDELIS_SUITE "FydelisRemote.exe"
#define CAMINHO_FYDELIS_EDIT     CAMINHO_FYDELIS_SUITE "FydelisTech++.exe"
#define CAMINHO_FYDELIS_WEB_APP  CAMINHO_FYDELIS_SUITE "FydelisTech-Web/FydelisTech-Web.exe"
#define CAMINHO_FYDELIS_DISCO    CAMINHO_FYDELIS_SUITE "FydelisTech-DiscoTools/FydelisTech-DiscoTools.exe"

// ✅ Traduz caminho formato Windows → formato nativo do SEU SO
// Exemplo: "C:\\Windows\\System32" → "/Windows/System32/"
const char* fs_map_windows_to_native(const char* caminhoWindows);

// ✅ Define qual usuário está logado → resolve "/Users/current/"
void fs_definirUsuarioAtual(const char* nomeUsuario);

#ifdef __cplusplus
}
#endif

#endif // FSMAP_H
