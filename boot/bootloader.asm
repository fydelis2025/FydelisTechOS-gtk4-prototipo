; ==========================================
; FydelisTech - Bootloader Legacy BIOS (MBR, 512 bytes)
; Localização: boot/bootloader.asm
; ==========================================
BITS 16
ORG 0x7C00

KERNEL_LBA      equ 4096        ; kernel no LBA 4096 (2 MiB)
KERNEL_SECTORS  equ 1         ; Aumentado para garantir espaço para o C + Assembly
KERNEL_ADDR     equ 0x8000

start16:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl        ; ✅ salva a unidade REAL que o BIOS entregou

    ; 1. VGA Texto 80x25
    mov ax, 0x0003
    int 0x10

    ; 2. Lê o Kernel via LBA Estendido (AH=0x42)
    sub sp, 16
    mov bp, sp
    mov word [bp],    0x0010    ; tamanho do pacote + reservado
    mov word [bp+2],  KERNEL_SECTORS
    mov word [bp+4],  KERNEL_ADDR
    mov word [bp+6],  0x0000
    mov dword [bp+8], KERNEL_LBA
    mov dword [bp+12], 0

    mov si, bp
    mov dl, [boot_drive]        ; ✅ usa a unidade real (pendrive/HD/USB)
    mov ah, 0x42
    int 0x13
    add sp, 16
    jc disk_error

    ; 3. A20
    in  al, 0x92
    or  al, 2
    out 0x92, al

    ; 4. Modo Protegido
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:start32

disk_error:
    mov ax, 0xb800
    mov es, ax
    mov byte [es:0], 'E'
    mov byte [es:1], 0x0A
    cli
    hlt

BITS 32
start32:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 5. Paginação — Zera e configura as tabelas de forma segura
    mov edi, 0x1000
    xor eax, eax
    mov ecx, 4096 / 4             ; Limpa 4 KB para a PML4 (0x1000)
    rep stosd

    mov edi, 0x2000
    mov ecx, 4096 / 4             ; Limpa 4 KB para o PDPT (0x2000)
    rep stosd

    mov edi, 0x3000
    mov ecx, 4096 / 4             ; Limpa 4 KB para o PD (0x3000)
    rep stosd

    ; Configura o mapeamento (Identity Map dos primeiros 4MB)
    mov dword [0x1000], 0x2003          ; PML4[0] -> aponta para PDPT (0x2000)
    mov dword [0x2000], 0x3003          ; PDPT[0] -> aponta para PD (0x3000)
    
    ; 2 huge pages de 2MB cada (totalizando 4MB mapeados)
    mov dword [0x3000], 0x00000083      ; PD[0] -> 0-2MB (Presente + Escrita + Page Size 2MB)
    mov dword [0x3008], 0x00200083      ; PD[1] -> 2-4MB (Correto: espaçamento de 8 bytes)
    
    ; 🔴 CRUCIAL: Carrega o CR3 apontando para a PML4 (0x1000) AGORA, com tudo pronto
    mov eax, 0x1000
    mov cr3, eax
    
    ; 6. PAE
    mov eax, cr4
    or  eax, 1 << 5
    mov cr4, eax

    ; 7. Long Mode no EFER
    mov ecx, 0xC0000080
    rdmsr
    or  eax, 1 << 8
    wrmsr

    ; 8. Paginação ON
    mov eax, cr0
    or  eax, 1 << 31
    mov cr0, eax

    ; 9. GDT 64-bit e salto para o kernel
    lgdt [gdt64_descriptor]
    jmp 0x18:0x8000

; --- GDT ---
align 8
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF       ; 0x08: código 32-bit
    dq 0x00CF92000000FFFF       ; 0x10: dados 32-bit
    dq 0x00AF9A0000000000       ; 0x18: código 64-bit
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt64_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start                ; ✅ base de 8 bytes (mais seguro)

boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55
