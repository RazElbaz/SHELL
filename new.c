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
        /*
        In C, strtok is a function used to tokenize a given string based on a specified delimiter. When the strtok function is called with the first argument as NULL, it continues tokenizing the same string from where it left off during the last call.
        So strtok(NULL, delim) is used to retrieve the next token in the string that was not processed by the previous call to strtok, where delim is the delimiter used for tokenizing the string.
        */
        part_of_command = strtok(NULL, " ");
        i++;
    }
    argv[i] = NULL;
}

//https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
void termination_handler(int signum)
{
/**
Task number: 8
If the user typed C-Control, the program will not finish but will print the
The message:
You typed Control-C!
If the SHELL runs another process, the process will be thrown by the system)default behavior(
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
        fprintf(stderr, "\ncaught signal: %d\n", signum);
        kill(mainProcess, SIGKILL); //If the SHELL runs another process, the process will be thrown by the system
        return;
    }
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
    /*
    Task number: 9
    Option to chain several commands in a pipe.
    For each command in pipe a dynamic allocation of argv is needed
    */
    if (CountPIPEPointer != NULL)
    {
        *CountPIPEPointer = NULL;
        piping++;
        
        /*
        In C, pipe(pipeFD) is a system call that creates a new pipe, which is a unidirectional data channel that allows data to be passed between two processes. The pipeFD parameter is a pointer to an integer array that will be used to store the file descriptors for the read and write ends of the pipe.
        */
        int result=pipe(pipeFD);
        if (result == -1) {
        // Error occurred
        printf("Failed to create pipe\n");
        exit(EXIT_FAILURE);
        }


        cpid = fork();

        /*
        the fork() system call can return -1 if it fails to create a new process. This can happen if the system is unable to allocate resources such as memory or if the maximum number of processes allowed per user or per system has already been reached.
        When fork() returns -1, it means that the child process was not created, and the parent process should handle the error appropriately.
        */
        if (cpid == -1)
        {       
            // Error occurred
            perror("Error occurred: Failed to create new process\n");
            exit(EXIT_FAILURE);
        }

        /*
        When the fork() system call is successful and creates a new process, it returns a value of 0 in the child process. This indicates to the child process that it is a new process, separate from the parent process.
        */
        // Child process
        else if (cpid == 0)
        {
            /*
            In C, close(pipeFD[1]) is a system call that closes the file descriptor pipeFD[1], which is the file descriptor for the write end of a pipe. When a process no longer needs a file descriptor, it should close it to free up system resources and avoid resource leaks.
            */
            int status =close(pipeFD[1]); // Reader will see EOF 
            if (status == -1) {
            // Error occurred
            printf("Failed to close file descriptor\n");
            exit(EXIT_FAILURE);
            } 

            /*
            In C, close(0) is a system call that closes the standard input file descriptor, which has a value of 0. This is typically associated with the keyboard or another input device.
            When you close the standard input file descriptor, any subsequent attempts to read from it will fail. This is usually done when you want to redirect input from a file or another process to replace the standard input.
            */
            int status_file =close(0);
            if (status_file == -1) {
            // Error occurred
            printf("Failed to close standard input file descriptor\n");
            exit(EXIT_FAILURE);
            }

            /*
            In C, dup(pipeFD[0]) is a system call that duplicates the file descriptor pipeFD[0], which is the file descriptor for the read end of a pipe. The dup() system call creates a new file descriptor that refers to the same open file description as the original file descriptor. This new file descriptor can be used to read data from the pipe.
            */
            int newFD =dup(pipeFD[0]); //Duplicate FD, returning a new file descriptor on the same file
            if (newFD == -1) 
            {
            // Error occurred
            printf("Failed to duplicate file descriptor\n");
            exit(EXIT_FAILURE);
            } 
            execute(CountPIPEPointer + 1);
            exit(0);
        }

        DuplicateFD = dup(STDOUT_FILENO); //Duplicate FD, returning a new file descriptor on the same file.
        if (DuplicateFD == -1) {
            // Error occurred
            printf("Failed to duplicate file descriptor\n");
            exit(EXIT_FAILURE);
        }
        // Redirect output to write end of pipe
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

    int redirect = -1;
    /*
    Task number: 1
    Redirect writes to stderr
    hello: ls –l nofile 2> mylog
    Adding to an existing file by >>
    hello: ls -l >> mylog
    As in a normal shell program, if the file does not exist, it will be created.
    */
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

    /*
    Task number: 2
    Command to change the cursor:
    hello : prompt = myprompt
    (The command contains three words separated by two spaces)
    */
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
        /*
        Task number: 4
        The command: hello: echo $?
        Print the status of the last command executed.
        */
        if (strcmp(*echo_var, "$?")==0)
        {
            printf("%d\n", status);
            return 0;
        }

        while (*echo_var)
        {
            
            if (echo_var != NULL && echo_var[0][0] == '$')
            {
                /*
                Task number: 3
                ---Part A of task number: 3---
                echo command that prints the arguments:
                hello: echo abc xyz
                will print
                abc xyz
                Here is the search for the variable $var in the list of variables
                */
                Node *node = variables.head;
                char *new_variable = NULL;

                /*
                We will go through the list of saved variables so that if a defined variable is found it will be printed
                */
                while (node)
                {
                    if (strncmp(((Var *)node->data)->key, *echo_var, strlen(*echo_var)) == 0)
                    {
                        new_variable=((Var *)node->data)->value;
                    }
                    node = node->next;
                }
                if (new_variable != NULL)
                    printf("%s ", new_variable);
            }

            else{
            /*
            Task number: 3
            ---Part B of task number: 3---
            echo command that prints the arguments:
            hello: echo abc xyz
            will print
            abc xyz
            */
            printf("%s ", echo_var[0]);
            }

            echo_var++;
        }
        printf("\n");
        return 0;
    }

    else
        amper = 0;
    
    /*
        Task number: 5
        A command that changes the shell's current working directory:
        hello: cd mydir
        */
    if (strcmp(args[0], "cd")==0)
    {
        if (chdir(args[1]) != 0){ // chdir is a system call.
        printf(" %s: no such directory\n", argv[1]);
    
    }
        return 0;
    }
    /* 
    Task number:  6
    A command that repeats the last command:
    hello: !!
    (two exclamation marks in the first word of the command)
    * I concluded that this was the last command typed in the terminal
    */
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
    /*
    Task number: 10
    Adding variables to the shell:
    hello: $person = David
    hello: echo person
    person
    hello: echo $person
    David
    */
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
    /*
    Task number: 11
    read command:
    hello: echo Enter a string
    read name
    Hello
    echo $name
    Hello
    Save a variable as $
    */
    if(strcmp(args[0], "read")==0)
    {
        Var *var = (Var *)malloc(sizeof(Var));
        var->key = malloc(sizeof(char) * (strlen(args[1])));
        var->value = malloc(sizeof(char) * 1024);
        var->key[0] = '$';
        memset(var->value, 0, 1024);
        strcpy(var->key + 1, args[1]);
        fgets(var->value, 1024, stdin);
        /*
        In C, strings are represented as arrays of characters terminated by a null character '\0'. When you receive input from the user, it is stored in a character array. 
        If you don't add a null character at the end of the array, any string functions that operate on that array will not know where the end of the string is and may read past the end 
        of the array,causing unexpected behavior or even crashes.
        */
        var->value[strlen(var->value) -1] = '\0';
        add(&variables, var);
        return 0;
    }


    /* for commands not part of the shell command language */ 
    // Fork a new process to execute the command
    ProccesID = fork();

    /*
    Continue task number: 1
    Redirect writes to stderr
    hello: ls –l nofile 2> mylog
    Adding to an existing file by >>
    hello: ls -l >> mylog
    As in a normal shell program, if the file does not exist, it will be created.
    */
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
            dup(fd); //Duplicate FD, returning a new file descriptor on the same file.
            close(fd);
            args[i - 2] = NULL;
            // close(STDOUT_FILENO);
        }
        // Child process: execute the command
        int status_code=execvp(args[0], args);
        if (status_code == -1) {
                // Error executing the command
                printf("Unknown command: %s\n", command);
                exit(1);
            }
    }
    else if (ProccesID == -1) {
            // Error forking a new process
            printf("Error forking a new process\n");
            exit(1);
        }
    /* parent continues here */
    else {
            
        /**
         * In C, the process ID -1 is a special value that represents an error condition. This value is typically used in system calls to indicate that the call failed, and the specific error code can be retrieved using the errno global variable.
        */
        ProccesID = -1;

        
        /**
         * wait(&status) system call is used when the parent process needs to know the exit status of the child process. When wait(&status) is called, the parent process will block until any child process terminates, and the exit status of the child process is stored in the status variable.
        */
        wait(&status); // Parent process: wait for the child process to finish
        rv = status;  
    }

    if (piping !=0 )
    {
        /*
        In C, when piping is used, the dup2() system call is often used to redirect the output of one process to the input of another process. When this redirection is performed, it is necessary to close the file descriptors that are no longer needed.
        In particular, it is important to close the STDOUT_FILENO file descriptor (which represents standard output) in the process that is sending output to the pipe. This is because, after the dup2() call, the file descriptor representing the writing end of the pipe is now associated with STDOUT_FILENO. If STDOUT_FILENO is not closed, any subsequent write operations that use this file descriptor will actually be sent to the pipe instead of to the original standard output, which could cause unexpected behavior and output.
        In other words, closing STDOUT_FILENO ensures that the process only sends output to the writing end of the pipe, and not to any other file descriptor that may be associated with standard output.
        */
        int status = close(STDOUT_FILENO);

        if (status == -1) {
            // Error occurred
            printf("Failed to close standard output file descriptor\n");
            exit(EXIT_FAILURE);
        } 

        /*
        the dup() function is used in piping to duplicate the file descriptor of the pipe onto the standard input or output file descriptors of the respective processes, thereby enabling inter-process communication through the pipe.
        In C, dup(DuplicateFD) is a system call that duplicates the file descriptor specified by DuplicateFD, and returns a new file descriptor that is the lowest numbered file descriptor not currently open for the process.
        The new file descriptor returned by dup() points to the same open file description as the file descriptor specified by DuplicateFD. This means that any operations performed on the new file descriptor will affect the same file as the original file descriptor.
        */
        int newFD =dup(DuplicateFD);
        if (newFD == -1) {
            // Error occurred
            printf("Failed to duplicate file descriptor\n");
            exit(EXIT_FAILURE);
        } 

        /*
        In C, when piping is used, it is important to close the file descriptors that are no longer needed to prevent potential issues such as deadlocks or resource leaks.
        In particular, after the dup2() function is used to redirect the output of one process to the input of another process, it is important to close the write end of the pipe (i.e. pipeFD[1]) in the parent process. This is because the parent process no longer needs to write to the pipe, and keeping the write end open can cause problems, such as the child process blocking indefinitely on a read call waiting for data that will never come.
        Closing the write end of the pipe in the parent process (close(pipeFD[1])) ensures that the child process will eventually receive an end-of-file indication when it attempts to read from the pipe, allowing it to gracefully exit once it has processed all of the data.
        In summary, closing the write end of the pipe after the dup2() function is used helps prevent issues such as deadlocks, resource leaks, and other unintended behavior that can occur when file descriptors are not properly managed in a multi-process environment.
        */
        int status_pipeFD =close(pipeFD[1]); // Reader will see EOF 
            if (status_pipeFD == -1) {
            // Error occurred
            printf("Failed to close file descriptor\n");
            exit(EXIT_FAILURE);
        } 


        /*
        the wait() system call in the parent process ensures that the child process has completed its execution and any resources associated with it are properly cleaned up before the parent process exits. This helps prevent issues such as resource leaks and other unintended behavior that can occur when processes are not properly managed in a multi-process environment.
    
        In C, wait(NULL) is a system call that suspends the execution of the calling process until one of its child processes terminates. When a child process terminates, its exit status is collected by the parent process using the wait() system call.
        The wait(NULL) system call is used when the parent process does not need to know the specific exit status of the child process. When wait(NULL) is called, the parent process will block until any child process terminates, and the exit status of the child process is discarded.
        */
        wait(NULL);
    }

    /*
    In C, the main() function of a program is required to return an integer value to indicate the status of the program to the operating system. This value is commonly referred to as the "return value" or "exit status" of the program.
    When a C program is executed in a terminal, the shell that is running in the terminal will display the return value of the program after it exits. This is often represented as rv or retval in the terminal, followed by the integer value that was returned by the program.
    */
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
            /*
            Task number: 12
            Memory of the last commands (at least 20) Option to browse by
            Arrows: "up" and "down"
            (as in the real SHELL)
            */
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
        if(commands.size>0){ //
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
                printf("\b \b");
                // command[i] = '\0';
                i--;
                           
            }
            else 
            {
                command[i] = b; i++;
            };
               
        }
        //adding '\n'
        command[i] = b;

        /*
        In C, strings are represented as arrays of characters terminated by a null character '\0'. When you receive input from the user, it is stored in a character array. 
        If you don't add a null character at the end of the array, any string functions that operate on that array will not know where the end of the string is and may read past the end 
        of the array,causing unexpected behavior or even crashes.
        */
        i++;
        command[strlen(command)]='\0';

        //Resetting the index
        i=1;
        }


    int if_flag=0;
    //https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus
    if (!strncmp(command, "if", 2)) {
        /*
        Task number: 13
        Support for flow control, i.e. ELSE/IF. For example:
        if date | grep Fri
        then
        echo "Shabbat Shalom"
        else
        echo "Hard way to go"
        fi
        Typing the condition will execute line by line, as shown here.
        */
        if_flag=1;
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

        /*
        In C, strings are represented as arrays of characters terminated by a null character '\0'. When you receive input from the user, it is stored in a character array. 
        If you don't add a null character at the end of the array, any string functions that operate on that array will not know where the end of the string is and may read past the end 
        of the array,causing unexpected behavior or even crashes.
        */
        command[strlen(command) - 1] = '\0';

        if (!strcmp(command, "quit")){
            /*
            Task number: 7
            command to exit the shell:
            hello: quit
            */
            // tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
            exit(0);
        }
        
        if (strcmp(command, "!!")){
            tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
            strcpy(lastCommand, command);
            // printf("%s",lastCommand);
        }
        

        //adding the new command to the command list
        new_command= malloc(sizeof(char)*strlen(command));
        strcpy(new_command, command);
        add(&commands, new_command);

        //We will update the last command index to be the updated size
        last_command = commands.size;

        //if the last command is "if" we dont want to run this command again(Because we already ran it after it was called)
        if(!if_flag){
        split(command);
        status = change_status(argv);
        }
        else{
            continue;
        }
        
        }
    }