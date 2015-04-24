/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#ifndef MYSHELL_H
#define MYSHELL_H

#define DEBUG
#define PROGRAM_NAME	"MyShell"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <limits.h>

#include <sys/types.h>

#include "utility.h"
#include "intrnlcmd.h"

static bool printed_prefix = false;

char **tokenise_line(char *line);
void print_shell_prefix();
void intercept_signal(int signum);
int parse_command(char *line);

#endif
