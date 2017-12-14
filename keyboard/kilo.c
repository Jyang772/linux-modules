#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <readline/readline.h>
#include <signal.h>


struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_lflag &= ISIG;
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int keyPressed(int count, int key) {

	printf("key pressed: %d\n",key);
	//rl_on_new_line();
	return 0;
}



int main(void) {
	rl_catch_signals = 0; //Don't let readline install signal handlers. 

	rl_bind_keyseq("\\C-a",keyPressed);
	rl_bind_keyseq("\\C-g",keyPressed);
	rl_bind_keyseq("\\C-p",keyPressed);
	rl_bind_keyseq("\\C-z",keyPressed);
	
	//signal(SIGTSTP,keyPressed);
	enableRawMode();
	while(1) {
		char *line = readline("rl> ");
		/*
		char c = 0;
		read(STDIN_FILENO, &c, 1);
		if(c == 'q') {
			break;
		}*/
	}

/* 
  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;
  }
 */ 

  return 0;
}
