#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

#define BUFSIZE 81

int main(int argc, char *argv[]){
	int ret_code;		// return code
	int len;		// length of entered command
	char buffer[BUFSIZE];	// room for 80 chars plus \0
	char *cmd;		// pointer to entered command
	int pid;		// the process ID
	char *pch;		// help tokenize the command
	char *arg1;		// the first part of cmd
	char *arg2;		// the second part of cmd

	// Print a prompt and read a command from standard input
	printf("[Joao's Lab 2 Shell]$ ");
	cmd = fgets(buffer, BUFSIZE, stdin);
	
	while(cmd != NULL){	
		pid = fork();
		if(pid != 0){
			wait();		
		}
		else{
			len = strlen(buffer);
			if(buffer[len-1] == '\n'){
				buffer[len-1] = '\0';
			}

			//parsing the command up to 1 arg
			pch = strtok (cmd," ");
			arg1 = pch;
			pch = strtok (NULL ," ");
			arg2 = pch;

			if(arg2 == NULL){ //If no arg
				ret_code = execlp(arg1, arg1, (char *)NULL);
			}
			else{	//If the command has an arg
				ret_code = execlp(arg1, arg1, arg2, (char *)NULL);
			}
			if(ret_code != 0){
				printf("Error executing %s. \n", cmd);
				exit(EXIT_FAILURE);
			}
		}
		printf("[Joao's Lab 2 Shell]$ ");
		cmd = fgets(buffer, BUFSIZE, stdin);
	}
	printf("\nExit Joao's Shell!\n");
	return(0);
}