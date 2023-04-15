#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist.h"
//https://www.programiz.com/dsa/doubly-linked-list

//display the list
void printList(List *list) {
   	if (!list || list->size == 0)
	{
		return;
	}
   struct Node *ptr = list->head;
   printf("\n[ ");
	
   //start from the beginning
   while(ptr != NULL) {
      printf("(%s,%s) ",ptr->data->key,ptr->data->value);
      ptr = ptr->next;
   }
	
   printf(" ]");
}

void sort(List *list) {
	if (!list || list->size == 0)
	{
		return;
	}
   char* tempKey;
   char* tempData;
   int i, j, k;
   struct Node *current=(Node *)malloc(sizeof(Node));
   struct Node *next=(Node *)malloc(sizeof(Node));
   tempData=malloc(sizeof(char *)*1024);
   tempKey=malloc(sizeof(char *)*1024);
   int size = list->size;
   k = size ;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = list->head;
      next = list->head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data->value > next->data->value ) {
			strcpy(tempData,current->data->value);
			strcpy(current->data->value , next->data->value);
			strcpy(next->data->value,tempData);
			strcpy(tempKey,current->data->value);
			strcpy(current->data->value,next->data->value);
			strcpy(next->data->value , tempKey);
         }
			
         current = current->next;
         next = next->next;
      }
   }   
}

void add(List *list, void *data){
if (!list)
	{
		return;
	}
	Node *node = (Node *)malloc(sizeof(Node));
	node->next = NULL;
	node->prev = list->tail;
	node->data = data;
	list->size++;
	if (!list->head)
	{
		list->head = list->tail = node;
		return;
	}

	list->tail->next = node;
	list->tail = node;
}

void *get_command(List *list, int index)
{
	if (!list || !list->head )
	{
		return NULL;
	}
	int i = 0;
	Node *node = list->head;
	while (i < index)
	{
		node = node->next;
		i++;
	}
	return node->data;
}