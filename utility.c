/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#include "utility.h"

/**
 * tokenise_line()	- breaks up a line
 *
 * Explodes/breaks up a string using whitespace
 * as delimiters.
 *
 * Returns the 2D array of the broken up line.
 */
char **tokenise_line(char *line) {
	char **arguments = malloc(sizeof(void) * NUM_MAX_ARGUMENTS);
	char **curr_arg = arguments;
	*curr_arg++ = strtok(line, WHITESPACE);
	while((*curr_arg++ = strtok(NULL, WHITESPACE)));	// Make the last argument NULL.

	return arguments;
}

/**
 * is_io_char()	- is the argument an I/O symbol?
 *
 * Checks whether the supplied argument is an I/O
 * redirection symbol.
 *
 * Returns true if it is, false otherwise.
 */
bool is_io_char(char *argument) {
	return (strcmp(">", argument) == 0) || (strcmp(">>", argument) == 0) || (strcmp("<", argument) == 0) || (strcmp("&", argument) == 0);
}

/**
 * extract_path()	- returns the file path
 *
 * Returns the filepath of the filename (i.e. it
 * ignores and excludes the file, only returns the 
 * directory it is in).
 *
 * Returns the path, without the file.
 */
char *extract_path(char *filename) {
	char *result;
	int len_fn = strlen(filename);

	result = calloc(len_fn + 1, sizeof(char));
	int pos;
	for(pos = len_fn; pos >= 0; --pos) {
		if(filename[pos] == '/') {
			strncpy(result, filename, pos + 1);
			break;
		}
	}

	return result;
}
