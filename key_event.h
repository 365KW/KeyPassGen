#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include <termios.h>

extern int peek_character;
static struct termios initial_settings, new_settings;

void ke_init(void);
void ke_close(void);
int ke_kbhit(void);
int ke_readch(void);

#endif //KEY_EVENT_H
