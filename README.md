# SHELL
## written by ✨

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/RazElbaz"><br /><sub><b>Raz Elbaz</b></sub></a><br /> </td>
  </tr>
</table>

</table>

# Introduction:
The main goal of this assignment is to create a shell.

# Explanation of the codes found in the project:

In the assignment folder we received there were three files for the shell program:  
•shell1.c - prints a cursor and runs commands with arguments.  
•shell2.c – adds routing to the file.  
•shell3.c - pipe adds   
•key.c  

### The codes I wrote to accomplish the task:
•**new.c**  
The program that implements the assignment  
•**Examples_to_run.txt**  
A text file that contains examples that can be run in the program.  
•**Examples_from_running_my_program.txt**  
A text file that contains examples that I ran in my program as proof that everything is working properly.  
•**linkedlist.h and linkedlist.c**  
Two programs that create a library for new variables to implement the task:
*Var* - is designed to save new variables like in the real shell with key and value  
*Node*- to create a variable of a doubly linked list with next and prv, in addition it has a data variable.  
*List*- creating a doubly linked list whose fields are data and head, tail.      
•**makefile**   
In C programming, a Makefile is a script or file that contains a set of instructions for compiling and building a program from source code files. The Makefile specifies the dependencies between the different source code files and the rules for how to build the program, including the compiler options, libraries to link, and other build-time options.

The Makefile is used with the make utility, which reads the instructions in the Makefile and executes the necessary steps to build the program. The make utility determines which files need to be recompiled based on their dependencies and the modification time of each file.

### Assumptions I made:
1. I assumed that the command ```!!``` Need to re-execute the last command typed.  
2. I assumed that the command ```cat > file``` would be executed as in the real Tremil: first typing the command → pressing Enter → typing the desired text → pressing Control-D as I read on many websites and among other things also on these websites:  
https://www.hostinger.com/tutorials/linux-cat-command-tutorial-and-examples/
https://linuxize.com/post/linux-cat-command/  
3. I assumed that if an if command was typed then it was typed correctly with fi at the end, and typed line by line as written in the assignment . Therefore, after typing the command you can press the up arrow ↑ or alternatively type !! to see the command but from these options you cannot execute the command again (you can see with the arrows but not run with the enter arrow or with !! , because it was defined that way in the instructions, the '\n' character will cause errors).

-----------
##  How To Run
1. Compile files first with ```make``` or ```make all``` in terminal.
2. Run with ``./myshell```
3. Enjoy my shell 🙂.
-----------
## My shell support the following commands: 
arrows: :arrow_up: or :arrow_down:  ```ls``` ```ls -l```  ```ls -a```  ```ls -h```  ```ls -t```  ```ls -R``` ```prompt = new_prompt:``` ```!!``` ```text >> file```  ```text > file```  ```text 2> file```  ```cat > file``` ```date``` ```date -u``` ```ls -S```  ```ls -r``` ```ls -u``` ```ls -1``` ```ls -m``` ```ls -C``` ```echo text``` ```echo $?``` ```pwd``` ```quit ``` ```rm file``` ```cat file``` ``` | (piping) ``` ```wc -l < myfile``` ```ls -l nofile 2> mylog (for errors)``` ```Control-C```
```sort file.txt``` ```uniq -c file.txt``` ```head -3 file.txt``` ```cd``` ```cd ..``` ```mkdir date dir ```
```
$var = David
echo $var
```
```
read name
hello
echo $name
```  
``` 
cat > file
**press Enter**
some text
**press Control-D**
```  
```
if condition 
then  
   // code to be executed if condition is true
else  
  // code to be executed if condition is false
fi  //to finish typing the if command
```

And much more, feel free to try

### Running examples (from the file: Examples_to_run.txt )
```date >> myfile```    
```cat myfile```  
```date -u >> myfile```  
```cat myfile```  
```wc -l < myfile```  
```ls -l nofile 2> mylog```  
```cat mylog```  
```ls -l >> mylog```  
```cat mylog```  
```prompt = hi:```  
```mkdir mydir```  
```cd mydir```  
```pwd```  
```touch file1 file2 file3```  
```ls```  
```!!```  

```prompt = show_echo:```  
```echo abc xyz```  
```ls```  
```echo $?```  
```ls no_such_file```  
```echo $?```  
```ls no_such_file 2> file```  
```cat file```  
```Control-C```  

```prompt = show_variables:```  
```$person = David```  
```echo person```  
```echo $person```  
```read name```  
```hello```    
```echo name```  
```echo $name```  

```prompt = show_if:```  

```
if date | grep Sat  
then  
 echo "Shabat Shalom"  
else  
 echo "Hard way to go"  
fi  
```  
```
if date | grep Sat
then
 ls 
fi
``` 



```prompt = show_pipe: ```  
```cat > colors.txt```  
```
blue  
black  
red  
red  
green  
blue  
green  
red  
red  
blue  
```  
```Control-D```  

```cat colors.txt```  
```cat colors.txt | cat | cat | cat```  
```sort colors.txt | uniq -c | sort -r | head -3```  
```ls | wc -l```  
```ls | tee list```  
```ls -l | wc -l```  

```prompt = show_more_commands:```  
```mkdir date_dir```  
```cd date_dir```  
```pwd```  
```cd ..```  
```pwd```  
```cd date_dir```  
```touch date_file```  
```ls```  
```rm date_file```  
```ls```  
```sort colors.txt```  
```uniq -c colors.txt  ```  
```colors.txt head -3```  
```head -3 colors.txt```   
arrows: :arrow_up: or :arrow_down: (like in the real shell)  
```quit```  

## The task  
You must add the following features (you can add functions to main :)  


1. Routing writing to stderr
hello: ls -l nofile 2> mylog
Adding to an existing file by >>
hello: ls -l >> mylog
As in a normal shell program, if the file does not exist, it will be created.
Built-in commands in the shell (placed before fork (placed before fork) and must be executed after them
:) continue  
2. Command to change the cursor:
hello : prompt = myprompt
(The command contains three words separated by two spaces)  
3. echo command that prints the arguments:
hello: echo abc xyz  
will print  
abc xyz
4. The command  
hello: echo $?
Print the status of the last command executed.  
5. A command that changes the current working folder of the shell:
hello: cd mydir  
6. A command that repeats the last command:
hello: !!  
(two exclamation marks in the first word of the command)  
7. Command to exit the shell:
hello: quit 
8. If the user typed C-Control, the program will not finish but will print the
The message:  
You typed Control-C!  
If the SHELL runs another process, the process will be thrown by the system  
(default behavior)  
9. Possibility to chain several commands in a pipe.  
For each command in pipe a dynamic allocation of argv is needed  
10. Adding variables to the shell:  
hello: $person = David  
hello: echo person  
person  
hello: echo $person  
David    
11. read command  
hello: echo Enter a string  
read name  
Hello  
echo $name  
Hello    
12. Memory of the last commands (at least 20) Possibility to browse by  
Arrows: "up" and "down"  
(as in the real SHELL)    
13. Support for flow control, ie ELSE/IF. For example:  
if date | grep Fri  
then  
  echo "Shabbat Shalom"  
else  
  echo "Hard way to go"  
fi    
Typing the condition will execute line by line, as shown here.  
After you have added the features, please run the following commands:  
./shell  
hello: date >> myfile  
hello: cat myfile  
hello: date -u >> myfile  
hello: cat myfile  
hello: wc -l < myfile  
hello: prompt = hi:  
hi: mkdir mydir  
hi: cd mydir  
hi: pwd  
hi: touch file1 file2 file3  
hi: ls  
hi: !!  
hi: echo abc xyz  
hi: ls  
hi: echo $?  
hi: ls no_such_file  
hi: echo $?  
hi: ls no_such_file 2> file  
hi: Control-C  
hi: cat > colors.txt  
blue  
black  
red  
red  
green  
blue  
green  
red  
red  
blue  
Control-D  
hi: cat colors.txt  
hi: cat colors.txt | cat cat cat  
hi: sort colors.txt | uniq -c | sort -r | head -3  
hi: quit  

Put ♥ in the task attached here task1.pdf in the definition of the instructions there is sometimes the sign (–) instead (-) and this may cause problems when it is typed in the terminal of the program for the following reason:
In C, the hyphen (-) is a common operator used for subtraction, and it can also be used to negate a value.

The en-dash (–) is a character that is not used as an operator in C. In fact, the en-dash is not even included in the ASCII character set, which is a set of characters commonly used in C programming. Instead, the ASCII hyphen character (-) is used for all subtraction and negation operations in C.

It's important to use the correct character when writing code in C, as using the wrong character can result in syntax errors or unexpected behavior.
