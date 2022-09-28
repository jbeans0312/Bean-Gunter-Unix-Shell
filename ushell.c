#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <shell.h>

#define MAXBUFFER 128

int main(void){
	char buffer[MAXBUFFER];	
	//Note to self: initially malloc prompt then use 'realloc' later when prompt is changed
	char* prefix = malloc(2 * sizeof(char));
	strcpy(prefix, " ");
	
	char* wdpath = NULL;
	wdpath = getcwd(NULL, 0);

	int running = 1;
	

	do {
		printf("%s[%s]> ", prefix, wdpath);
		if(fgets(buffer, MAXBUFFER, stdin) != NULL){
			int inputlen = strlen(buffer);

			if(buffer[inputlen - 1] == '\n'){ //replace newline char with null char
				buffer[inputlen - 1] = '\0';
			}

			if(strcmp(buffer, "exit") == 0){ //calls the exit function
				exit_ush(prefix, wdpath);
			}	

			if(strcmp(buffer, "pid") == 0){ //calls the pid function
				printf("pid: %d\n", get_pid());
			}

		}
	}while(running);
}

//Frees the memory allocated to prefix and wdpath
//Exits the shell
//Params: char* prefix, char* wdpath
void exit_ush(char* pf, char* wd){
	free(pf);
	free(wd);
	exit(1);
}

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid() {
	pid_t currentPID = getpid();
	return currentPID;
}