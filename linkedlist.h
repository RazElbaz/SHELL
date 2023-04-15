typedef struct Var
{
    char *key;
    char *value;
} Var;

//https://www.programiz.com/dsa/doubly-linked-list
/*
A doubly linked list is a type of linked list in which each node consists of 3 components:
*prev - address of the previous node
data - data item
*next - address of next node
*/
typedef struct Node
{
  Var* data;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct List
{
  int size;
  struct Node *head;
  struct Node *tail;
} List;

void add(List *list, void *data);
void sort(List *list);
void printList(List *list);
void *get_command(List *list, int index);