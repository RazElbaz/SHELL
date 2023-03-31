#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>
// https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
// https://www.csl.mtu.edu/cs4411.ck/www/NOTES/signal/handler.html
void termination_handler(int signum)
{
    printf("You typed Control-C!");
}

int main() {
    char command[1024];
    char *token;
    char *outfile;
    int i, fd, amper, redirect, retid, status;
    char *argv[10];
    char prompt[1024] = "hello";
    char last[1024]="last command";
if (signal (SIGINT, termination_handler) == SIG_IGN)
    signal (SIGINT, SIG_IGN);
while (1)
{
    printf("%s: ",prompt);
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';

    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
    }
    argv[i] = NULL;

    /* Is command empty */
    if (argv[0] == NULL)
        continue;

    /* Does command line end with & */ 
    if (! strcmp(argv[i - 1], "&")) {
        amper = 1;
        argv[i - 1] = NULL;
    }
    else 
        amper = 0; 
    //Question 7
    if(!strcmp(argv[0], "quit"))
        exit(0);
    //Question 6
    if (!strcmp(argv[0], "!!"))
        {
            strcpy(command, last);
            i = 0;
            token = strtok(command, " ");
            while (token != NULL)
            {
                argv[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            argv[i] = NULL;
        }
    //Question 5 
    if (! strcmp(argv[0], "cd")){   
     if (chdir(argv[1]) == -1){  // chdir is a system call.
        perror("System cannot find the path specified");
     }
    continue;
    }
    //Question 3+4  
    if (! strcmp(argv[0], "echo")){   
            if(!strcmp(argv[1], "$?")){
                printf("%d\n", status);
            }
            else {
                for (int j = 1; j < i; j++) {
                printf("%s ", argv[j]);
                }
                printf("\n");
            }
            continue;
        }
        //Question 2
    else if (!strcmp(argv[0], "prompt"))
        {
            if (!strcmp(argv[1], "="))
            {
                strcpy(prompt, argv[2]);
            }
            continue;
    }

    //Question 1
    if (!strcmp(argv[i - 2], ">")) {
        redirect = 1;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
        }
     else if (!strcmp(argv[i - 2], "2>")) {   
        redirect = 2;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
        }
    else if (!strcmp(argv[i - 2], ">>")){
            redirect = 3;
            argv[i - 2] = NULL;
            outfile = argv[i - 1];
        }
    else 
        redirect = 0; 

    /* for commands not part of the shell command language */ 
        if (fork() == 0)
        {
            /* redirection of IO ? */
            if (redirect == 3)
            { 
                //http://math.haifa.ac.il/ronn/advprog/unix_c/docs/htm/iosyscalls.htm
                if ((fd = open(outfile, O_WRONLY |  O_CREAT | O_EXCL, 0660)) < 0)
                {
                    perror("ERROR");
                    exit(1);
                }
                close(STDOUT_FILENO);
                dup(fd);
                close(fd);
            }
            else if (redirect == 1 || redirect == 2)
            {
                if ((fd = creat(outfile, 0660)) < 0)
                {
                    perror("ERROR");
                    exit(1);
                }
                close(redirect);
                dup(fd);
                close(fd);
                /* stdout is now redirected */
            }
            execvp(argv[0], argv);
        }
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
    }
}
