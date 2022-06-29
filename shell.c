
/*Declaring necessary Header Files below*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAXCMD 64
#define MAXARG 256
#define MAXLINE 512
#define MAXLINEPLUS 513

static char *cmd_sep = ";";//to be used in 1st level of parsing
static char *delim = " \t\n";//to be used for 2nd level of parsing

int exit_flag = 0, batch = 0;

char *com[MAXARG];//to store the commands after 1st level of parsing
char line[MAXLINEPLUS];//to store the commands entered by user in one line
char *cmds[MAXCMD];//to store the commands obtained after 2nd level parsing of commands

void reset_com()//resets the com to com array with all zeros
{
    memset(com, 0, sizeof(com));
}
void reset_cmds()//resets the cmds to cmds array with all zeros
{
    memset(cmds, 0, sizeof(cmds));
}
void reset_line()//resets the line to empty line
{
    memset(line, 00, sizeof(line));
}

void dump_comd()//displays all commands entered by user
{
    int i;
    for (i = 0; com[i]; i++)
        printf("cmds[%d]=%s\n", i, com[i]);
}

void execute_commands()//function to handle commands execution
{
    int i = 0; // looping var
    int j = 0; // looping var
    int p = 0; // looping var

    char *token = NULL;

    while (fgets(line, sizeof(line), stdin))
    {

        if (strlen(line) > MAXLINE)
        {
            perror("Input line max length exceeded!!");
            continue;
        }

        // skip lines starting with whitespace,NULL,or carriage return
        if (isspace(line[0] || line[0] == 0 || line[0] == 13))
        {
            continue;
        }
        // Tokenize the string
        i = 0;
        token = strtok(line, cmd_sep);
        if (strcmp(token, "quit") == 0 || strcmp(token, "exit") == 0)
            exit_flag = 1;
        while (token != NULL)
        {
            if (strcmp(token, " ") != 0)
            {
                if (strcmp(token, "\n") != 0) // handling case where user enters ;;;; ,so empty command;This prevents segementation fault
                {
                    cmds[i] = token;
                    printf("cmds[%d]:%s\n", i, cmds[i]);
                }
                else
                    cmds[i] = NULL;

                ++i;
            }
            token = strtok(NULL, cmd_sep);

        } // 1st level tokenization ends.

        cmds[i] = NULL;

        // 2nd level Tokenization begins below

        for (i = 0; cmds[i]; ++i)
        {
            j = 0;

            printf("\n\nProcessing command %s----\n", cmds[i]);
            token = strtok(cmds[i], delim);
            if (strcmp(token, "quit") == 0 || strcmp(token, "exit") == 0)
                exit_flag = 1;
            while (token != NULL)
            {

                com[j++] = token;

                token = strtok(NULL, delim);
            }
            com[j + 1] = NULL;

            printf("Parsed Commands =\n");
            dump_comd();

            pid_t pid = 1;

            pid = fork();
            if (pid == 0)
            {
                execvp(*com, com); // execute each command on a different child process.
            }
            if (pid < 0)
            {
                printf("Error executing child processpwd");
            }

            if (exit_flag)
                exit(EXIT_SUCCESS);
            reset_com();
        }
       
        // Below we wait for each child process to complete and execute their respective commands;
        for (p = 0; p < i; p++)
        {
            if(p==0)
 printf("\nExecuting commands....\n");
            wait(NULL);
        }

        if (exit_flag)
            exit(EXIT_SUCCESS);
        reset_line();
        reset_cmds();
        printf("\nDone\n");
        if (!batch)
            printf("\nprompt~>>");
    }
}

int main(int argc, char *argv[MAXCMD])
{

    reset_com();
    reset_cmds();
    reset_line();

    if (argc > 1)
    {
        freopen(argv[1], "r", stdin);
        if (stdin == NULL)
        {
            perror("**Batch file failed to open");
            exit(EXIT_FAILURE);
        }

        batch = 1;
    }
    if (!batch)
        printf("prompt~>>");
    execute_commands();
}