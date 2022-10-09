#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <shell.h> 
#include <dirent.h>
#include <sys/stat.h>

//External variables
extern char** environ;

#define MAXBUFFER 128
#define MAXARGS 10

int main(void){

	char buffer[MAXBUFFER];	
	//Note to self: initially malloc prompt then use 'realloc' later when prompt is changed
	char* prefix = malloc(128 * sizeof(char));
	strcpy(prefix, " ");

	//Get the cwd & move into env variable for later
	char *wdpath = getcwd(NULL, 0);
	setenv("OLDPWD", wdpath, 1);

	int running = 1;

	char **args=malloc(MAXARGS*sizeof(char*));

	PathElement* p = get_path();

	char* homedir = getenv("HOME");
	
	do {
		//Signal Catching Code
        	signal(SIGINT, signalHandler);
        	signal(SIGTSTP, signalHandler);
        	signal(SIGTERM, signalHandler);

		char* cwd = getcwd(NULL, 0);
		printf("%s[%s]> ", prefix, cwd);
		free(cwd);
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
			if (args[0] == NULL) {
				continue;
			}

			if(strcmp(args[0], "exit") == 0){ //calls the exit function
				print_status(args[0]);
				exit_ush(prefix, wdpath, p, args);
			}	

			else if(strcmp(args[0], "pid") == 0){ //calls the pid function
				print_status(args[0]);
				printf("pid: %d\n", get_pid());
			}

			else if(strcmp(args[0], "kill") == 0){ //calls the kill function
				print_status(args[0]);
				cmd_kill(args);
			}

			else if(strcmp(args[0], "printenv") == 0){ //calls the printenv function
				print_status(args[0]);
				cmd_printenv(args);
			}

			else if(strcmp(args[0], "prompt") == 0){ //calls the prefix function
				print_status(args[0]);
				cmd_prefix(&prefix, args);
			}

			else if(strcmp(args[0], "list") == 0) { //calls the list function
				print_status(args[0]);
				cmd_list(args);
			}

			else if(strcmp(args[0], "where") == 0) { //calls the where function
				print_status(args[0]);
				WHERE(args, p);
			}

			else if(strcmp(args[0], "which") == 0) { //calls the which function 
				print_status(args[0]);
				char* cmd = WHICH(args, p);
				if(strcmp("error", cmd) != 0){
					printf("[%s]\n", cmd);
				}else{
					printf("which: given program not found\n");
				}
				free(cmd);
			}
			
			else if (strcmp(args[0], "pwd") == 0) { //calls the cd function
				print_status(args[0]);
				cmd_pwd();
			}

			else if (strcmp(args[0], "cd") == 0) { //calls the cd function
				print_status(args[0]);
				cmd_cd(args);
			}

			else if (strcmp(args[0], "setenv") == 0) { //calls the cd- function
				print_status(args[0]);
				cmd_setenv(args);

			}else{ //exec user programs
				if(strncmp(args[0], "/", 1) == 0 
					|| strncmp(args[0], "./", 2) == 0  
					|| strncmp(args[0], "../", 3) == 0)
				{ //execute program with absolute path
					struct stat path;
					stat(args[0], &path);
					
					if(S_ISREG(path.st_mode) == 0){ //checks if the given path is a directory
						printf("Not a file, cannot execute.\n");
					}else{
						if(access(args[0], X_OK) == 0){ //if the given path is not a directory, try to execute
							pid_t child_pid, wpid;
							int status;

							child_pid = fork();
							if(child_pid == -1){ //fork failed
								perror("fork");
							}

							if(child_pid == 0){ //fork success
								print_status(args[0]);
								int exec_val = execve(args[0], args, NULL);

								if(exec_val == -1){ //exec failure
									perror("execve");
								}
							}else{ //parent process
								do{
									wpid = waitpid(child_pid, &status, 0);

									if(wpid == -1){
										perror("waitpid");
									}
									
									if(WIFEXITED(status)){
										printf("child exited, status %d\n", WEXITSTATUS(status));
									}else if(WIFSIGNALED(status)){
										printf("child killed, status %d\n", WTERMSIG(status));
									}
								}while (!WIFEXITED(status) && !WIFSIGNALED(status));
							}
						}else{
							printf("given absolute path does not have execute perms\n");
						}
					}
				}else{ //search for and execute programs on search path
					char* exec_path = WHICH(args, p);
					if(strcmp(exec_path, "error") == 0){ //check if the program exists
						printf("%s: command not found\n", args[0]);
					}else{ //if program exists, execute program in child process, wait for child process to complete 					 			
						pid_t child_pid, wpid;
						int status;
						child_pid = fork();
						if(child_pid == -1){ //fork failed
							perror("fork");
						}
						
						if(child_pid == 0){ //fork success, we are in the child process (major W) ez clap ez dub ggwp
							print_status(args[0]);
							//Scan arguments for wildcards
							int starWildcard = find_wildcard(args, '*');
							int questionWildcard = find_wildcard(args, '?');
							char** starExpanded;
							char** questionExpanded;
							int starExpandLength = 0;
							int questionExpandLength = 0;
							char** expanded_wildcard_args;
							int num_args = 0;
							for (int i = 0; args[i] != NULL; i++) {
								num_args++;
							}
							//If starWildCard != -1 or questionWildcard != -1, then we have a wildcard
							if (starWildcard != -1 || questionWildcard != -1) {
								if (starWildcard != -1) {
									//If we have a star wildcard, we need to expand it
									starExpanded = expand_wildcard(args, starWildcard);
									while (starExpanded[starExpandLength] != NULL) {
									starExpandLength++;
									}
								}
								if (questionWildcard != -1) {
									//If we have a question wildcard, we need to expand it
									questionExpanded = expand_wildcard(args, questionWildcard);
									while (questionExpanded[questionExpandLength] != NULL) {
									questionExpandLength++;
									}
								}
								//Combine it all together with original args
								expanded_wildcard_args = malloc(MAXBUFFER * (num_args + starExpandLength + questionExpandLength + 1));
								int i = 0;
								//Put original args in minus the args that contain the wildcards
								for(int x = 0; x < num_args; x++) {
									if (x != starWildcard && x != questionWildcard) {
										expanded_wildcard_args[i] = args[x];
										i++;
									}
								}
								//Put the expanded star wildcard args in
								if (starWildcard != -1) {
									for (int x = 0; x < starExpandLength; x++) {
										expanded_wildcard_args[i] = starExpanded[x];
										i++;
									}
								}
								//Put the expanded question wildcard args in
								if (questionWildcard != -1) {
									for (int x = 0; x < questionExpandLength; x++) {
										expanded_wildcard_args[i] = questionExpanded[x];
										i++;
									}
								}
								//Put the NULL in
								expanded_wildcard_args[i] = NULL;
								if (expanded_wildcard_args[1] == NULL) {
									printf("No matches for wildcard.\n");
								}
								else {
									int exec_val = execve(exec_path, expanded_wildcard_args, NULL);
									if(exec_val == -1){ //exec failure
										perror("execve");
									}
								}	
							} else {
								//If we don't have a wildcard, we can just execute the program
								int exec_val = execve(exec_path, args, NULL);
								if(exec_val == -1){ //exec failure
									perror("execve");
								}
							}
						}else{ //fork success, we are in the parent process wooooo
							do{
								wpid = waitpid(child_pid, &status, 0);
								
								if(wpid == -1){
									perror("waitpid");
								}

								if(WIFEXITED(status)){
									printf("child exited, status %d\n", WEXITSTATUS(status));
								}else if(WIFSIGNALED(status)){
									printf("child killed, status %d\n", WTERMSIG(status));
								}
							}while (!WIFEXITED(status) && !WIFSIGNALED(status));
						}
					}
					free(exec_path);
				}
			}

		} else {
			printf("Caught Ctrl-D. Ignoring and continuing...\n");
			clearerr(stdin);
		}
	}while(running);
}

//Prints the program that the shell is about to exectue
//Params: args[0] or the path to the program
//Returns: none
void print_status(char* program){
	printf("executing: %s\n", program);
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

	unsetenv("OLDPWD");

	exit(0);
}

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid() {
	pid_t currentPID = getpid();
	return currentPID;
}

//Function that prints a line by line list of each file in a given directory
//Called with no args: prints the files in the working directory
//Called with a path to a directory: attempts to print the files contained within the given directory
//Params: char** args[]
//Returns: an integer representing the status of the list command - 0 means failure, 1 means success
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
			return 1;
		}
	} else if(args[2] == NULL) {
		char* env = getenv(args[1]);
		if(env != NULL) {
			printf("%s\n", env);
			return 1;
		} else {
			printf("printenv: %s: no such variable\n", args[1]);
			return 0;
		}
	} else {
		printf("printenv: too many arguments\n");
		return 0;
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

//Function that prints the current working directory
//Params: None
//Returns: None
void cmd_pwd() {
	char *wdpath = getcwd(NULL, 0);
	printf("%s\n", wdpath);
	free(wdpath);
}

//Function that moves the current working directory to the directory if passed a path in args[1].
//If no path is passed, the function with chdir to the home directory.
//If - is the only argument, chdirs to directory previously in
//Params: char** args[]
//Returns: an integer representing the status of the cd command - 0 means failure, 1 means success
void cmd_cd(char** args) {
	int status = 0;
	if(args[1] == NULL || strcmp(args[1], "~") == 0) {
		char* home = getenv("HOME");
		if(chdir(home) == 0) {
			status = 1;
		} else {
			printf("cd: unable to change directory\n");
			status = 0;
		}
	} else if(args[2] == NULL) {
		if(strcmp(args[1], "-") == 0) {
			if(chdir(getenv("OLDPWD")) == 0) {
				char *tempDir = getcwd(NULL, 0);
				status = 1;
				setenv("OLDPWD", tempDir, 1);
				free(tempDir);
			} else {
				printf("cd: unable to change directory\n");
				status = 0;
			}
		} else {
			if(chdir(args[1]) == 0) {
				status = 1;
			} else {
				printf("cd: unable to change directory\n");
				status = 0;
			}
		}
	} else {
		printf("cd: too many arguments\n");
		status = 0;
	}
}

//Function that lets the user set/create an environmental variable.
//If there are no arguments, the function calls printenv with no arguments
//If there is one argument, the function creates an environmental variable with the name specified by the argument and an empty value
//If there are two arguments, the function creates an environmental variable with the name specified by the first argument and the value specified by the second argument
//If there are more than 2 arguments, the function prints an error message
//Params: char** args[]
//Returns: nothing
void cmd_setenv(char** args) {
	if(args[1] == NULL) {
		cmd_printenv(args);
	} else if(args[2] == NULL) {
		setenv(args[1], "", 1);
	} else if(args[3] == NULL) {
		setenv(args[1], args[2], 1);
	} else {
		printf("setenv: too many arguments\n");
	}
}

//Function that returns the index of the first occurance of the wildcard character passed in wildcard parameter
//Returns -1 if there are no wildcard characters
//Params: char** args[], char wildcard
//Returns: an integer representing the index of the first wildcard character in the args array
int find_wildcard(char** args, char wildcard) {
	int index = -1;
	for(int i = 0; args[i] != NULL; i++) {
		if(strchr(args[i], wildcard) != NULL) {
			index = i;
			break;
		}
	}
	return index;
}

//Function that expands wildcard given the list of args, the index of the star wildcard, and the index of the question mark wildcard
//This function only handles star wildcard and question mark wildcard, nothing else
//Uses glob() to expand the wildcards
//Params: char** args[], int star_index, int question_index
//Returns: char** expanded_args (the expanded list of args)
char** expand_wildcard(char** args,int wildcardIndex) {
	glob_t globbuf;
	globbuf.gl_offs = 0;
	glob(args[wildcardIndex], GLOB_DOOFFS, NULL, &globbuf);
	char** expanded_args = malloc((globbuf.gl_pathc + 1) * sizeof(char*));
	for(int i = 0; i < globbuf.gl_pathc; i++) {
		expanded_args[i] = malloc(MAXBUFFER * sizeof(char));
		strcpy(expanded_args[i], globbuf.gl_pathv[i]);
	}
	expanded_args[globbuf.gl_pathc] = NULL;
	globfree(&globbuf);
	return expanded_args;
}

//Function that is run when Control + C (SIGINT), Control + Z (SIGTSTP), and SIGTERM signals are passed to the shell.
//The function doesn't actually do anything.
void signalHandler(int signal) {
	printf("\nRecieved signal - ignoring...\n");
	printf("Press enter/return to acknowledge.\n");
}
