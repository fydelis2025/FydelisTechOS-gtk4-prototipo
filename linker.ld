ENTRY(_start)
SECTIONS {
    . = 0x700000000000;   /* região do usuário — PML4[0x70] */
    .text ALIGN(4096) : { *(.text) *(.text.*) }
    .rodata ALIGN(4096) : { *(.rodata) *(.rodata.*) }
    .data ALIGN(4096) : { *(.data) *(.data.*) }
    .bss ALIGN(4096) : { *(COMMON) *(.bss) *(.bss.*) }
}
