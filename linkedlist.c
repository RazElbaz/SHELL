#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist.h"

// //display the list
// void printList(List *list) {
//    	if (!list || list->size == 0)
// 	{
// 		return;
// 	}
//    struct Node *ptr = list->head;
//    printf("\n[ ");
	
//    //start from the beginning
//    while(ptr != NULL) {
//       printf("(%d,%d) ",ptr->data,ptr->data);
//       ptr = ptr->next;
//    }
	
//    printf(" ]");
// }

// void sort(List *list) {
// 	if (!list || list->size == 0)
// 	{
// 		return NULL;
// 	}
//    int i, j, k, tempKey, tempData;
//    struct Node *current=(Node *)malloc(sizeof(Node));;
//    struct Node *next=(Node *)malloc(sizeof(Node));;
	
//    int size = length();
//    k = size ;
	
//    for ( i = 0 ; i < size - 1 ; i++, k-- ) {
//       current = list->head;
//       next = list->head->next;
		
//       for ( j = 1 ; j < k ; j++ ) {   

//          if ( current->data > next->data ) {
//             tempData = current->data;
//             current->data = next->data;
//             next->data = tempData;

//             tempKey = current->data;
//             current->data = next->data;
//             next->data = tempKey;
//          }
			
//          current = current->next;
//          next = next->next;
//       }
//    }   
// }

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