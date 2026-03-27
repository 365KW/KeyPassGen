#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "key_event.h"

void _setup();
// Unused void _option_choose();
void _main();

void _setup()
{
  printf("Keyborad Password Generator");
  _main();
}

void _main()
{
    const int key_size = 250;
    int total_keys = 0;
    int *pressed_keys = (int *)malloc(key_size * sizeof(int));
    ke_init();
    printf("Press any key (ESC to exit)...\n");
    
    while (total_keys < key_size)
    {
        if (ke_kbhit()) {
            int ch = ke_readch();
            printf("Key pressed: 0x%02X (%c)\n", ch, (ch >= 32 && ch <= 126) ? ch : '.');
	    pressed_keys[total_keys] = ch,
	      total_keys++;
            if (ch == 27) break;
        }
        usleep(10000);
    }    
    ke_close();

    for(int i = 0;i < total_keys;i++)printf("pressed_keys[%d] is %d\n",i,pressed_keys[i]);
}

int main()
{
  _setup();
  return 0;
}
