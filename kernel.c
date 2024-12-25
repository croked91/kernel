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

void paint_screen(int color);
void custom_printf(const char *message, unsigned int line, int text_color);
void listen_keys(char *buffer, int pos, unsigned int max_length);
void move_cursor(int row, int col);

void k_main() 
{
    char password[4];

    paint_screen(WHITE_TXT_ON_BLUE_BG);
    custom_printf("GIVE ME YOUR DENGI", 0, WHITE_TXT_ON_BLUE_BG);
    custom_printf("OUR SCHET IS 0000 0000 0000 0000", 1, WHITE_TXT_ON_BLUE_BG);
    custom_printf("ENTER PASSWORD:", 3, WHITE_TXT_ON_BLUE_BG);
    move_cursor(3, 15);

    outb(inb(SPEAKER_PORT) | 0x03, SPEAKER_PORT); 

    int pos = 3 * LINE_SIZE + 15 * 2;
    listen_keys(password, pos, sizeof(password));

    outb(inb(SPEAKER_PORT) & 0xFC, SPEAKER_PORT);
    paint_screen(WHITE_TXT_ON_GREEN_BG);
    custom_printf("THANK YOU!", 0, WHITE_TXT_ON_GREEN_BG);
    move_cursor(0, 9);

    while (1) {}
}

void move_cursor(int row, int col)
{
    unsigned short position = (row * 80) + col;

    outb(14, CURSOR_COMMAND_PORT);
    outb((unsigned char)(position >> 8), CURSOR_DATA_PORT);
    outb(15, CURSOR_COMMAND_PORT);
    outb((unsigned char)position, CURSOR_DATA_PORT);
}

void custom_printf(const char *message, unsigned int line, int text_color)
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

void paint_screen(int color)
{
    char *vidmem = (char *)VIDEO_MEMORY;
    for (unsigned int i = 0; i < (LINE_SIZE * LINE_COUNT); i += 2) {
        vidmem[i] = ' ';               
        vidmem[i + 1] = color;
    }
}

void listen_keys(char *buffer, int pos, unsigned int max_length)
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
            case 0x1C:
                buffer[count] = '\0';
                return;
            case 0x0E:
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
            move_cursor(0, pos / 2);
        }

        do {
            scancode = inb(KEYBOARD_PORT);
        } while (!(scancode & 0x80));
    }
}
