# ==========================================
# FYDELISTECH OS — Interface Gráfica v4.5
# Ambiente: Usuário · GTK4 · ELF Nativo + KERNEL LEGACY
# Bootloader: BIOS MBR 512b → Imagem de Disco
# Salvador • Bahia 🇧🇷
# ==========================================

CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -O2 -D__USE_LIBC__ \
          -I. -Icore -Iusuarios -I../src/include -I../include \
          -Iboot -Iterminal -Iinclude \
          -I/usr/include/gtk-4.0 \
          -I/usr/include/pango-1.0 \
          -I/usr/include/harfbuzz \
          -I/usr/include/cairo \
          -I/usr/include/gdk-pixbuf-2.0 \
          -I/usr/include/glib-2.0 \
          -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
          -I/usr/include/graphene-1.0 \
          -I/usr/lib/x86_64-linux-gnu/graphene-1.0/include

CFLAGS_KERNEL = -std=c17 -Wall -Wextra -O2 -ffreestanding -nostdlib -m64 \
                -I. -Icore -Iboot -Iinclude

LDFLAGS = -lgtk-4 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 \
          -lcairo-gobject -lcairo -lvulkan -lgraphene-1.0 -lgio-2.0 \
          -lgobject-2.0 -lglib-2.0 -lcrypto

TARGET        = fyd_desktop
BIN_DIR       = fyd/bin
LINKER_SCRIPT = linker.ld

# === NOVO: Caminhos do sistema de boot Legacy ===
BOOTLOADER_ASM = boot/bootloader.asm
KERNEL_SRC = boot/kernel_main.c
LINKER_KERNEL  = boot/linker_kernel.ld

BOOTLOADER_BIN = $(BIN_DIR)/fyd_boot.bin
KERNEL_ELF     = $(BIN_DIR)/fyd_kernel.elf
DISK_IMG       = $(BIN_DIR)/fyd_disk.img

# ==========================================
# 🔹 FONTES — Aplicação de Usuário
# ==========================================
FONTES = \
	main.c \
	core/fyd_ponte.c \
	core/fyd_iniciar.c \
	core/fyd_tema.c \
	core/fyd_associar.c \
	core/fyd_registro.c \
	core/fyd_estilos.c \
	core/fyd_dat_protegido.c \
	core/fyd_arquivo_protegido.c \
	core/fyd_barra_sistema.c \
	core/fyd_menu_iniciar.c \
	core/fyd_area_trabalho.c \
	core/fyd_explorador.c \
	core/fyd_painel_controle.c \
	core/fyd_splash.c \
	core/fyd_login.c \
	core/fyd_icone.c \
	core/fyd_panic_excessao.c \
	core/fyd_fsmap.c \
	core/fyd_recursos.c \
	core/fyd_usuarios.c \
	core/fyd_sobre_dialogo.c \
	core/fyd_stubs.c \
	core/shell/launcher.c \
	core/shell/powershell/powershell.c \
	core/storage/fyd_diskclone.c \
	core/storage/fyd_disktools.c \
	core/storage/fyd_filerecovery.c \
	core/storage/fyd_mountmanager.c \
	core/storage/fyd_partitionmanager.c \
	core/storage/fyd_syscall_wrappers.c \
	core/bridge/fyd_bluetooth_bridge.c \
	core/bridge/fyd_rede_bridge.c \
	core/hardware/fyd_driver_instalador.c \
	core/hardware/fyd_hardware_manager.c \
	core/security/fyd_cve_panel.c \
	core/security/fyd_cve_check.c \
	core/security/fyd_guard.c \
	core/update/fyd_update_panel.c \
	terminal/boot_config.c

FONTES_ELF = \
        core/main_elf.c \
        core/fyd_ponte_elf.c \
        core/fyd_stubs.c

# ==========================================
# 🔹 ALVO PRINCIPAL
# ==========================================
all: setup core/fyd_recursos.c bootimg $(TARGET) instalar elf
	@echo "=========================================="
	@echo "  ✅ TUDO GERADO COM SUCESSO!"
	@echo "  📍 Imagem de Disco: $(DISK_IMG)"
	@echo "  📍 Bootloader:      $(BOOTLOADER_BIN)"
	@echo "  📍 Kernel:          $(KERNEL_ELF)"
	@echo "  📍 App Linux:       $(BIN_DIR)/$(TARGET)"
	@echo "  📍 App ELF:         $(BIN_DIR)/fyd_desktop_elf"
	@echo "  💡 Use 'make qemu' para rodar no QEMU"
	@echo "=========================================="

core/fyd_recursos.c: core/fyd_recursos.gresource.xml
	glib-compile-resources --generate-source \
		--c-name fyd_recursos \
		--sourcedir=assets/styles \
		--sourcedir=assets/icons \
		--sourcedir=assets/images \
		core/fyd_recursos.gresource.xml \
		--target=core/fyd_recursos.c

$(TARGET): $(FONTES)
	$(CC) $(CFLAGS) $(FONTES) -o $@ $(LDFLAGS)

instalar: $(TARGET)
	mkdir -p $(BIN_DIR)
	cp $(TARGET) $(BIN_DIR)/$(TARGET)
	@echo "✅ Binário GTK instalado em: $(BIN_DIR)/$(TARGET)"

elf: setup
	@echo "🔥 Gerando ELF Nativo..."
	mkdir -p $(BIN_DIR)
	$(CC) -ffreestanding -nostdlib -nostartfiles -m64 \
		-fno-stack-protector -fomit-frame-pointer \
		-O2 -g \
		-I. -Icore -Iboot -Iterminal -Iinclude -nostdlib \
		-Iusuarios -I../src/include -I../include \
		$(FONTES_ELF) \
		-T $(LINKER_SCRIPT) \
		-o $(BIN_DIR)/fyd_desktop_elf
	@echo "✅ ELF nativo gerado!"

# ==========================================
# 🔹 SISTEMA DE BOOT — LEGACY BIOS MBR
# ==========================================

# Compila o bootloader Assembly → binário cru
$(BOOTLOADER_BIN): $(BOOTLOADER_ASM)
	@echo "🔨 Montando Bootloader MBR..."
	nasm -f bin $(BOOTLOADER_ASM) -o $@
	@echo "✅ Bootloader pronto: $@"

# Compila o kernel C → ELF
$(KERNEL_ELF): $(KERNEL_SRC) $(LINKER_KERNEL)
	@echo "🔨 Compilando Kernel..."
	$(CC) $(CFLAGS_KERNEL) -c boot/kernel_main.c -o kernel_main.o
	ld -m elf_x86_64 -T boot/linker_kernel.ld kernel_main.o -o $@
	rm -f *.o
	@echo "✅ Kernel pronto: $@"




# Monta imagem de disco completa
bootimg: $(BOOTLOADER_BIN) $(KERNEL_ELF)
	@echo "💿 Montando Imagem de Disco..."
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=32768 status=none
	dd if=$(BOOTLOADER_BIN) of=$(DISK_IMG) bs=512 count=1 conv=notrunc status=none
	objcopy -O binary $(KERNEL_ELF) kernel_raw.bin
	dd if=kernel_raw.bin of=$(DISK_IMG) bs=512 seek=4096 count=2048 conv=notrunc status=none
	rm -f kernel_raw.bin
	@echo "✅ Imagem pronta: $(DISK_IMG)"

# ==========================================
# 🔹 QEMU — Roda a IMAGEM DE DISCO (sem PVH!)
# ==========================================
qemu: all
	@echo "=========================================="
	@echo "  🖥️  LIGANDO FYDELISTECH OS..."
	@echo "=========================================="
	qemu-system-x86_64 \
		-machine pc \
		-drive format=raw,file=$(DISK_IMG),if=ide,media=disk \
		-boot order=c \
		-m 2G \
		-serial stdio &
	@sleep 4   # ⏳ Tempo do boot no QEMU
	@echo "🪟 Abrindo Ambiente Gráfico..."
	./$(BIN_DIR)/fyd_desktop

setup:
	@echo "📁 Criando estrutura de pastas..."
	mkdir -p assets/images assets/icons assets/styles
	mkdir -p core/shell core/shell/powershell core/storage
	mkdir -p core/bridge core/hardware core/security core/update
	mkdir -p boot terminal include
	mkdir -p usuarios fyd/bin fyd/lib fyd/apps fyd/config
	mkdir -p fyd/menu fyd/registro fyd/associacoes fyd/temas
	mkdir -p fyd/drivers fyd/terminal fyd/lixeira
	mkdir -p secure/contas secure/sistema secure/cofre
	mkdir -p dados/geral dados/projetos dados/recuperacao

	@if [ ! -f fyd/config/fyd_registro.fyd ]; then \
		echo "📝 Criando registro padrão..."; \
		mkdir -p fyd/config; \
		echo '# ==========================================' > fyd/config/fyd_registro.fyd; \
		echo '# FYDELISTECH OS — REGISTRO DO SISTEMA v4.5' >> fyd/config/fyd_registro.fyd; \
		echo '# Atualizado: 2026-11-04 · Salvador 🇧🇷' >> fyd/config/fyd_registro.fyd; \
		echo '# ==========================================' >> fyd/config/fyd_registro.fyd; \
		echo '' >> fyd/config/fyd_registro.fyd; \
		echo '[Caminhos]' >> fyd/config/fyd_registro.fyd; \
		echo 'raiz_sistema         = /fyd' >> fyd/config/fyd_registro.fyd; \
		echo 'binarios             = /fyd/bin' >> fyd/config/fyd_registro.fyd; \
		echo 'bibliotecas          = /fyd/lib' >> fyd/config/fyd_registro.fyd; \
		echo 'aplicativos          = /fyd/apps' >> fyd/config/fyd_registro.fyd; \
		echo 'configuracoes        = /fyd/config' >> fyd/config/fyd_registro.fyd; \
		echo 'menu_dinamico        = /fyd/menu' >> fyd/config/fyd_registro.fyd; \
		echo 'registro_logs        = /fyd/registro' >> fyd/config/fyd_registro.fyd; \
		echo 'associacoes          = /fyd/associacoes' >> fyd/config/fyd_registro.fyd; \
		echo 'temas_cores          = /fyd/temas' >> fyd/config/fyd_registro.fyd; \
		echo 'drivers              = /fyd/drivers' >> fyd/config/fyd_registro.fyd; \
		echo 'terminal_shell       = /fyd/terminal' >> fyd/config/fyd_registro.fyd; \
		echo 'lixeira              = /fyd/lixeira' >> fyd/config/fyd_registro.fyd; \
		echo 'dados_protegidos     = /secure' >> fyd/config/fyd_registro.fyd; \
		echo 'perfis_usuarios      = /usuarios' >> fyd/config/fyd_registro.fyd; \
		echo '' >> fyd/config/fyd_registro.fyd; \
		echo '[Sistema]' >> fyd/config/fyd_registro.fyd; \
		echo 'versao               = 4.5' >> fyd/config/fyd_registro.fyd; \
		echo 'codigo_versao        = FydelisTechOS-2026-11-04' >> fyd/config/fyd_registro.fyd; \
		echo 'teclado              = pt-BR' >> fyd/config/fyd_registro.fyd; \
		echo 'fuso_horario         = America/Salvador' >> fyd/config/fyd_registro.fyd; \
		echo 'idioma               = pt-BR' >> fyd/config/fyd_registro.fyd; \
		echo 'auto_atualizar       = 1' >> fyd/config/fyd_registro.fyd; \
		echo 'verificar_cve        = 1' >> fyd/config/fyd_registro.fyd; \
		echo "✅ Registro criado!"; \
	else \
		echo "✅ Registro já existe, mantido!"; \
	fi
	@echo "✅ Estrutura do FydelisTechOS pronta!"

run: all
	./$(BIN_DIR)/$(TARGET)

clean:
	rm -f $(TARGET) $(BIN_DIR)/$(TARGET) $(BIN_DIR)/fyd_desktop_elf
	rm -f $(BIN_DIR)/fyd_kernel.bin $(BIN_DIR)/fyd_boot.bin
	rm -f $(BIN_DIR)/fyd_kernel.elf $(BIN_DIR)/fyd_disk.img
	rm -f core/fyd_recursos.c *.o kernel_raw.bin
	rm -f core/*.o core/shell/*.o core/shell/powershell/*.o core/storage/*.o
	rm -f core/bridge/*.o core/hardware/*.o core/security/*.o usuarios/*.o
	rm -f boot/*.o terminal/*.o
	@echo "🧹 Limpo com sucesso!"

rebuild: clean all
