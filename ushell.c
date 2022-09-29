#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <shell.h>

//External variables
extern char** environ;

#define MAXBUFFER 128
#define MAXARGS 10

int main(void){
	char buffer[MAXBUFFER];	
	//Note to self: initially malloc prompt then use 'realloc' later when prompt is changed
	char* prefix = malloc(2 * sizeof(char));
	strcpy(prefix, " ");
	
	char* wdpath = NULL;
	wdpath = getcwd(NULL, 0);

	int running = 1;

	char **args=malloc(MAXARGS*sizeof(char*));
	
	do {
		printf("%s[%s]> ", prefix, wdpath);
		if(fgets(buffer, MAXBUFFER, stdin) != NULL){
			int inputlen = strlen(buffer);

			if(buffer[inputlen - 1] == '\n'){ //replace newline char with null char
				buffer[inputlen - 1] = '\0';
			}

			//Break the buffer into an array of strings broken up by the space character
			//First, we need to clear the args array in case it was used in a previous iteration
			//As a result, args[0] will be the command being run and args[1] will be the first argument
			for(int i = 0; i < MAXARGS; i++){
				args[i] = NULL;
			}
			int i = 0;
			char* token = strtok(buffer, " ");
			while(token != NULL){
				args[i] = token;
				token = strtok(NULL, " ");
				i++;
			}

			if(strcmp(args[0], "exit") == 0){ //calls the exit function
				exit_ush(prefix, wdpath, args);
			}	

			if(strcmp(args[0], "pid") == 0){ //calls the pid function
				printf("pid: %d\n", get_pid());
			}

			if(strcmp(args[0], "kill") == 0){ //calls the kill function
				cmd_kill(args);
			}

		}
	}while(running);
}

//Frees the memory allocated to prefix, wdpath, and args
//Exits the shell
//Params: char* prefix, char* wdpath, char** args
void exit_ush(char* pf, char* wd, char** args){
	free(pf);
	free(wd);
	free(args);
	exit(1);
}

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid() {
	pid_t currentPID = getpid();
	return currentPID;
}

//Function that implements the functionality of the kill command - "kills a process"
//If there is only one argument, the function will kill the process with the pid specified by the argument
//If there are two arguments and one of them starts with a -, the function will send that signal to the pid specified by the other argument
//Params: char* args[]
//Returns: an integer representing the status of the kill command - 0 means failure, 1 means success
int cmd_kill(char** args) {
	int status = 0;
	if(args[1] == NULL) {
		printf("kill: missing operand\n");
		status = 0;
	} else if(args[2] == NULL) {
		int pid = atoi(args[1]);
		if(kill(pid, 0) == 0) {
			kill(pid, SIGTERM);
			status = 1;
		} else {
			printf("kill: invalid pid\n");
			status = 0;
		}
	} else if(args[3] == NULL) {
		if(args[1][0] == '-') {
			int signal = atoi(args[1]) * -1;
			int pid = atoi(args[2]);
			if(kill(pid, 0) == 0) {
				kill(pid, signal);
				status = 1;
			} else {
				printf("kill: invalid pid\n");
				status = 0;
			}
		} else {
			printf("kill: invalid signal\n");
			status = 0;
		}
	} else {
		printf("kill: too many arguments\n");
		status = 0;
	}
	return status;
}