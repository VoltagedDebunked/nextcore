#include "../kernel/types.h"
#include "io.h"

void putchar(char c);

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Pointer to the VGA text buffer
volatile char* vga_buffer = (volatile char*)VGA_MEMORY;

// Function to write a character to the screen
void putchar(char c) {
    static int cursor_x = 0; // Current cursor position (x)
    static int cursor_y = 0; // Current cursor position (y)

    // Handle newline
    if (c == '\n') {
        cursor_x = 0; // Move to the beginning of the next line
        cursor_y++;
    } else {
        // Write the character to the VGA buffer
        vga_buffer[(cursor_y * VGA_WIDTH + cursor_x) * 2] = c; // Character
        vga_buffer[(cursor_y * VGA_WIDTH + cursor_x) * 2 + 1] = 0x07; // Attribute (white on black)
        cursor_x++;
    }

    // Handle cursor wrapping
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0; // Move to the next line
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        cursor_y = 0; // Reset to the top of the screen
    }
}

// Define keyboard ports
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

// Function to handle keyboard interrupts
void keyboard_handler() {
    uint8_t scancode = inb(KBD_DATA_PORT);

    // Simple scancode to ASCII mapping (for demonstration purposes)
    static const char sc_ascii[] = {
        0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
        '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
        'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
        'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '
    };

    static const char sc_ascii_shift[] = {
        0,    27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',  '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
        'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|',  'Z',
        'X',  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' '
    };

    static int shift = 0;
    static int caps_lock = 0;

    if (scancode & 0x80) { // Key release
        if (scancode == 0x2A || scancode == 0x36) {
            shift = 0; // Left or Right Shift released
        }
    } else { // Key press
        if (scancode == 0x2A || scancode == 0x36) {
            shift = 1; // Left or Right Shift pressed
        } else if (scancode == 0x3A) {
            caps_lock = !caps_lock; // Toggle Caps Lock
        } else {
            char c;
            if (shift) {
                c = sc_ascii_shift[scancode];
            } else {
                c = sc_ascii[scancode];
            }

            if (caps_lock && c >= 'a' && c <= 'z') {
                c -= 32;  // Convert to uppercase
            } else if (caps_lock && c >= 'A' && c <= 'Z') {
                c += 32;  // Convert to lowercase
            }

            if (c) {
                putchar(c); // Display the character on the screen
            }
        }
    }
}

// Initialize the keyboard
void keyboard_init() {
    outb(KBD_STATUS_PORT, 0xAE); // Enable keyboard interrupts
}
