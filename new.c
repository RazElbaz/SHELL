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
int last_command,fd, amper, rv ,piping , i;
int DuplicateFD, mainProcess,status = 0;
int change_status(char **args);
char current_command[1024]; 
List variables,commands;
char **CountPIPEPointer; 
pid_t ProccesID,cpid;
char* end_if="fi\n";
char *new_command2;
char *new_command;
char *argv[1024];
char *outfile;
int pipeFD[2]; 
char *token;
int enter=0;
char a,b,c;



////////////////////////////////////////////////couters//////////////////////////////////////
char **CountPIPE(char **args)
{
/**
 * A helper function that counts how many times | appears In the receiving line in the terminal
*/
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
/**
 * A helper function that counts how many arguments appears In the receiving line in the terminal
*/
    char **CounterARGS = args;
    int cnt = 0;
    while (*CounterARGS != NULL)
    {
        CounterARGS++;
        cnt++;
    }
    return cnt;
}

///////////////////////////////////////////helper functions//////////////////////////////////////

void split(char *command)
{
/**
* A helper function designed to split the command
*/
    char *part_of_command = strtok(command, " ");
    int i = 0;
    while (part_of_command != NULL)
    {
        argv[i] = part_of_command;
        part_of_command = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;
}



//https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
void termination_handler(int signum)
{
/**
* A function designed to handle the signal as soon as the user presses ctrl+c
*/    
    if (getpid() == mainProcess) {
        printf("\n");
        printf("You typed Control-C!");
        printf("\n");
        write(0, prompt, strlen(prompt)+1);
        return;
    }
    else{
        // fprintf(stderr, "\ncaught signal: %d\n", signum);
        return;
    }
}

char *safe_strcpy(char *dest, size_t size, char *src) 
{
/**
 * A helper function designed to simulate the strcpy function using the strcat function
 */
    if (size > 0) {
        *dest = '\0';
        return strncat(dest, src, size - 1);
    }
    return dest;
}

int first_index_in_str(char* arr){
/**
 * A helper function designed to find the index of the first element in the string
 */
    int j;
        for (j = 0; i < 1024; i++) {
        if (strlen(arr) <= i) {
            break;
        }
    }
    return j;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int execute(char **args)
{
/**
 * This function simulates the function exec()
 * An execute function in a terminal typically refers to the ability to run commands or programs directly from the command line interface.
 * In Unix-based systems, this is commonly accomplished using the exec() family of functions, which are used to replace the current process image with a 
 * new process image. These functions are typically used in conjunction with the fork() system call to create a new process, which can then be replaced with a new program using the exec() functions.
 * The exec() functions take the name of the program to be executed, as well as any command-line arguments that should be passed to the program. 
 * Once the exec() function is called, the current process image is replaced with the new program, which begins executing from the beginning of its main() function.
 * Overall, the ability to execute commands and programs directly from the terminal is a powerful feature of Unix-based systems that allows for efficient and flexible interaction with the operating system and other programs.
 
 Websites I used:
 https://stackoverflow.com/questions/43295721/how-to-duplicate-a-child-descriptor-onto-stdout-fileno-and-stderr-fileno ->  fd
 https://man7.org/linux/man-pages/man2/pipe.2.html-> pipe
 */
    rv = -1; piping  = 0; i = CountARGS(args); char **CountPIPEPointer = CountPIPE(args); 

    if (CountPIPEPointer != NULL)
    {
        piping  = 1;
        *CountPIPEPointer = NULL;
        pipe(pipeFD);
        cpid = fork();
        if (cpid == -1)
        {
               perror("fork");
               exit(EXIT_FAILURE);
        }
        else if (cpid == 0)
        {
            close(pipeFD[1]); // Reader will see EOF 
            close(0);
            dup(pipeFD[0]); //Duplicate FD, returning a new file descriptor on the same file
            execute(CountPIPEPointer + 1);
            exit(0);
        }

        DuplicateFD = dup(STDOUT_FILENO); //Duplicate FD, returning a new file descriptor on the same file.
        dup2(pipeFD[1], STDOUT_FILENO); //Duplicate FD to FD2, closing FD2 and making it open on the same file.

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
        if(commands.size>0){
        strcpy(currentCommand, lastCommand);
        split(currentCommand);
        execute(argv);
        }
        else{
            printf("The command history list is empty");
        }

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
            if (*echo_var!=NULL && *echo_var[0] == '$')
            {
                Node *node = variables.head;
                char *new_variable = NULL;
                
                while (node)
                {
                    if (!strcmp(((Var *)node->data)->key,*echo_var ))
                    {
                        new_variable=((Var *)node->data)->value;
                    }
                    node = node->next;
                }
                if (new_variable != NULL)
                    printf("%s ", new_variable);
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
        ProccesID = -1;
        wait(&status);
        rv = status;  
    }

    if (piping !=0 )
    {
        close(STDOUT_FILENO);
        dup(DuplicateFD);
        close(pipeFD[1]);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    mainProcess = getpid(); //Get the process ID of the calling process.
    signal(SIGINT, termination_handler); //SIGINT  interrupt from keyboard (ctrl-c)
    strcpy(prompt, "hello: ");
    last_command=0;
    char* prevCommand=malloc(sizeof(char) * 1024);
    struct termios old_terminal, new_terminal;
    tcgetattr(STDIN_FILENO, &old_terminal);

    while (1)
    {
        memset(command, 0, sizeof(command));
        new_terminal = old_terminal;
        new_terminal.c_lflag &= ( ECHOE | ~ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal);

        //http://www.java2s.com/Tutorial/C/0080__printf-scanf/bMovesthecursortothelastcolumnofthepreviousline.htm
        printf("\r");
        printf("%s", prompt);
        i=1;
        c=getchar();
        if(c== 127 || c == '\b')
        {
            if(i<strlen(prompt))
            {
                printf("\b\b\b   \b");
                }
                continue;
        }
        else if (c == '\033')
		{
            printf("\33[2K");
			a=getchar(); //skip the [
            b=getchar();
			switch(b) { 
			case 'A':
                if (commands.size==0)
                {
                    break;
                }
                else if (last_command>0)
                {
                    last_command--;
                }
                
                printf("\b");
                printf("\b");
                printf("\b");
                printf("\b");
            
                prevCommand=(char *)get_command(&commands, last_command);
                printf("%s", (char *)get_command(&commands, last_command));

                break;
			case 'B':
                if (commands.size==0 ||last_command >= commands.size-1 )
                {
                    if(last_command == commands.size-1){
                        last_command++;
                    }
                    break;
                }
                else 
                {
                    last_command++;
                }
                
                printf("\b");
                printf("\b");
                printf("\b");
                printf("\b");
                prevCommand=(char *)get_command(&commands, last_command);
                printf("%s", (char *)get_command(&commands, last_command));
                break;

    		}
        command[0]=c;
        continue;
        }

        else if (c == '\n')
        {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
        if(commands.size>0){
        prevCommand=(char *)get_command(&commands, last_command);
        new_command2= malloc(sizeof(char) * strlen(prevCommand));
        prevCommand[strlen(prevCommand)]=' ';
        strcpy(new_command2, prevCommand);
        add(&commands, new_command2);
        last_command = commands.size;
        strcpy(command, new_command2);
        split(command);
        status = change_status(argv);
        }
        continue;
        }
        else{
        tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
        memset(command, 0, 1024);
        command[0]=c;
        char b;
        int Flag=0;
        i=1;
        
        while((b = getchar()) != '\n')
        {  
            if(b == 127 || b=='\b')
            {
                printf("\b\b\b   \b\b\b");
                // command[i] = '\0';
                i--;
                           
            }
            else 
            {
                command[i] = b; i++;
            };
               
        }
        command[i] = b;
        i++;
        command[strlen(command)]='\0';
        i=1;
        }
        

    int flag=0;
    //https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus
    if (!strncmp(command, "if", 2)) {
        flag=1;
        while (1) {
            fgets(current_command, 1024, stdin);
            strcat(command, current_command);
            command[strlen(command) - 1] = '\n';
            if (!strcmp(current_command, end_if))
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
            strcat(command, current_command);
        }

        command[strlen(command) - 1] = '\0';
        if (!strcmp(command, "quit")){
            // tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
            exit(0);
        }
        
        if (strcmp(command, "!!")){
            tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
            strcpy(lastCommand, command);
            // printf("%s",lastCommand);
        }

        new_command= malloc(sizeof(char)*strlen(command));
        strcpy(new_command, command);
        add(&commands, new_command);
        last_command = commands.size;
        if(!flag){
        split(command);
        status = change_status(argv);
        }
        else{
            continue;
        }
        
        }
    }
