//John Bean, Will Gunter
//CISC361
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

char *path, *p;

//Need to free the memory allocated by the linked list
//Returns: a linked list where each node is a directory in a path
PathElement* get_path() {
	PathElement *temp, *pathlist = NULL;
	
	p = getenv("PATH"); //FREE ME
	path = malloc((strlen(p) + 1) * sizeof(char)); //FREE ME
	strncpy(path, p, strlen(p));
	path[strlen(p)] = '\0';

	p = strtok(path, ":");

	//Loop through PATH to build linked list of directories
	do {
		if(!pathlist){ //create head of list
			temp = calloc(1, sizeof(PathElement)); //FREE EXTERN
			pathlist = temp;
		}else{ //add other elements to the list
			temp->next = calloc(1, sizeof(PathElement)); //FREE EXTERN
			temp = temp->next;
		}
		temp->dir_name = p;
		temp->next = NULL;
	} while(p = strtok(NULL, ":"));
	
	return(pathlist);
}

//Frees the path value
//No params, no returns
void freePath(){
	free(path);
}

int WHERE(char** args, PathElement* p) {
	char cmd[64];
	int status = 0;

	if(args[1] == NULL){
		status = 0;
		printf("where: invalid arguments\n");
	}else{
		while(p){
			sprintf(cmd, "%s/%s", p->dir_name, args[1]);
			if(access(cmd, F_OK) == 0){
				printf("[%s]\n", cmd);
			}
			p = p->next;
		}
		status = 1;
	}
	return(status);
}

char* WHICH(char** args, PathElement* p) {
	char *cmd;
	char c[64];
	char *program_name;

	if(args[1] == NULL && strcmp(args[0], "which") == 0){ //checks if the user just types "which"
		printf("which: no arguments\n");
	}else{
		if(strcmp(args[0], "which") == 0){ //handles if WHICH was called from the command line
			program_name = malloc(sizeof(char) * (strlen(args[1])+ 1));
			strcpy(program_name, args[1]);
			program_name[strlen(args[1])] = '\0';
		}else{ //handles if WHICH was called from the exec code
			program_name = malloc(sizeof(char) * (strlen(args[0])+1));
			strcpy(program_name, args[0]);
			program_name[strlen(args[0])] = '\0';
		}

		while(p){ //iterates through the pathlist and finds the path of the given program
			sprintf(c, "%s/%s", p->dir_name, program_name);
			if(access(c, X_OK) == 0){
				cmd = malloc(sizeof(char) * (strlen(c)+1));
				strcpy(cmd, c);
				cmd[strlen(c)] = '\0';
				break;
			}
			p = p->next;
		}
		free(program_name);
		if(p){
			return(cmd);
		}
	}
	//if the program is not found, return "error"
	sprintf(c, "%s", "error");
	cmd = malloc((strlen(c) + 1) * sizeof(char));
	strcpy(cmd, c);
	cmd[strlen(c)] = '\0';
	return(cmd);
}
