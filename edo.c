/*Includes*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/*Defines*/

#define CTRL_KEY(k) ((k) & 0x1f)

/*Data*/
struct editorConfig {
    int termRows;
    int temCols;
    struct termios orig_termios_setting;
};
struct editorConfig E;
/*terminal*/

void killP(const char* error) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(error);
    exit(1);
}

void diableRawMode() {
    if( tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios_setting) == -1 ) {
        killP("tcsetattr");
    }
}

void enableRawMode() {
    if( tcgetattr(STDIN_FILENO, &E.orig_termios_setting) == -1 ) {
        killP("tcgetattr");
    }
    atexit(diableRawMode);
    
    struct termios raw;
    raw = E.orig_termios_setting;
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

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*Output*/

void editorRows(){
    int rows;
    for(rows = 0; rows< E.termRows; rows++)
    {
        write( STDOUT_FILENO, "~\r\n",3);
    }
}

void clearScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    editorRows();
    
    write(STDOUT_FILENO, "\x1b[H", 3);
}
/*Input*/
void processKeyPress() {
    char c = readKeyPress();
    switch (c)
    {
    case (CTRL_KEY('q')):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

        exit(0);
        break;
    
    default:
        break;
    }
 }
/*Intit section*/
void initEdo() {
    if(getWindowSize(&E.termRows,& E.temCols) == -1) killP("getWindowSize");
}

int main() {
    enableRawMode();
    initEdo();

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
        clearScreen();
        processKeyPress();
    }
    return 0;
}
