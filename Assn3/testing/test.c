#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>  

int kbhit()
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, NULL) > 0;
}

void* keyboardRoutine(void *args){
	// Middleware to allow keyboard detection without clicking the return key.
	static struct termios oldt, newt;
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);

	while(1){
		//This needs to wrap the kb input
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		if(kbhit()){
			printf("\n%c\n",getchar());		
		}
		tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	}
	return 0;
}

int main(void) {
	int c; 

	pthread_t keyboardT;
	pthread_create(&keyboardT, NULL, &keyboardRoutine, NULL);
	while(1){};

	return 0;
}
