#include <gtk/gtk.h>
#include <capstone/capstone.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* --- Dados Globais --- */
static csh capstone_h;
static gboolean capstone_ok = FALSE;
static GBytes *dados_binario = NULL;
static gsize tamanho_binario = 0;

/* --- Widgets --- */
GtkWidget *janela;
GtkWidget *view_hex, *view_asm;
GtkTextBuffer *buf_hex, *buf_asm;
GtkWidget *btn_carregar, *btn_limpar;
GtkWidget *combo_arquitetura;

/* --- Inicializar Capstone --- */
static gboolean iniciar_capstone(int arq_idx) {
    cs_close(&capstone_h);
    capstone_ok = FALSE;
    
    cs_arch arch = CS_ARCH_X86;
    cs_mode mode = (arq_idx == 0) ? CS_MODE_32 : CS_MODE_64;
    
    if (cs_open(arch, mode, &capstone_h) != CS_ERR_OK) {
        g_printerr("Erro ao inicializar Capstone\n");
        return FALSE;
    }
    cs_option(capstone_h, CS_OPT_DETAIL, CS_OPT_ON);
    capstone_ok = TRUE;
    return TRUE;
}

/* --- Formatar Linha Hex --- */
static void formatar_hex(const guint8 *dados, gsize tam, gsize deslocamento, GString *saida) {
    gchar ascii[17] = {0};
    gsize i;
    
    g_string_append_printf(saida, "%08zx  ", deslocamento);
    
    for (i = 0; i < 16; i++) {
        if (i < tam) {
            g_string_append_printf(saida, "%02x ", dados[i]);
            ascii[i] = g_ascii_isprint(dados[i]) ? dados[i] : '.';
        } else {
            g_string_append(saida, "   ");
        }
        if (i == 7) g_string_append(saida, " ");
    }
    g_string_append_printf(saida, " |%s|\n", ascii);
}

/* --- Desassemblar e Preencher --- */
static void renderizar_conteudo(void) {
    if (!dados_binario || !capstone_ok) return;
    
    const guint8 *dados = g_bytes_get_data(dados_binario, NULL);
    gsize pos = 0;
    uint64_t endereco = 0x100000; /* Endereço base padrão */
    
    GString *saida_hex = g_string_new("");
    GString *saida_asm = g_string_new("");
    
    while (pos < tamanho_binario) {
        /* Bloco de 16 bytes para visão hex */
        gsize bloco_tam = (tamanho_binario - pos) >= 16 ? 16 : (tamanho_binario - pos);
        formatar_hex(&dados[pos], bloco_tam, pos, saida_hex);
        
        /* Desassemblar instruções */
        cs_insn *instrucoes;
        size_t qtd = cs_disasm(capstone_h, &dados[pos], bloco_tam, endereco, 0, &instrucoes);
        
        if (qtd > 0) {
            for (size_t k = 0; k < qtd; k++) {
                g_string_append_printf(saida_asm,
                    "0x%lx  %-18s %s %s\n",
                    instrucoes[k].address,
                    instrucoes[k].bytes, /* placeholder → substituído abaixo */
                    instrucoes[k].mnemonic,
                    instrucoes[k].op_str);
            }
            cs_free(instrucoes, qtd);
        } else {
            g_string_append_printf(saida_asm,
                    "0x%lx  %02x                 ; desconhecido\n",
                    endereco, dados[pos]);
        }
        
        pos += bloco_tam;
        endereco += bloco_tam;
    }
    
    gtk_text_buffer_set_text(buf_hex, saida_hex->str, -1);
    gtk_text_buffer_set_text(buf_asm, saida_asm->str, -1);
    
    g_string_free(saida_hex, TRUE);
    g_string_free(saida_asm, TRUE);
}

/* --- Carregar Arquivo --- */
static void ao_carregar(GtkWidget *w, gpointer d) {
    GtkFileChooserNative *dlg = gtk_file_chooser_native_new(
        "Abrir binário — Fydelis Disassembler",
        GTK_WINDOW(janela),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Abrir", "_Cancelar");
    
    if (gtk_native_dialog_run(GTK_NATIVE_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        GFile *arquivo = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dlg));
        GError *erro = NULL;
        
        if (dados_binario) g_bytes_unref(dados_binario);
        dados_binario = g_file_load_bytes(arquivo, NULL, &erro);
        
        if (dados_binario) {
            tamanho_binario = g_bytes_get_size(dados_binario);
            renderizar_conteudo();
        } else {
            g_warning("Erro ao ler: %s", erro->message);
            g_error_free(erro);
        }
        
        g_object_unref(arquivo);
    }
    g_object_unref(dlg);
}

/* --- Limpar Tela --- */
static void ao_limpar(GtkWidget *w, gpointer d) {
    if (dados_binario) g_bytes_unref(dados_binario);
    dados_binario = NULL;
    tamanho_binario = 0;
    gtk_text_buffer_set_text(buf_hex, "", -1);
    gtk_text_buffer_set_text(buf_asm, "", -1);
}

/* --- Trocar Arquitetura --- */
static void ao_trocar_arq(GtkWidget *w, gpointer d) {
    iniciar_capstone(gtk_drop_down_get_selected(GTK_DROP_DOWN(w)));
    if (dados_binario) renderizar_conteudo();
}

/* --- CSS Estilo Fydelis --- */
static void aplicar_estilo(void) {
    const gchar *css =
        "@define-color fydelis-fundo      #0f1720;\n"
        "@define-color fydelis-painel     #1a2838;\n"
        "@define-color fydelis-destaque  #00d4ff;\n"
        "@define-color fydelis-texto      #e6f0f8;\n"
        "@define-color fydelis-botao      #0099cc;\n"
        "window { background-color: @fydelis-fundo; color: @fydelis-texto; }\n"
        "textview { background-color: @fydelis-painel; color: @fydelis-texto; font-family: 'JetBrains Mono', Consolas, monospace; font-size: 10pt; }\n"
        "button { background-color: @fydelis-botao; color: white; border-radius: 4px; padding: 6px 12px; }\n"
        "button:hover { background-color: @fydelis-destaque; }\n"
        "paned { background-color: @fydelis-fundo; }\n"
        "label { color: @fydelis-destaque; font-weight: bold; }\n";
    
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_string(prov, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(prov);
}

/* --- Construção da Interface --- */
static void ativar(GtkApplication *app, gpointer d) {
    janela = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(janela), "Fydelis Disassembler");
    gtk_window_set_default_size(GTK_WINDOW(janela), 1000, 600);
    
    aplicar_estilo();
    iniciar_capstone(1); /* Padrão: x86_64 */
    
    /* Layout Principal */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_window_set_child(GTK_WINDOW(janela), vbox);
    
    /* Barra Superior */
    GtkWidget *barra = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vbox), barra);
    
    btn_carregar = gtk_button_new_with_label("📂 Carregar Arquivo");
    g_signal_connect(btn_carregar, "clicked", G_CALLBACK(ao_carregar), NULL);
    gtk_box_append(GTK_BOX(barra), btn_carregar);
    
    btn_limpar = gtk_button_new_with_label("🧹 Limpar");
    g_signal_connect(btn_limpar, "clicked", G_CALLBACK(ao_limpar), NULL);
    gtk_box_append(GTK_BOX(barra), btn_limpar);
    
    const char *arq_opcoes[] = {"x86-32", "x86-64", NULL};
    combo_arquitetura = gtk_drop_down_new_from_strings(arq_opcoes);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_arquitetura), 1);
    g_signal_connect(combo_arquitetura, "notify::selected", G_CALLBACK(ao_trocar_arq), NULL);
    gtk_box_append(GTK_BOX(barra), gtk_label_new("Arquitetura:"));
    gtk_box_append(GTK_BOX(barra), combo_arquitetura);
    
    /* Painéis Divididos */
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_append(GTK_BOX(vbox), paned);
    
    /* Painel Hex */
    GtkWidget *scr_hex = gtk_scrolled_window_new();
    GtkWidget *lbl_hex = gtk_label_new("📋 Visualização Hexadecimal");
    buf_hex = gtk_text_buffer_new(NULL);
    view_hex = gtk_text_view_new_with_buffer(buf_hex);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view_hex), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr_hex), view_hex);
    
    GtkWidget *bloco_hex = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(bloco_hex), lbl_hex);
    gtk_box_append(GTK_BOX(bloco_hex), scr_hex);
    gtk_paned_set_start_child(GTK_PANED(paned), bloco_hex);
    gtk_paned_set_position(GTK_PANED(paned), 280);
    
    /* Painel Assembly */
    GtkWidget *scr_asm = gtk_scrolled_window_new();
    GtkWidget *lbl_asm = gtk_label_new("⚙️ Código Assembly Desassemblado");
    buf_asm = gtk_text_buffer_new(NULL);
    view_asm = gtk_text_view_new_with_buffer(buf_asm);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view_asm), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr_asm), view_asm);
    
    GtkWidget *bloco_asm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(bloco_asm), lbl_asm);
    gtk_box_append(GTK_BOX(bloco_asm), scr_asm);
    gtk_paned_set_end_child(GTK_PANED(paned), bloco_asm);
    
    gtk_window_present(GTK_WINDOW(janela));
}

/* --- Ponto de Entrada --- */
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.fydelis.disasm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(ativar), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    if (capstone_ok) cs_close(&capstone_h);
    if (dados_binario) g_bytes_unref(dados_binario);
    g_object_unref(app);
    
    return status;
}