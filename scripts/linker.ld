OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_main)

SECTIONS
{
    . = 1M;

    .text ALIGN(4K) : AT(ADDR(.text))
    {
        _text_start = .;
        *(.text)
        *(.text.*)
        _text_end = .;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata))
    {
        _rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        _rodata_end = .;
    }

    .data ALIGN(4K) : AT(ADDR(.data))
    {
        _data_start = .;
        *(.data)
        *(.data.*)
        _data_end = .;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss))
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        _bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
