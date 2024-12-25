#define WHITE_TXT_ON_BLUE_BG 0x1F 
#define WHITE_TXT_ON_GREEN_BG 0x2F 
#define LINE_SIZE 160 // 80 символов на строку по 2 байта
#define LINE_COUNT 25

void k_clear_screen(int color);
unsigned int k_printf(char *message, unsigned int line, int text_color);
void k_get_text(char *buffer, int pos, unsigned int max_length);
void k_update_cursor(int row, int col);
static inline void k_outb(unsigned short port, unsigned char value);
static inline unsigned char k_inb(unsigned short port);

void k_main() 
{
    char password[4];
		char rate[2];

    k_clear_screen(WHITE_TXT_ON_BLUE_BG);
    k_printf("Hello! It's your bank security service.", 0, WHITE_TXT_ON_BLUE_BG);
    k_printf("Send money to our secret deposit and paste the any password.", 1, WHITE_TXT_ON_BLUE_BG);
    k_printf("Enter password:", 3, WHITE_TXT_ON_BLUE_BG);
		k_update_cursor(3, 15);

    k_outb(0x61, k_inb(0x61) | 0x03); 

		int pos = 3 * LINE_SIZE + 15 * 2;
    k_get_text(password, pos, 4);

 		k_outb(0x61, k_inb(0x61) & 0xFC);
		k_clear_screen(WHITE_TXT_ON_GREEN_BG);
    k_printf("Password received!", 0, WHITE_TXT_ON_GREEN_BG);
		k_printf("Thank you for your trust!", 1, WHITE_TXT_ON_GREEN_BG);
		k_printf("Please rate our scam service on www.its-really-truth.net", 2, WHITE_TXT_ON_GREEN_BG);
		k_update_cursor(3, 0);

    while (1) {}
};

static inline unsigned char k_inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
};

static inline void k_outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "d"(port));
};

void k_clear_screen(int color)
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i = 0;
    while (i < (LINE_SIZE * LINE_COUNT))
    {
        vidmem[i] = ' ';              
        i++;
        vidmem[i] = color;
        i++;
    };
};

unsigned int k_printf(char *message, unsigned int line, int text_color)
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i = 0;

    i = (line * LINE_SIZE);

    while (*message != 0)
    {
        if (*message == '\n') 
        {
            line++;
            i = (line * LINE_SIZE);
            message++;
        }
        else
        {
            vidmem[i] = *message;              
            message++;
            i++;
            vidmem[i] = text_color;
            i++;
        };
    };

    return 1;
};

void k_update_cursor(int row, int col)
{
    unsigned short position=(row * 80) + col;

    k_outb(0x3D4, 14);
    k_outb(0x3D5, (unsigned char)(position>>8));
    k_outb(0x3D4, 15);
    k_outb(0x3D5, (unsigned char)(position));
};

void k_get_text(char *buffer, int pos, unsigned int max_length)
{
    unsigned int count = 0;
    char *vidmem = (char *) 0xb8000;

    while (1) {
        unsigned char scancode;

        do {
            scancode = k_inb(0x60);
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
            buffer[count] = key;  
            vidmem[pos] = key;    
            vidmem[pos + 1] = WHITE_TXT_ON_BLUE_BG;
            count++;
            pos += 2;         
						k_update_cursor(0, pos/2);
        }

        do {
            scancode = k_inb(0x60);
        } while (!(scancode & 0x80));
    }
};
