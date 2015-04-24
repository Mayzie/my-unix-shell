/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#include "intrnlcmd.h"

/**
 * execute_command()	- executes a program
 *
 * Executes program 'command', with arguments 'arguments'.
 * Executes in the background if exec_background is true.
 * Will redirect input and output to f_stdin and f_stdout descriptors
 * respectively.
 */
void execute_command(char *command, char **arguments, FILE *f_stdout, FILE *f_stdin, bool exec_background) {
	pid_t process_id;
	int status;
	
	switch(process_id = fork()) {
		case -1:
			fprintf(stderr, "Unable to execute command '%s'.", command);
			break;
		case 0:
			setenv("PARENT", getenv("SHELL"), true);	// Set PARENT

			// Close already open fd's.
			close(STDIN_FILENO);
			close(STDIN_FILENO);

			// Overwrite stdin and stdout fd's with wanted ones.
			dup2(fileno(f_stdin), STDIN_FILENO);
			dup2(fileno(f_stdout), STDOUT_FILENO);

			// Execute command.
			execvp(command, arguments);
			break;
		default:
			if(!exec_background) {
				waitpid(process_id, &status, WUNTRACED);
			}
	}
}

/**
 * clear_screen()	- clears the terminal screen.
 */
void clear_screen() {
	printf("\033[2J\033[1;1H");
}

/**
 * echo()	- prints out the arguments, separated by a space.
 */
void echo(FILE *f_stdout, char **arguments) {
	char **arg_cpy = arguments;
	// Iterate over arguments and print them out.
	while(*arg_cpy++ != NULL) {
		fprintf(f_stdout, "%s", *(arg_cpy - 1));
		if(*arg_cpy != NULL)	// Is this the last argument?
			printf(" ");
	}
	fprintf(f_stdout, "\n");
}

/*
 * change_dir()	- changes the current working directory.
 *
 * Changes th current working directory of the program to 'new_directory'
 * and updates the PWD environment variable accordingly.
 */
void change_dir(char *new_directory) {
	struct stat dir;
	int dir_res = stat(new_directory, &dir);

	// Check if the directory exists.
	if(dir_res == -1) {
		fprintf(stderr, "The directory '%s' does not exist.\n", new_directory);
	} else {
		if(S_ISDIR(dir.st_mode)) {	// Is it actually a directory?
			chdir(new_directory);
			char *cwd = getcwd(NULL, 0);
			char *pwd_temp = getenv("PWD");
			setenv("PWD", cwd, true);
			if(pwd_current != NULL)	// Free previous enviornment variable that we set.
				free(pwd_current);
			pwd_current = pwd_temp;
		} else {
			fprintf(stderr, "'%s' is not a directory.\n", new_directory);
		}
	}
}

/*
 * print_env()	- prints the environment to the shell.
 */
void print_env(FILE *f_stdout) {
	extern char **environ;
	char **env = environ;
	// Iterate over all environemnt variables and display them.
	while(*env++ != NULL) {
		fprintf(f_stdout, "%s\n", *(env - 1));
	}
}

/*
 * dir()	- display the contents of directories
 *
 * Displays the file contents of the directories supplied in
 * 'arguments'. 'size' is the length of the original, untokenised
 * command line.
 *
 * This command is analagous to 'ls -al <arguments>'.
 */
void dir(FILE *f_stdout, char **arguments, int size) {
	// Combine 'ls -al ' string and all arguments.
	char *new_command = calloc(size + 8, sizeof(char));
	strcat(new_command, "ls -al ");

	char **iter_arg = &arguments[1];
	while(*iter_arg++ != NULL) {
		strcat(new_command, *(iter_arg - 1));
		if(*iter_arg != NULL)
			strcat(new_command, " ");
	}

	// Retokenise new command.
	char **new_args = tokenise_line(new_command);
	
	execute_command("ls", new_args, stdin, f_stdout, false);

	free(new_command);
	free(new_args);
}

/**
 * display_help()	- shows the help file on the shell.
 *
 * Displays the contents of the readme file found in the shell
 * directory, using the more command. It is expected to be
 * there, and will print an error if not there.
 */
void display_help(FILE *f_stdout) {
	// Get the path the shell executable is located at.
	char *shell_path = extract_path(getenv("SHELL"));

	// Concatenate "more" with the path to the readme file.	
	char *new_command = calloc(strlen(shell_path) + 12, sizeof(char));
	strcat(new_command, "more ");
	strcat(new_command, shell_path);
	strcat(new_command, "readme");

	// Execute 'more'
	char **new_args = tokenise_line(new_command);
	execute_command("more", new_args, stdin, f_stdout, false);

	free(shell_path);
	free(new_command);
	free(new_args);
}
