#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "a2lib.h"

int pid_status[2] = {0,0};
int susp_flag = 0;

int main(int argc, char *argv[]){
	char cwd[PATH_MAX];		// The current directory path
	char buffer[BUFSIZE];	// Room for 80 chars plus \0
	char *cmd;				// Pointer to entered command
	char *cmd_arr[ARGS_MAX];// Array of commands and arguments
	int ret_code;			// Return code for the cd internal command
	int num_pipes;			// Number of pipes in a command

	//Handle ^Z signals
	void sig_handler(int);
	if (signal(SIGTSTP,sig_handler)==SIG_ERR){
		perror("setup of SIGTSTP handler failed");
		exit(1);
	}

	do {
		//Clear the argument array
		memset(cmd_arr, 0, sizeof cmd_arr);

		//Print job suspensions message
		if(susp_flag){print_susp_msg();susp_flag = 0;}

		//Print the new line prefix
		getcwd(cwd, sizeof(cwd));
		printf("%s%% ",cwd);
		cmd = fgets(buffer, BUFSIZE, stdin);

		//Parse arugments into an array	and count pipes		
		parse_args(cmd, cmd_arr);
		num_pipes = count_pipes(cmd_arr);

		//Check for empty line or exit
		if(cmd_arr[0] && strcmp(cmd,"exit\n")){
			//Check for internal commands (cd, fg, bg)
			if(!strcmp(cmd_arr[0],"cd")){
				ret_code = chdir(cmd_arr[1]);
				if(ret_code != 0){
					printf("Could not find Directory: %s\n", cmd_arr[1]);
				}
			}else if(!strcmp(cmd_arr[0],"fg")){
				if(!pid_status[STATUS_IDX]){
					printf("fg: No current job.\n");
				}else{
					kill(pid_status[PID_IDX], SIGCONT);
					waitpid(pid_status[PID_IDX], &pid_status[STATUS_IDX], WUNTRACED);
				}
			}else if(!strcmp(cmd_arr[0],"bg")){
				if(!pid_status[STATUS_IDX]){
					printf("bg: No current job.\n");
				}else{
					kill(pid_status[PID_IDX], SIGCONT);
				}
			}else if(pid_status[STATUS_IDX]){
				printf("Not allowed to start a new command while you have a job active.\n");
				printf("[HINT] Enter 'fg' to continue job\n");
			}else{
				//If no internal arguments, then execute arguments
				exec_args(cmd_arr, num_pipes);
			}	
		}
	} while (strcmp(cmd,"exit\n") != 0);

	exit(0);
}

