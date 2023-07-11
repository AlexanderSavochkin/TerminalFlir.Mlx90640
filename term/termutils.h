#pragma once

#include <termios.h>

/*** Defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)

#define MAX_PALETTE 1024

#ifdef __cplusplus
extern "C" {
#endif

/*** Data ***/
struct TerminalConfig {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

extern struct TerminalConfig terminal_config;

/*** Terminal ***/
void die(const char *s);
void terminal_disable_raw_mode();
void terminal_enable_raw_mode();

int get_window_size(int *rows, int *cols); 
void terminal_set_background_color(unsigned char r, unsigned char g, unsigned char b);
void clear_screen();
void move_cursor(unsigned char row, unsigned char col);
char terminal_read_key();

#ifdef __cplusplus
} //extern "C"
#endif