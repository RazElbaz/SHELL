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
pid_t ProccesID;
int last_command,fd, amper, rv ,piping , i;
char *argv[1024];
char *outfile;
int enter=0;
char a,b,c;
char **pipPointer; 
int pipeFD[2];
pid_t cpid;
char *token;
char *new_command;
char* new_command2;
char current_command[1024]; 
char* end_if="fi\n";

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
void split2(char *command)
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
//https://stackoverflow.com/questions/43295721/how-to-duplicate-a-child-descriptor-onto-stdout-fileno-and-stderr-fileno ->  fd
////https://man7.org/linux/man-pages/man2/pipe.2.html-> pipe
int execute(char **args)
{
    rv = -1;
    piping  = 0;
    i = CountARGS(args);
    char **pipPointer = CountPIPE(args); 

    if (pipPointer != NULL)
    {
        piping  = 1;
        *pipPointer = NULL;
        pipe(pipeFD);
        cpid = fork();
        if (cpid == -1)
        {
               perror("fork");
               exit(EXIT_FAILURE);
        }
        if (cpid == 0)
        {
            close(pipeFD[1]); // Reader will see EOF 
            close(0);
            dup(pipeFD[0]); //Duplicate FD, returning a new file descriptor on the same file
            execute(pipPointer + 1);
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

    if (piping)
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
//https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
void termination_handler(int signum)
{
    if (getpid() == mainProcess) {
        printf("\n");
        printf("You typed Control-C!");
        printf("\n");
        write(0, prompt, strlen(prompt)+1);
        return;
    }
    else{
        fprintf(stderr, "caught signal: %d\n", signum);
        return;
    }
}
char *safe_strcpy(char *dest, size_t size, char *src) {
    if (size > 0) {
        *dest = '\0';
        return strncat(dest, src, size - 1);
    }
    return dest;
}
int main()
{
    
    mainProcess = getpid();
    signal(SIGINT, termination_handler); //SIGINT  interrupt from keyboard (ctrl-c)
    strcpy(prompt, "hello: ");
    last_command=0;
    char* prevCommand=malloc(sizeof(char) * 1024);
    int j=0;
            struct termios originalTermios, newTermios;

            // Initialize termios structure to default values
    if (tcgetattr(STDIN_FILENO, &originalTermios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
     // Modify the behavior of the Delete key
    newTermios.c_lflag &= ( ECHOE | ~ICANON | VERASE);
    newTermios.c_cc[VERASE] = 0x8; // Send Backspace (ASCII code 8) instead of Delete (ASCII code 127)
    newTermios = originalTermios;
    // Apply the new terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

        // tcsetattr(STDIN_FILENO, &newTermios);
        
        // tcgetattr(0, &newTermios);
        // newTermios.c_cc[VERASE] = 0x08; 
            // Restore the original terminal attributes before exiting
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newTermios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        //http://www.java2s.com/Tutorial/C/0080__printf-scanf/bMovesthecursortothelastcolumnofthepreviousline.htm
        printf("\r");
        printf("%s", prompt);
        c=getchar();
        if(c== 127){
                if(i<strlen(prompt+1)){
                printf("\b\b\b   \b\b\b");
                }
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
                
                // printf("\r");
                prevCommand=(char *)get_command(&commands, last_command);
                printf("%s", (char *)get_command(&commands, last_command));
                // printf("\033[999C"); // Move the cursor to the end of the line
                

                // printf("%s %s %d", (char *)get_command(&commands, last_command),prevCommand,last_command);
// // printf("\n %ld\n",strlen(prevCommand));
// printf(" prev: %s",(prevCommand));
                break;
			case 'B':
                if (commands.size==0 ||last_command >= commands.size-1 )
                {
                    if(last_command == commands.size-1){
                        last_command++;
                        // continue;
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
                // printf("\r");
                // prevCommand=(char *)get_command(&commands, last_command);
                prevCommand=(char *)get_command(&commands, last_command);
                printf("%s", (char *)get_command(&commands, last_command));
                // printf("\r");
                // printf("\033[999C"); // Move the cursor to the end of the line
                // printf("prev: %s ",(prevCommand));
                break;

    		}
            command[0]=c;
         continue;
        }

        else if (c == '\n')
        {
        
        new_command2= malloc(sizeof(char) * strlen(prevCommand+4));
        prevCommand[strlen(prevCommand)]=' ';
        strcpy(new_command2, prevCommand);
        add(&commands, new_command2);
        last_command = commands.size;
        // split2((char *)get_command(&commands, last_command-1));
        // // //
        // command[0]=c;
        // fgets(command ,1023, stdin);
        strcpy(command, new_command2);
        split(command);
        status = change_status(argv);
        // execute(command);
        // printf("\n");
        continue;
        }
        else{
        command[0]=c;
        char str=0;
        char b;
        i=1;
        while((b = getchar()) != '\n'){
            if(b == 127 || b=='\b'){
                // if(i<strlen(prompt+1)){
                
                // }
                printf("\b\b\b   \b\b\b");
                command[i] = '\0';
                i--;
            }
                
            else {
                command[i] = b; i++;
             
                };
               
        }
        command[i] = b;
        printf("\n%s\n",command);
        }


    //https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus
    if (!strncmp(command, "if", 2)) {
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
            continue;
        }
        
        if (!strcmp(command, "quit")){
            command[strlen(command) - 1] = '\0';
            exit(0);
        }
            

        if (strcmp(command, "!!")){
            command[strlen(command) - 1] = '\0';
            strcpy(lastCommand, command);
        }
            
        
        new_command= malloc(sizeof(char) * strlen(command));
        strcpy(new_command, command);
        add(&commands, new_command);
        last_command = commands.size;
        split(command);
        status = change_status(argv);
        }
    }
