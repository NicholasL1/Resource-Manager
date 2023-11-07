/**
 *  list.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  A list data structure to hold activities in the resource manager.
 *
 */

#ifndef LIST_H
#define LIST_H

#include "activity.h"

// Node structure for the linked list
typedef struct ListNode
{
  Activity *data;
  struct ListNode *next;
} ListNode;

// List structure
typedef struct List
{
  ListNode *head;
  int size;
} List;

// Function prototypes
List *createList();
int isEmpty(List *list);
int getSize(List *list);
void appendToList(List *list, Activity *activity);
void removeFromList(List *list, Activity *activity);
void removeAtIndex(List *list, int index);
void removeByTaskNumber(List *list, int task_number);
void clearList(List *list);
void destroyList(List *list);

#endif