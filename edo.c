#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios_setting;

void killP(const char* error) {
    perror(error);
    exit(1);
}

void diableRawMode() {
    if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_setting) == -1 ) {
        killP("tcsetattr");
    }
}

void enableRawMode() {
    atexit(diableRawMode);
    struct termios raw;

    if( tcgetattr(STDIN_FILENO, &raw) == -1 ) {
        killP("tcgetattr");
    }

    orig_termios_setting = raw;
    raw.c_iflag &= ~(ICRNL| IXON | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1 ) {
        killP("tcsetattr");
    }
}

int main() {
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if(iscntrl(c)){
            printf("%d \r\n",c);
        }
        else {
            printf("%d ('%c') \r\n",c ,c);
        }
    }
    return 0;
}
