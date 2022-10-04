#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <shell.h>
#include <dirent.h>

//External variables
extern char** environ;

#define MAXBUFFER 128
#define MAXARGS 10

int main(void){
	char buffer[MAXBUFFER];	
	//Note to self: initially malloc prompt then use 'realloc' later when prompt is changed
	char* prefix = malloc(128 * sizeof(char));
	strcpy(prefix, " ");
	
	char* wdpath = NULL;
	wdpath = getcwd(NULL, 0);

	int running = 1;

	char **args=malloc(MAXARGS*sizeof(char*));

	PathElement* p = get_path();

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
				exit_ush(prefix, wdpath, p, args);
			}	

			if(strcmp(args[0], "pid") == 0){ //calls the pid function
				printf("pid: %d\n", get_pid());
			}

			if(strcmp(args[0], "kill") == 0){ //calls the kill function
				cmd_kill(args);
			}

			if(strcmp(args[0], "printenv") == 0){ //calls the printenv function
				cmd_printenv(args);
			}

			if(strcmp(args[0], "prefix") == 0){ //calls the prefix function
				cmd_prefix(&prefix, args);
			}

			if(strcmp(args[0], "list") == 0) { //calls the list function
				cmd_list(args);
			}

			if(strcmp(args[0], "where") == 0) { //calls the where function
				WHERE(args, p);
			}

			if(strcmp(args[0], "which") == 0) { //calls the which function 
				WHICH(args, p);
			}

		}
	}while(running);
}

//Frees the memory allocated to prefix, wdpath, and args
//Exits the shell
//Params: char* prefix, char* wdpath, char** args
void exit_ush(char* pf, char* wd, PathElement* p,  char** args){
	free(pf);
	free(wd);
	free(args);
	//code to free PathElement* p
	PathElement* tmp;
	while(p){
		tmp = p;
		p = p->next;
		free(tmp);
	}
	free(p);
	freePath();
	exit(1);
}

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid() {
	pid_t currentPID = getpid();
	return currentPID;
}

//HI WILL, I code on vim, pls copy and paste the comments over here with ur VSC powers :D
int cmd_list(char** args){
	int status = 0, warning = 0;
	DIR *folder;
	if(args[1] == NULL){
		//print files in the working dir
		char* cwd = getcwd(NULL, 0);
		folder = opendir(cwd);
		if(folder == NULL){
			status = 0;
			printf("list: unable to read directory\n");
		}else{
			status = 1;
			printf("list: reading current directory\n");
			struct dirent *entry;
			int file_num = 0;
			while(entry = readdir(folder)){
				file_num++;
				printf("File: %d: %s\n", file_num, entry->d_name);
			}	
		}
		closedir(folder);
		free(cwd);
	}else{
		//print files in given directory(ies)(iez)(gg2ez)
		for(int argnum = 1; args[argnum] != NULL; argnum++){
			folder = opendir(args[argnum]);
			printf("list: [%s]", args[argnum]);
			if(folder == NULL){
				status = 0;
				printf("list: unable to read directory\n\tgiven directory [%s] does not exist\n", args[argnum]);
			}else{
				status = 1;
				printf("list: reading %s\n", args[argnum]);
				struct dirent *entry;
				int file_num = 0;
				while(entry = readdir(folder)){
					file_num++;
					printf("\tFile: %d: %s\n", file_num, entry->d_name);
				}	
			}
			closedir(folder);
		}
	}
	if(warning){
		printf("list: WARNING, one or more given paths are invalid");
	}
	return(status);	
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

//Function that implements the functionality of the printenv command - "prints the value of an environment variable"
//If there are no arguments, the function will print all the environment variables and their values
//If there is one argument, the function will print the value of the environment variable specified by the argument
//First index in the args array is the command name, second index is the first argument, etc.
//Params: char** args[]
//Returns: an integer representing the status of the printenv command - 0 means failure, 1 means success
int cmd_printenv(char** args) {
	int status = 0;
	if(args[1] == NULL) {
		for(int i = 0; environ[i] != NULL; i++) {
			printf("%s\n", environ[i]);
		}
	} else if(args[2] == NULL) {
		char* env = getenv(args[1]);
		if(env != NULL) {
			printf("%s\n", env);
		} else {
			printf("printenv: %s: no such variable\n", args[1]);
		}
	} else {
		printf("printenv: too many arguments\n");
	}
}

//Function that allows the user to edit the prompt prefix of the shell
//When the function is called with no arguments, the user is prompted to enter a new prefix
//When the function is called with an argument, the parameter is set as the new prefix
//When the function is called with 2 or more arguments, the function prints an error message
//Params: char* prefix
//Returns: None
void cmd_prefix(char** prefix, char** args) {
	if(args[1] == NULL) {
		printf("Enter new prefix: ");
		char* newPrefix = malloc(MAXBUFFER * sizeof(char));
		fgets(newPrefix, MAXBUFFER, stdin);
		int inputlen = strlen(newPrefix);
		if(newPrefix[inputlen - 1] == '\n'){ //replace newline char with null char
			newPrefix[inputlen - 1] = '\0';
		}
		strcpy(*prefix, newPrefix);
		free(newPrefix);
	} else if(args[2] == NULL) {
		strcpy(*prefix, args[1]);
	} else {
		printf("prefix: too many arguments\n");
	}
}
