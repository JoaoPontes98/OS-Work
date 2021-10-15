#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAXBUFLEN 50
#define MAXPROC   50
#define MAXPROCMEM 1000
#define MAXPROCDUR 1000
#define MAXUSERNAME 30

struct Process {
	char username[MAXUSERNAME]; //max name length of a user
	char name;
	long int arrival;
	long int duration;
	struct Process *next;
};

struct User{
	char username[MAXUSERNAME]; //max name length of a user
	long int duration;
	long int stopTime;
	struct User *next;
};

int main() {
	char source[MAXBUFLEN + 1]; //Buffered input from STDIN
	int numOfProcs = 0;
	
	//Defining the linked-list of process structs
	struct Process *listHead = malloc(sizeof(struct Process));
	struct Process *listCurr = listHead;

	//Data Parser
	int i = 0; //outer loop counter
	int j = 0; //inner loop counter
	char* cursor; //cursor to navigate the string
	while(fgets(source, MAXBUFLEN, stdin) != NULL){
		cursor = source;
		if(i != 0 && strcmp(source, "\n") != 0){ //skip if it's the first line or an empty line
			struct Process *newProc = malloc(sizeof(struct Process));
			numOfProcs++;
			while (j != MAXBUFLEN){
				if(*cursor != '\t'){ //Parse the data to form into Process structs
					(*newProc).username[j] = *cursor;
				}
				else{ 
					cursor++;
					(*newProc).name = *cursor;
					cursor++;
					(*newProc).arrival = strtol(cursor, &cursor, 10);
					cursor++;
					(*newProc).duration = strtol(cursor, &cursor, 10);
					if(i == 1){
						listHead = newProc;
						listCurr = newProc;					
					}else{
						(*listCurr).next = newProc;
						listCurr = newProc;
					}
					break;	
				}
				j++;
				cursor++;		
			}
			j = 0;
		}
		i = i+1;
	}

	//Defining the user data linked list
	struct User *userHead = malloc(sizeof(struct User));
	struct User *userCurr = userHead; //this somehow breaks the code
	listCurr = listHead;
	int numOfUsers = 0;
	int userFound = 0;
	i = 0;
	j = 0;
	while(i<numOfProcs){
		while(j<numOfUsers){
			//printf("i=%d, ProcUname: %s,	UserUname: %s\n", i, (*listCurr).username,(*userCurr).username);
			if(strcmp((*listCurr).username, (*userCurr).username) == 0){
				//printf("%ld(%s) = %ld(%s) + %ld(%s)\n", (*userCurr).duration, (*userCurr).username, (*userCurr).duration, (*userCurr).username, (*listCurr).duration, (*listCurr).username);
				(*userCurr).duration = (*userCurr).duration + (*listCurr).duration;
				userFound = 1;
				break;
			}
			userCurr = (*userCurr).next;
			j++;
		}
		if(userFound == 0){
			if(numOfUsers != 0){ //Append more users to the linked list
				snprintf((*userCurr).username, sizeof((*userCurr).username), "%s", (*listCurr).username);
				(*userCurr).duration = (*listCurr).duration;
				(*userCurr).next = malloc(sizeof(struct User));
			}else{ //create head of the user list
				snprintf((*userHead).username, sizeof((*userHead).username), "%s", (*listCurr).username);
				(*userHead).duration = (*listCurr).duration;
				(*userHead).next = malloc(sizeof(struct User));
			}
			numOfUsers++;
		}
		listCurr = (*listCurr).next;
		userCurr = userHead;
		userFound = 0;
		j = 0;
		i++;
	}

	//The SJF Algorithm **add the last bit of the algorithm to find the end times
	printf("Time	Job\n");
	int done = 0;
	int currentTime = (*listHead).arrival;
	int shortestDur = MAXPROCDUR;
	userCurr = userHead;
	struct Process *nextProc = listHead;
	while(done == 0){
		i = 0;
		listCurr = listHead;
		while(i<numOfProcs){ //Find the shortest duration during the appropriate arrival time				
			if(currentTime >= (*listCurr).arrival){
				if((*listCurr).duration < shortestDur && (*listCurr).duration > 0){
					nextProc = listCurr;
					shortestDur = (*listCurr).duration;			
				}			
			}
			i++;
			listCurr = (*listCurr).next;
		}
		if(shortestDur < MAXPROCDUR){ //if a new shortest duration was found, print it
			printf("%d	%c\n", currentTime, (*nextProc).name);
			(*nextProc).duration = (*nextProc).duration - 1;
			currentTime++;
			shortestDur = MAXPROCDUR;
			while(userCurr != NULL){ //Subtract 1 duration from the user's total duration
				if((strcmp((*nextProc).username, (*userCurr).username) == 0)){
					(*userCurr).duration--;
					if((*userCurr).duration == 0){
						(*userCurr).stopTime = currentTime;	
					}
					break;			
				}
				userCurr = (*userCurr).next;
			}
			userCurr = userHead;
		}else{ //else print IDLE and exit the loop
			printf("%d	IDLE\n", currentTime);
			done = 1;
		}
	}

	//print the summary
	i = 0;
	userCurr = userHead;
	printf("\nSummary\n");
	while(i<numOfUsers){
		printf("%s	%ld\n", (*userCurr).username, (*userCurr).stopTime);
		userCurr = (*userCurr).next;
		i++;
	}

 	return(0);
}
//
///
//
/
/
/
/
/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 81

int main(int argc, char *argv[]){
	int ret_code;		// return code
	int len;		// length of entered command
	char buffer[BUFSIZE];	// room for 80 chars plus \0
	char *cmd;		// pointer to entered command
	int pid;
	char * pch;
	char *arg1;
	char *arg2;

	// Print a prompt and read a command from standard input
	printf("[Joao's Lab 2 Shell]$");
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

			pch = strtok (cmd," ");
			//*arg1 = *pch;
			printf("pch = %s\n", *pch);

			pch = strtok (NULL ," ");
			//*arg2 = *pch;
			printf("pch = %s\n", pch);
			//printf("command = %s %s\n", arg1, arg2);

			ret_code = execlp(cmd, cmd, NULL);
			if(ret_code != 0){
				printf("Error executing %s. \n", cmd);
				exit(0);
			}
		}
		printf("[Joao's Lab 2 Shell]$ ");
		cmd = fgets(buffer, BUFSIZE, stdin);
	}
	return(0);
}