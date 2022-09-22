#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAXBUFFER 128

int main(void){
	char buffer[MAXBUFFER];	
	//Note to self: initially malloc prompt then use 'realloc' later when prompt is changed
	char* prefix = malloc(2 * sizeof(char));
	strcpy(prefix, " ", 2);
	
	char* wdpath = NULL;
	int pathlen = (int) strlen(getcwd(NULL, 0)) + 1;
	wdpath = malloc(pathlen * sizeof(char));
	strcpy(wdpath, getcwd(NULL, 0), pathlen);

	int running = 1;
	

	do {
		printf("%s[%s]>", prefix, wdpath);
		if(fgets(buffer, MAXBUFFER, stdin) != NULL){
			int inputlen = strlen(buffer);

			if(buffer[inputlen - 1] == '\n'){ //replace newline char with null char
				buffer[inputlen - 1] = '\0';
			}	
			if(strcmp(buffer, "exit" == 0){
				free(prefix); //replace with a call to a memory freeing function?
				free(wdpath);
				running = 0;
			}
		}
	}while(running);

	exit(1);
}
