#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

//Need to free the memory allocated by the linked list
//Returns: a linked list where each node is a directory in a path
PathElement* get_path() {
	char *path, *p;
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

int WHERE(char** args) {
	char cmd[64];
	PathElement *p;
	p = get_path();
	int status = 0;

	if(args[1] == NULL){
		status = 0;
		printf("where: invalid arguments");
	}else if(args[2] != NULL){
		status = 0;
		printf("where: too many arguments");
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

int WHICH(char** args) {
	char cmd[64];
	PathElement *p;
	p = get_path();
	int status = 0;

	if(args[1] == NULL){
		status = 0;
		printf("which: invalid arguments");
	}else if(args[2] != NULL){
		status = 0;
		printf("which: too many arguments");
	}else{
		while(p){
			sprintf(cmd, "%s/%s", p->dir_name, args[1]);
			if(access(cmd, X_OK) == 0){
				printf("[%s]\n", cmd);
				break;
			}
			p = p->next;
		}
		status = 1;
	}
	return(status);
}
