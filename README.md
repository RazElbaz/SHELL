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
•shell1.c -prints a cursor and runs commands with arguments.  
•shell2.c – adds routing to the file.  
•shell3.c pipe adds   
•key.c  

The codes I wrote to accomplish the task:
**linkedlist.h and linkedlist.c** 

creating a library for creating variables:
*Var* - is designed to save new variables like in the real shell with key and value  
*Node*- to create a variable of a doubly linked list with next and prv, in addition it has a data variable.  
*List*- creating a doubly linked list whose fields are data and head, tail.  

# The task  
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


```ECHO <message>```
In C, the hyphen (-) is a common operator used for subtraction, and it can also be used to negate a value.

The en-dash (–) is a character that is not used as an operator in C. In fact, the en

dash is not even included in the ASCII character set, which is a set of characters commonly used in C programming. Instead, the ASCII hyphen character (-) is used for all subtraction and negation operations in C.

It's important to use the correct character when writing code in C, as using the wrong character can result in syntax errors or unexpected behavior.
