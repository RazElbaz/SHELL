#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>
#include "linkedlist.h"
//https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only
#include <termios.h>     //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO


// https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
// https://www.csl.mtu.edu/cs4411.ck/www/NOTES/signal/handler.html
char command[1024],lastCommand[1024], currentCommand[1024],prompt[1024];
int DuplicateFD, mainProcess,status = 0;
int change_status(char **args);
List variables,commands;
pid_t ProccesID = -1;
int last_command=0;
char *argv[1024];
char *outfile;
int enter=0;

///////////////////////////couters//////////////////////////////////////
char **CountPIPE(char **args)
{
    char **CounterPIPE = args;
    while (*CounterPIPE != NULL)
    {
        if (strcmp(*CounterPIPE, "|") == 0)
        {
            return CounterPIPE;
        }

        CounterPIPE++;
    }

    return NULL;
}

int CountARGS(char **args)
{
    char **CounterARGS = args;
    int cnt = 0;
    while (*CounterARGS != NULL)
    {
        CounterARGS++;
        cnt++;
    }

    return cnt;
}

///////////////////////////////////////////Auxiliary functions//////////////////////////////////////

// Function to implement `strcat()` function in C
//https://developers.redhat.com/blog/2019/08/12/efficient-string-copying-and-concatenation-in-c#attempts_to_overcome_limitations
char *MYstrcat(const char *destination, const char *source)
{
    size_t size_destination = strlen(destination);
    size_t size_source = strlen(source);
    char *new_str = malloc(size_destination + size_source);
    if (!new_str)
        return NULL;
    memcpy(new_str, destination, size_destination);
    printf("%ld %ld ", size_destination , size_source);
    memcpy(new_str + size_destination, source, size_source);
    new_str[size_destination+size_source]='\0';
    return new_str;
}


void split(char *command)
{
    char *token = strtok(command, " ");
    int i = 0;

    while (token != NULL)
    {
        argv[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////

int execute(char **args)
{
    int fd, amper, rv = -1,pipe_num = 0, i = CountARGS(args);
    char **pipPointer = CountPIPE(args); 
    int pipeFD[2];
    
    if (pipPointer != NULL)
    {
        pipe_num = 1;
        *pipPointer = NULL;
        pipe(pipeFD);

        if (fork() == 0)
        {
            close(pipeFD[1]);
            close(0);
            dup(pipeFD[0]);
            execute(pipPointer + 1);
            exit(0);
        }
        DuplicateFD = dup(1);
        dup2(pipeFD[1], 1);
    }


    /* Is command empty */
    if (args[0] == NULL)
        return 0;

    /* Does command line end with & */
    if (!strcmp(args[i - 1], "&"))
    {
        amper = 1;
        args[i - 1] = NULL;
    }
    if (strcmp(args[0], "!!")==0)
    {
        strcpy(currentCommand, lastCommand);
        split(currentCommand);
        execute(argv);
        return 0;
    }

    if (args[0][0] == '$' && i > 2)
    {
        Var *var = (Var *)malloc(sizeof(Var));
        var->key = malloc((strlen(args[0]) + 1));
        var->value = malloc((strlen(args[2]) + 1));
        strcpy(var->key, args[0]);
        strcpy(var->value, args[2]);
        add(&variables, var);
        return 0;
    }

    if(strcmp(args[0], "read")==0)
    {
        Var *var = (Var *)malloc(sizeof(Var));
        var->key = malloc(sizeof(char) * (strlen(args[1])));
        var->value = malloc(sizeof(char) * 1024);
        var->key[0] = '$';
        memset(var->value, 0, 1024);
        strcpy(var->key + 1, args[1]);
        fgets(var->value, 1024, stdin);
        var->value[strlen(var->value) -1] = '\0';
        add(&variables, var);
        return 0;
    }

    if (strcmp(args[0], "cd")==0)
    {
        if (chdir(args[1]) != 0){ // chdir is a system call.
        printf(" %s: no such directory\n", argv[1]);
    
    }
        return 0;
    }

    if (strcmp(args[0], "prompt")==0)
    {
         if (!strcmp(argv[1], "="))
             {
                strcpy(prompt, args[2]);
            }

        return 0;
    }

    if (strcmp(args[0], "echo")==0)
    {
        char **echo_var = args + 1;
        if (strcmp(*echo_var, "$?")==0)
        {
            printf("%d\n", status);
            return 0;
        }

        while (*echo_var)
        {
            if (*echo_var && *echo_var[0] == '$')
            {
                char *v = NULL;
                Node *node = variables.head;
                while (node)
                {
                    if (!strcmp(((Var *)node->data)->key,*echo_var ))
                    {
                        v=((Var *)node->data)->value;
                    }
                    node = node->next;
                }
                if (v != NULL)
                    printf("%s ", v);
            }

            else{
                printf("%s ", *echo_var);
            }

            echo_var++;
        }
        printf("\n");
        return 0;
    }


    else
        amper = 0;

    int redirect = -1;
    if (i >= 2 && (!strcmp(argv[i - 2], ">") || !strcmp(argv[i - 2], ">>")))
    {
        outfile = argv[i - 1];
        redirect = 1;
    }
    else if (i >= 2 && !strcmp(argv[i - 2], "2>"))
    {
        outfile = argv[i - 1];
        redirect = 2;
    }
    else if (i >= 2 && !strcmp(argv[i - 2], "<"))
    {
        outfile = argv[i - 1];
        redirect = 0;
    }


    /* for commands not part of the shell command language */ 
    ProccesID = fork();
    if (ProccesID == 0)
    {
        /* redirection of IO ? */
        if (redirect >= 0)
        {
            if (!strcmp(args[i - 2], ">>"))
            {
                if ((fd = open(outfile, O_APPEND | O_CREAT | O_WRONLY, 0660)) < 0)
                {
                    perror("Error: Can't create file");
                    exit(1);
                }
                lseek(fd, 0, SEEK_END);
            }
            else if (!strcmp(args[i - 2], ">") || !strcmp(args[i - 2], "2>"))
            {
                if((fd = creat(outfile, 0660))<0){
                    perror("Error: Can't create file");
                    exit(1);
                }
                // lseek(fd, 0, SEEK_END);
             
            }
            else
            {
                fd = open(outfile, O_RDONLY);
            }

            close(redirect);
            dup(fd);
            close(fd);
            args[i - 2] = NULL;
            // close(STDOUT_FILENO);
        }

        execvp(args[0], args);
    }
    /* parent continues here */
    if (amper == 0)
    {
        wait(&status);
        rv = status;
        ProccesID = -1;
    }

    if (pipe_num)
    {
        close(STDOUT_FILENO);
        dup(DuplicateFD);
        wait(NULL);
    }

    return rv;
}

int change_status(char **args)
{
    int rv = -1;
    if (args[0] == NULL)
    {
        rv = 0;
        return rv;
    }
    else 
    {
        rv = execute(args);
        return rv;
    }
}

void termination_handler(int signum)
{
    strcpy(command, "^C");
    if (getpid() == mainProcess) {
        printf("\n");
        printf("You typed Control-C!");
        printf("\n");
        char message[2] = " ";
        write(0, prompt, strlen(prompt)+1);
        write(0, message, strlen(message)+1);
    }
}

int main()
{
    mainProcess = getpid();
    signal(SIGINT, termination_handler);
    strcpy(prompt, "hello:");
    char *token;
    char *new_command;
    char c;
    int last_command=0;
    while (1)
    {
        printf("%s ", prompt);
        c=getchar();
        if (c == '\033')
		{
			//these two strange prints are from:
			//https://itnext.io/overwrite-previously-printed-lines-4218a9563527
			//and they delete the previous line
			printf("\033[1A");//line up
			printf("\x1b[2K");//delete line
			getchar(); //skip the [
			switch(getchar()) { // the real value
			case 'A':
            printf("before %d \n", last_command);
            printf("before %d \n", commands.size);
                if (commands.size==0)
                {
                    break;
                }
                // if(last_command==1){
                //     last_command=1;
                // }
                else if (last_command>0)
                {
                    last_command--;
                }
                printf("after %d \n", last_command);
                printf("%s %s\n", prompt, (char *)get_command(&commands, last_command));
			    break;
			case 'B':
                printf("before %d \n", last_command);
                printf("before %d \n", commands.size);
                if (commands.size==0 ||last_command >= commands.size-1 )
                {
                    break;
                }
                else if (last_command < commands.size)
                {
                    last_command++;
                }
                // if(last_command>=commands.size){
                //     last_command=commands.size;
                // }
                // else{
                //     break;
                // }
                printf("after last command %d \n", last_command);
                printf("%s %s\n", prompt, (char *)get_command(&commands, last_command));
                break;

    		}
        
        getchar();
        continue;
        }


        if (c == '\n' && enter==1)
        {
        split((char *)get_command(&commands, last_command));
        // execute((char **)get_command(&commands, last_command));
        status = change_status(argv);
            // enter++;
            if(enter==1)
            {
                enter=0;
                char currentCodeLine[1024]; 
                fgets(currentCodeLine, 1024, stdin);
                char* commandcurr = "bash";
                char* argument_list[] = {"bash", "-c", currentCodeLine, NULL};
                if (fork() == 0) {
                    // Newly spawned child Process. This will be taken over by "bash"
                    int status_code = execvp(commandcurr, argument_list);
                    printf("bash has taken control of this child process. This won't execute unless it terminates abnormally!\n");
                    if (status_code == -1) {
                        printf("Terminated Incorrectly\n");
                        return 1;
                    }
                }
                

            }
            break;
            //         printf("\n\n");
            // wait(&status);
            //             continue;
        }
        else if (c == '\n')
        {
        enter++;
        split((char *)get_command(&commands, last_command));
        // execute((char **)get_command(&commands, last_command));
        status = change_status(argv);
        }
    command[0]=c;
    fgets(command+1 ,1023, stdin);
    char currentCodeLine[1024]; 
    //https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus
    if (!strncmp(command, "if", 2)) {
        command[strlen(command)-1] = '\n';
        while (1) {
            fgets(currentCodeLine, 1024, stdin);
            strcat(command, currentCodeLine);
            command[strlen(command) - 1] = '\n';
            if (!strcmp(currentCodeLine, "fi\n"))
                break;
        }
        char* commandcurr = "bash";
        char* argument_list[] = {"bash", "-c", command, NULL};
        if (fork() == 0) {
            // Newly spawned child Process. This will be taken over by "bash"
            int status_code = execvp(commandcurr, argument_list);
            printf("bash has taken control of this child process. This won't execute unless it terminates abnormally!\n");
            if (status_code == -1) {
                printf("Terminated Incorrectly\n");
                return 1;
            }
        }

            wait(&status);
            continue;
        }
        command[strlen(command) - 1] = '\0';
        if (!strcmp(command, "quit"))
            exit(0);

        if (strcmp(command, "!!"))
            strcpy(lastCommand, command);
        
        new_command= malloc(sizeof(char) * strlen(command));
        strcpy(new_command, command);
        add(&commands, new_command);
        last_command = commands.size;
        split(command);
        status = change_status(argv);
    }
}