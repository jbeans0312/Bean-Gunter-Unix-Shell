
//Defines the PathElement struct which is used to build the linked list that
//represents the path to the current working drectory
struct PathElement;

typedef struct PathElement{
	char* dir_name; 
	struct PathElement* next;
}PathElement;

//Params: char* prefix and char* wdpath
//Frees the memory allocated to the prefix and the memory allocated to the working directory strings
//Exits the shell
//Returns: None
void exit_ush(char* pf, char* wp, char* args);

//Function that implements the functionality of the pid command - "prints the pid of the shell"
//Params: None
//Returns: the pid as the datatype pid_t
pid_t get_pid();

//Function that 