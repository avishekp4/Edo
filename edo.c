/*Includes*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/*Defines*/

#define CTRL_KEY(k) ((k) & 0x1f)

/*Data*/

struct termios orig_termios_setting;

/*terminal*/

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
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1 ) {
        killP("tcsetattr");
    }
}
char readKeyPress(){
    int nread;
    char c;
    while( (nread = read(STDIN_FILENO, &c, 1)) != 1 ) {
        if(nread == -1 & errno != EAGAIN){
            killP("Read");
        }
    }
    return c;
}
/*Input*/
void processKeyPress() {
    char c = readKeyPress();
    switch (c)
    {
    case (CTRL_KEY('q')):
        exit(0);
        break;
    
    default:
        break;
    }
 }
/*Intit section*/

int main() {
    enableRawMode();

    while (1) {
        // char c = '\0';
        // if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) killP("Read");
        // if(iscntrl(c)){
        //     printf("%d \r\n",c);
        // }
        // else {
        //     printf("%d ('%c') \r\n",c ,c);
        // }
        // if(c == CTRL_KEY('q')) {
        //     break;
        // }
        processKeyPress();
    }
    return 0;
}
