#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios_setting;

void diableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_setting);
}

void enableRawMode() {
    atexit(diableRawMode);
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    orig_termios_setting = raw;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if(iscntrl(c)){
            printf("%d\n",c);
        }
        else {
            printf("%d ('%c') \n",c ,c);
        }
    }
    return 0;
}
