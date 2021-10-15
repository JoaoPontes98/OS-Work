#ifndef A2LIB_H
#define A2LIB_H
#define PATH_MAX 300
#define BUFSIZE 81
#define ARGS_MAX 100
#define PID_IDX 0
#define STATUS_IDX 1
#define STATUS_SUSP_MIN 5000 //When execs are suspended, they have a status of > 5000

//Used for the linked list of commands for pipes
typedef struct CMD_Node
{
    char *cmd_arr[ARGS_MAX];
    struct CMD_Node *next;
} CMD_Node;

//Globally tracks  of the last job's pid and status
//Important for suspending and proceeding with jobs
extern int pid_status[2];

//Globally tracks which job suspension message needs to be printed
extern int susp_flag;

//Input: A string & an array
//Output: Populates the array with tokenized argument strings
void parse_args(char *line, char *args_arr[ARGS_MAX]);

//Input: Argument Array
//Output: number of pipes in the arguments
int count_pipes(char *args_arr[ARGS_MAX]);

//Input: Argument Array
//Output: A malloc'd CmdNode containing the args
CMD_Node* malloc_CMD_Node();

//Input: Argument Array
//Output: The head of a command linked list partitioned by pipes
CMD_Node* make_CMD_LL(char *args_arr[ARGS_MAX]);

//Input: Argument array containing pipes.
void exec_args(char *args_arr1[ARGS_MAX], int num_pipes);

//Input: The Signal thrown
void sig_handler(int sig);

//Prints the process suspension message
void print_susp_msg();

//Input: The head to a CMD_Node linked list
//Frees the memory of the linked list
void free_CMD_List(CMD_Node* head);

#endif
