/*******************************
 * COMP3520 Assignment 1
 * Daniel Collis - 430133523
 ******************************/
#ifndef INTRNLCMD_H
#define INTRNLCMD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/wait.h>

#include "utility.h"

static char *pwd_current = NULL;

void execute_command(char *command, char **arguments, FILE *f_stdout, FILE *f_stdin, bool exec_background);
void clear_screen();
void echo(FILE *f_stdout, char **arguments);
void change_dir(char *new_directory);
void print_env(FILE *f_stdout);
void dir(FILE *f_stdout, char **arguments, int size);
void display_help(FILE *f_stdout);

#endif
