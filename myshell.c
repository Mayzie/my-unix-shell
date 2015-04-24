/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#include "myshell.h"

/**
 * pause_shell()	- pauses the shell, stops input
 *
 * Pauses the shell and blocks all input until the ENTER key
 * has been pressed, after which the shell will resume as normal.
 * Prompts the user to "Press ENTER to continue...".
 *
 * Result: 0 on success, -1 on failure.
 */
int pause_shell() {
	struct termios attr_old;
	struct termios attr_new;

	if(isatty(STDIN_FILENO) == 0) {
		return -1;
	}

	// Backup shell attributes
	tcgetattr(STDIN_FILENO, &attr_old);

	tcgetattr(STDIN_FILENO, &attr_new);
	attr_new.c_lflag &= ~(ICANON | ECHO);
	attr_new.c_cc[VMIN] = 1;
	attr_new.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr_new);

	// Send all input to the null device (prevents it from
	// echoing to the shell).
	char c;
	FILE *null_device = fopen("/dev/null", "w");
	fflush(stdin);
	
	printf("Please press ENTER to continue...\n");
	fflush(stdout);

	while(true) {
		read(STDIN_FILENO, &c, 1);
		if(c == '\n') {
			break;
		}
	}

	fclose(null_device);

	// Restore original shell attributes
	tcsetattr(STDIN_FILENO, TCSANOW, &attr_old);

	return 0;
}

/**
 * print_shell_prefix()	- prints the shell prompt
 *
 * Displays the shell prompt on the screen if it hasn't
 * already been printed.
 */
void print_shell_prefix() {
	// Have we just printed the prompt?
	if(printed_prefix)
		return;

	char *env_user = getenv("USER");
	char *env_path = getenv("PWD");

	printf("[");
	if(env_user == NULL)
		printf("%s", PROGRAM_NAME);
	else
		printf("%s", env_user);

	printf("@");

	if(env_path == NULL)
		printf("...");
	else
		printf("%s", env_path);
	
	printf("] ");

	printed_prefix = true;
}

/**
 * intercept_signal()	- catches signals from the prompt
 *
 * Catches and intercepts the CTRL+C (SIGINT) and CTRL+Z
 * (SIGTSTP) signals from the shell and acts on them
 * accordingly.
 */
void intercept_signal(int signum) {
	switch(signum) {
		case SIGINT:
			printf("\n");
			print_shell_prefix();
			fflush(stdout);
			break;
		case SIGTSTP:
			printf("\n");
			print_shell_prefix();
			fflush(stdout);
			break;
	}
}

/**
 * parse_command()	- parses the command line
 *
 * Parses the command line and executes the intended
 * program with the supplied argument.
 *
 * Returns 0 on success, 1 when the shell should quit,
 * <0 on error.
 */
int parse_command(char *line) {
	int result;

	FILE *f_stdout = NULL;
	FILE *f_stdin = NULL;
	bool exec_background = false;

	char *command;
	char **arguments;
	int len_cmd = strlen(line);

	// Split the supplied line up into the command and arguments portion.
	arguments = tokenise_line(line);
	command = arguments[0];
	char **curr_arg = arguments;

	// Handle I/O redirection requests.
	while(*curr_arg++ != NULL) {
		if(is_io_char(*(curr_arg - 1))) {
			if(strcmp("<", *(curr_arg - 1)) == 0) {	// Change stdin
				if(is_io_char(*curr_arg) || (f_stdin != NULL) || exec_background) {
					result = -1;
					goto CLEANUP;
				}

				f_stdin = fopen(*curr_arg, "r");
				if(f_stdin == NULL) {
					fprintf(stderr, "Unable to open file '%s' for reading.\n", *curr_arg);
					result = -1;
					goto CLEANUP;
				}
			} else if(strcmp(">", *(curr_arg - 1)) == 0) { // Change stdout (write)
				if(is_io_char(*curr_arg) || (f_stdout != NULL) || exec_background) {
					result = -1;
					goto CLEANUP;
				}

				f_stdout = fopen(*curr_arg, "w");
			} else if(strcmp(">>", *(curr_arg - 1)) == 0) { // Change stdout (append)
				if(is_io_char(*curr_arg) || (f_stdout != NULL) || exec_background) {
					result = -1;
					goto CLEANUP;
				}

				f_stdout = fopen(*curr_arg, "a");
			} else if(strcmp("&", *(curr_arg - 1)) == 0) { // Make process execute in the background.
				if(exec_background) {
					result = -1;
					goto CLEANUP;
				}

				exec_background = true;
			}

			if(exec_background)
				*(curr_arg - 1) = NULL;
			else 
				*(curr_arg++ - 1) = NULL;
		} else {
			if(exec_background) {	// There should be no more arguments after background execution has been specified.
				result = -1;
				goto CLEANUP;
			}
		}
	}

	// Set stdout and stdin if they haven't been already.
	if(f_stdout == NULL)
		f_stdout = stdout;
	if(f_stdin == NULL)
		f_stdin = stdin;

	// Execute internal commands.
	if(strcmp("clr", command) == 0) {
		clear_screen();
	} else if(strcmp("quit", command) == 0) {
		result = 1;
		goto CLEANUP;
	} else if(strcmp("echo", command) == 0) {
		echo(f_stdout, &arguments[1]);
	} else if(strcmp("cd", command) == 0) {
		if(arguments[1]) {
			change_dir(arguments[1]);
		} else {
			char *env_pwd = getenv("PWD");
			if(env_pwd == NULL)
				fprintf(stderr, "Unable to retrieve the current directory.\n");
			else if(f_stdout == stdout)
				fprintf(f_stdout, "Current directory: %s\n", env_pwd);
		}
	} else if(strcmp("environ", command) == 0) {
		print_env(f_stdout);
	} else if(strcmp("dir", command) == 0) {
		dir(f_stdout, arguments, len_cmd);
	} else if(strcmp("pause", command) == 0) {
		result = pause_shell();
		goto CLEANUP;
	} else if(strcmp("help", command) == 0) {
		display_help(f_stdout);
	} else {	// Otherwise execute the provided command.
		execute_command(command, arguments, f_stdout, f_stdin, exec_background);
	}

	result = 0;

CLEANUP:	
	// Close all file streams if opened
	if((f_stdin != NULL) && (f_stdin != stdin))
		fclose(f_stdin);
	if((f_stdout != NULL) && (f_stdout != stdout))
		fclose(f_stdout);

	free(arguments);
	return result;
}

int main(int argc, char **argv) {
	char *command;
	ssize_t len;
	FILE *f_stdin = NULL;

	// Set SHELL environment variable.
	char *shell_path = realpath(argv[0], NULL);
	setenv("SHELL", shell_path, true);

	// Ignore and intercept signals.
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, intercept_signal);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTSTP, intercept_signal);

	// Check program arguments (batch file)
	if(argc > 1) {
		f_stdin = fopen(argv[1], "r");
		printed_prefix = true;
	}
	if(f_stdin == NULL)	// Check if it's a file we can read.
		f_stdin = stdin;

	// Begin REPL
	while(true) {
		command = NULL;

		print_shell_prefix();

		// Retrieve line.
		size_t alloc;	// So we can block until RETURN
		len = getline(&command, &alloc, f_stdin);
		printed_prefix = f_stdin != stdin;
		if(len == -1)	// Did we get anything?
			break;
		if(command[len - 1] == '\n')	// Since getline() leaves the \n on the end, remove it.
			command[len - 1] = '\0';	// Replace '\n' with NULL character
		if(strcmp(command, "") == 0)	// Dit we get anything useful (i.e. no blank lines)?
			continue;

		int res;
		if((res = parse_command(command))) {	// Interpret command
			if(res > 0)
				break;
		}
		
		free(command);
	}

	// Cleanup
	free(command);
	free(shell_path);
	free(pwd_current);

	return 0;
}
