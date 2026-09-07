ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);
    rodata PT_LOAD FLAGS(4);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0xffffffff80000000;

    _kernel_start = .;

    .text : {
        *(.text*)
    } :text

    . = ALIGN(0x1000);

    .rodata : {
        *(.rodata*)

        . = ALIGN(8);
        PROVIDE(__start_ksymtab = .);
        KEEP(*(.ksymtab))
        PROVIDE(__stop_ksymtab = .);
    } :rodata

    .limine_requests : {
        KEEP(*(.limine_requests))
    } :rodata

    . = ALIGN(0x1000);

    .data : {
        *(.data*)
    } :data

    .bss : {
        *(COMMON)
        *(.bss*)
    } :data

    . = ALIGN(0x1000);
    _kernel_end = .;
}