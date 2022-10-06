#include <unistd.h>
//Defines the PathElement struct which is used to build the linked list that
//represents the path to the current working drectory
struct PathElement;

typedef struct PathElement{
	char* dir_name; 
	struct PathElement* next;
}PathElement;

//Params: None
//Gets the PATH env var and parses it into a linked list
//Allocates memory for the PATH var (freed internally)
//Allocates memory for the linked list (needs to be freed externally)
//Returns: A linked list where each node is a directory in the path
PathElement* get_path();

//Params: char** args[]
//Path is the linked list built by the get_path();
//Searches for all versions of a program on the path
//Returns: (1 or 0 for success or failure)
int WHERE(char** args, PathElement* path);

//Params: char** args[], PathElement* path
//Path is the linked list built by get_path();
//Searches for the firste executible version of a program on the path
//Returns: (1 or 0 for success or failure)
char* WHICH(char** args, PathElement* path);

//Literally just frees the char* path used in get_path()
//free(path); that is all lol
void freePath();

//Params: char* prefix and char* wdpath and char** args
//Frees the memory allocated to the prefix and the memory allocated to the working directory strings
//Exits the shell
//Returns: None
void exit_ush(char* pf, char* wp, PathElement* p,  char** args);

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid();

//Function that implements the functionality of the kill command - "kills a process"
//If there is only one argument, the function will kill the process with the pid specified by the argument
//If there are two arguments and one of them starts with a -, the function will send that signal to the pid specified by the other argument
//Params: char** args[]
//Returns: an integer representing the status of the kill command - 0 means failure, 1 means success
int cmd_kill(char** args);

//Function that implements the functionality of the printenv command - "prints the value of an environment variable"
//If there are no arguments, the function will print all the environment variables and their values
//If there is one argument, the function will print the value of the environment variable specified by the argument
//First index in the args array is the command name, second index is the first argument, etc.
//Params: char** args[]
//Returns: an integer representing the status of the printenv command - 0 means failure, 1 means success
int cmd_printenv(char** args);

//Function that allows the user to edit the prompt prefix of the shell
//When the function is called with no arguments, the user is prompted to enter a new prefix
//When the function is called with an argument, the parameter is set as the new prefix
//When the function is called with 2 or more arguments, the function prints an error message
//Params: char* prefix
//Returns: None
void cmd_prefix(char** prefix, char** args);

//Function that prints a line by line list of each file in a given directory
//Called with no args: prints the files in the working directory
//Called with a path to a directory: attempts to print the files contained within the given directory
//Params: char** args[]
//Returns: an integer representing the status of the list command - 0 means failure, 1 means success
int cmd_list(char** args);

//Function that prints the current working directory
//Params: None
//Returns: None
void cmd_pwd();

//Function that moves the current working directory to the directory if passed a path in args[1].
//If no path is passed, the function with chdir to the home directory.
//If - is the only argument, chdirs to directory previously in
//Params: char** args[]
//Returns: nothing
void cmd_cd(char** args);

//Function that lets the user set/create an environmental variable.
//If there are no arguments, the function calls printenv with no arguments
//If there is one argument, the function creates an environmental variable with the name specified by the argument and an empty value
//If there are two arguments, the function creates an environmental variable with the name specified by the first argument and the value specified by the second argument
//If there are more than 2 arguments, the function prints an error message
//Params: char** args[]
//Returns: nothing
void cmd_setenv(char** args);

