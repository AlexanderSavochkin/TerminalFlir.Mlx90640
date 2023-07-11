/*** Includes ***/
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include "termutils.h"

struct TerminalConfig terminal_config;

const int termCommandSizeLimit = 64;

/*** Terminal ***/
void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4); /*Clear screen*/
    write(STDOUT_FILENO, "\x1b[H", 3); /*Return cursor to home position*/

    perror(s);
    exit(1);
}

void terminal_disable_raw_mode() {
   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal_config.orig_termios) == -1)
       die("tcsetattr");
}

void terminal_enable_raw_mode() {
    if ( tcgetattr(STDIN_FILENO, &terminal_config.orig_termios) == -1)
        die("tcsgetattr");

    atexit(terminal_disable_raw_mode);
    struct termios raw = terminal_config.orig_termios;

    raw.c_iflag &= ~(BRKINT | IXON | ICRNL | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int get_window_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void terminal_set_background_color(unsigned char r, unsigned char g, unsigned char b) {
    char buffer[termCommandSizeLimit];
    sprintf(buffer, "\x1b[48;2;%hhu;%hhu;%hhum", r, g, b);
    int len = strnlen(buffer, termCommandSizeLimit);
    write(STDOUT_FILENO, buffer, len);
}

void move_cursor(unsigned char row, unsigned char col) {
    char buffer[termCommandSizeLimit];
    sprintf(buffer, "\x1b[%hhu;%hhuH", row, col);
    int len = strnlen(buffer, termCommandSizeLimit);
    write(STDOUT_FILENO, buffer, len);
}

void clear_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

char terminal_read_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
       if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}
