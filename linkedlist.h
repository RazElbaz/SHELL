typedef struct Var
{
    char *key;
    char *value;
} Var;

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
// void sort(List *list);
void printList(List *list);
void *get_command(List *list, int index);