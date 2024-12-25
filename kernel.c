#include <sys/io.h>

#define WHITE_TXT_ON_BLUE_BG 0x1F 
#define WHITE_TXT_ON_GREEN_BG 0x2F 
#define LINE_SIZE 80*2
#define LINE_COUNT 25
#define VIDEO_MEMORY 0xb8000
#define KEYBOARD_PORT 0x60
#define CURSOR_COMMAND_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define SPEAKER_PORT 0x61

void k_clear_screen(int color);
void k_printf(const char *message, unsigned int line, int text_color);
void k_get_text(char *buffer, int pos, unsigned int max_length);
void k_update_cursor(int row, int col);

void k_main() 
{
    char password[4];

    k_clear_screen(WHITE_TXT_ON_BLUE_BG);
    k_printf("GIVE ME YOUR DENGI", 0, WHITE_TXT_ON_BLUE_BG);
    k_printf("OUR SCHET IS 0000 0000 0000 0000", 1, WHITE_TXT_ON_BLUE_BG);
    k_printf("ENTER PASSWORD:", 3, WHITE_TXT_ON_BLUE_BG);
    k_update_cursor(3, 15);

    outb(inb(SPEAKER_PORT) | 0x03, SPEAKER_PORT); 

    int pos = 3 * LINE_SIZE + 15 * 2;
    k_get_text(password, pos, sizeof(password));

    outb(inb(SPEAKER_PORT) & 0xFC, SPEAKER_PORT);
    k_clear_screen(WHITE_TXT_ON_GREEN_BG);
    k_printf("THANK YOU!", 0, WHITE_TXT_ON_GREEN_BG);
    k_update_cursor(0, 9);

    while (1) {}
}

void k_clear_screen(int color)
{
    char *vidmem = (char *)VIDEO_MEMORY;
    for (unsigned int i = 0; i < (LINE_SIZE * LINE_COUNT); i += 2) {
        vidmem[i] = ' ';               
        vidmem[i + 1] = color;
    }
}

void k_printf(const char *message, unsigned int line, int text_color)
{
    char *vidmem = (char *)VIDEO_MEMORY;
    unsigned int i = line * LINE_SIZE;

    while (*message) {
        if (*message == '\n') {
            line++;
            i = line * LINE_SIZE;
        } else {
            vidmem[i++] = *message;
            vidmem[i++] = text_color;
        }
        message++;
    }
}

void k_update_cursor(int row, int col)
{
    unsigned short position = (row * 80) + col;

    outb(14, CURSOR_COMMAND_PORT);
    outb((unsigned char)(position >> 8), CURSOR_DATA_PORT);
    outb(15, CURSOR_COMMAND_PORT);
    outb((unsigned char)position, CURSOR_DATA_PORT);
}

void k_get_text(char *buffer, int pos, unsigned int max_length)
{
    unsigned int count = 0;
    char *vidmem = (char *)VIDEO_MEMORY;

    while (1) {
        unsigned char scancode;

        do {
            scancode = inb(KEYBOARD_PORT);
        } while (scancode & 0x80);

        char key = 0;
        switch (scancode) {
            case 0x02: key = '1'; break;
            case 0x03: key = '2'; break;
            case 0x04: key = '3'; break;
            case 0x05: key = '4'; break;
            case 0x06: key = '5'; break;
            case 0x07: key = '6'; break;
            case 0x08: key = '7'; break;
            case 0x09: key = '8'; break;
            case 0x0A: key = '9'; break;
            case 0x0B: key = '0'; break;
            case 0x1C: // Enter key
                buffer[count] = '\0';
                return;
            case 0x0E: // Backspace
                if (count > 0) {
                    count--;
                    pos -= 2;
                    vidmem[pos] = ' ';
                }
                continue;
            default:
                continue; 
        }

        if (count < max_length) {
            buffer[count++] = key;  
            vidmem[pos] = key;    
            vidmem[pos + 1] = WHITE_TXT_ON_BLUE_BG;
            pos += 2;         
            k_update_cursor(0, pos / 2);
        }

        do {
            scancode = inb(KEYBOARD_PORT);
        } while (!(scancode & 0x80));
    }
}
