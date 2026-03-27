#include "key_event.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int peek_character = -1;

void ke_init()
{
    tcgetattr(STDIN_FILENO, &initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0; 
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}
 
void ke_close(){tcsetattr(STDIN_FILENO, TCSANOW, &initial_settings);}
 
int ke_kbhit()
{
    unsigned char ch;
    int nread;
 
    if (peek_character != -1) return 1;
    
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    
    nread = read(STDIN_FILENO, &ch, 1);
    
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
 
    if (nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}
 
int ke_readch()
{
    unsigned char ch;
 
    if (peek_character != -1)
    {
        ch = (unsigned char)peek_character;
        peek_character = -1;
        return ch;
    }
    
    read(STDIN_FILENO, &ch, 1);
    return (int)ch;
}
/// The testing funtion
/* 
int main()
{
    ke_init();
    printf("Press any key (ESC to exit)...\n");
    
    while (1 == 1)
    {
        if (ke_kbhit()) {
            int ch = ke_readch();
            printf("Key pressed: 0x%02X (%c)\n", ch, (ch >= 32 && ch <= 126) ? ch : '.');
            if (ch == 27) break;
        }
        usleep(10000);
    }
    
    ke_close();
    return 0;
}
*/
