
//Defines the PathElement struct which is used to build the linked list that
//represents the path to the current working drectory
struct PathElement;

typedef struct PathElement{
	char* dir_name; 
	struct PathElement* next;
}PathElement;

//Params: char* prefix and char* wdpath and char** args
//Frees the memory allocated to the prefix and the memory allocated to the working directory strings
//Exits the shell
//Returns: None
void exit_ush(char* pf, char* wp, char** args);

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
