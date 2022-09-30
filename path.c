#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

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
		temp->element = p;
		temp->next = NULL;
	} while(p = strtok(NULL, ":"));

	free(p);
	free(path);

	return(pathlist);
}
