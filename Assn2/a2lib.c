#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2lib.h"

//Input: The Signal thrown
void sig_handler(int sig) {
	signal(sig,sig_handler);
	susp_flag = 1;
}

//Prints the job suspension message
void print_susp_msg(){
	if(pid_status[STATUS_IDX] == 0){
		printf("No Job To Suspend\n");
	}else{
		printf("job suspended. Type 'fg' to resume.\n");
	}
}

//Input: A string & an array
//Output: Populates the array with tokenized argument strings
void parse_args(char *line, char *args_arr[ARGS_MAX])
{	
	char *line_copy, *line_copy_rest, *token;
	int i = 0;

	//Allocating mem for cmd
	line_copy = strdup(line);
	line_copy_rest = line_copy;
	
	//Create arrary of arguments
	while((token = strtok_r(line_copy_rest, " \n", &line_copy_rest))){
		args_arr[i] = token;
		i++;
	}	
}

//Input: Argument Array
//Output: number of pipes in the arguments
int count_pipes(char *args_arr[ARGS_MAX]){
	int num_pipes = 0;
	int i = 0;

	while(args_arr[i] != NULL){
		if (strcmp(args_arr[i], "|") == 0){
			num_pipes++;	
		}
		i++;	
	}
	
	return num_pipes;
}

//Input: Argument Array
//Output: A malloc'd CmdNode containing the args
CMD_Node* mallocCmdNode(){
	//malloc the struct
	CMD_Node* cmd_node = (CMD_Node*)malloc(sizeof(CMD_Node));

	//set array elements to null
	memset(cmd_node->cmd_arr, 0, sizeof(cmd_node->cmd_arr));

	cmd_node->next = NULL;

	return cmd_node;
}

//Input: Argument Array
//Output: The head of a command linked list partitioned by pipes
CMD_Node* makeCmdLL(char *args_arr[ARGS_MAX]){
	CMD_Node* head = mallocCmdNode();
	CMD_Node* curr_node = head;
	
	int i=0;
	int j=0;
	while(args_arr[i] != NULL){
		if(strcmp(args_arr[i], "|") == 0){
			curr_node->next = mallocCmdNode();
			curr_node = curr_node->next;
			j = 0;	
		}else{
			curr_node->cmd_arr[j] = args_arr[i];
			j++;	
		}
		i++;
	}

	return head;
}

//Input: Argument array containing pipes.
void exec_args(char *args_arr[ARGS_MAX], int num_pipes){
	int status;
	int pipefds[2*num_pipes];
	CMD_Node* curr_node = makeCmdLL(args_arr);
	pid_t pid;

	//Open all pipes
	int i = 0;
	while(i<num_pipes){
		if(pipe(pipefds + i*2) < 0){
			printf("ERROR: Couldn't produce pipe!\n");
			exit(EXIT_FAILURE);		
		}
		i++;
	}

	int j = 0;
	while(curr_node){
		pid = fork();
		if(pid == 0){	//Child processs
			
			//if not last			
			if(curr_node->next){
				if(dup2(pipefds[j + 1], 1) < 0){
				    printf("ERROR: Couldn't dup2!\n");
				    exit(EXIT_FAILURE);
				}			
			}

			//if not first command && j!= 2*numPipes
			if(j != 0 ){
				if(dup2(pipefds[j-2], 0) < 0){
					perror("ERROR: Couldn't dup2!\n");///j-2 0 j+1 1
					exit(EXIT_FAILURE);
				}
            }
			
			//close all pipes
			i = 0;
			while(i < num_pipes*2){
				close(pipefds[i]);
				i++;			
			}

			//execute the command of the current node
			if(execvp(curr_node->cmd_arr[0], curr_node->cmd_arr) < 0){
				printf("ERROR: coudln't exec args!\n");
				exit(EXIT_FAILURE);		
			}
		}else if(pid < 0){
			printf("ERROR: Couldn't fork!\n");
			exit(EXIT_FAILURE);		
		}
		
		//if(curr_node->next){ //if not last, save PID in case of ^Z
		//	pid_status[PID_IDX] = (int)pid;
		//}

		curr_node = curr_node->next;
		j = j+2;
	}

	i = 0;
	while(i < num_pipes*2){
		close(pipefds[i]);
		i++;			
	}

	i = 0;
	while(i < num_pipes+1){
		waitpid(-1, &status, WUNTRACED);
		i++;			
	}
	
	pid_status[PID_IDX] = (int)pid;
	pid_status[STATUS_IDX] = status;	
}


