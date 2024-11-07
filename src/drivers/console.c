#define VGA_MEMORY 0xB8000
static int console_row = 0;
static int console_col = 0;

void put_char(char c) {
    unsigned char* vga = (unsigned char*)VGA_MEMORY;
    int index = (console_row * 80 + console_col) * 2;

    vga[index] = c;
    vga[index + 1] = 0x07; // Light grey on black

    console_col++;
    if (console_col >= 80) {
        console_col = 0;
        console_row++;
    }
    if (console_row >= 25) {
        console_row = 0;
    }
}

void put_hex(unsigned long n) {
    int i;
    char hex_chars[] = "0123456789ABCDEF";

    put_char('0');
    put_char('x');

    for (i = 60; i >= 0; i -= 4) {
        put_char(hex_chars[(n >> i) & 0xF]);
    }
}
