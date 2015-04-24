/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUM_MAX_ARGUMENTS 64
#define WHITESPACE	" \f\n\r\t\v"

char **tokenise_line(char *line);
bool is_io_char(char *argument);
char *extract_path(char *filename);

#endif
