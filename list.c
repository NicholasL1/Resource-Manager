/**
 *  list.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  A list data structure to hold activities in the resource manager.
 *
 */

#include <stdlib.h>
#include "list.h"

// Create a new list
List *createList()
{
  List *list = (List *)malloc(sizeof(List));
  if (list == NULL)
  {
    return NULL; // Memory allocation failed
  }
  list->head = NULL;
  list->size = 0;
  return list;
}

// Check if the list is empty
int isEmpty(List *list)
{
  return (list->head == NULL);
}

// Get the size of the list
int getSize(List *list)
{
  return list->size;
}

// Append an activity to the end of the list
void appendToList(List *list, Activity *activity)
{
  ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
  if (newNode == NULL)
  {
    return; // Memory allocation failed
  }
  newNode->data = activity;
  newNode->next = NULL;

  if (isEmpty(list))
  {
    list->head = newNode;
  }
  else
  {
    ListNode *current = list->head;
    while (current->next != NULL)
    {
      current = current->next;
    }
    current->next = newNode;
  }

  list->size++;
}

// Remove an activity from the list
void removeFromList(List *list, Activity *activity)
{
  ListNode *current = list->head;
  ListNode *prev = NULL;

  while (current != NULL)
  {
    if (current->data == activity)
    {
      if (prev == NULL)
      {
        list->head = current->next;
      }
      else
      {
        prev->next = current->next;
      }

      free(current);
      list->size--;
      return;
    }

    prev = current;
    current = current->next;
  }
}

// Remove an element from the list at the specified index
void removeAtIndex(List *list, int index)
{
  if (index < 0 || index >= list->size)
  {
    return; // Invalid index
  }

  ListNode *current = list->head;
  ListNode *prev = NULL;
  int i = 0;

  while (current != NULL && i < index)
  {
    prev = current;
    current = current->next;
    i++;
  }

  if (prev == NULL)
  {
    // Removing the first element
    list->head = current->next;
  }
  else
  {
    prev->next = current->next;
  }

  free(current);
  list->size--;
}

// Remove an activity from the list based on task_number
void removeByTaskNumber(List *list, int task_number)
{
  ListNode *current = list->head;
  ListNode *prev = NULL;

  while (current != NULL)
  {
    if (current->data->task_number == task_number)
    {
      if (prev == NULL)
      {
        list->head = current->next;
      }
      else
      {
        prev->next = current->next;
      }

      free(current);
      list->size--;
      return; // Exit the function after removing the first matching activity
    }

    prev = current;
    current = current->next;
  }
}

// Clear the entire list
void clearList(List *list)
{
  while (!isEmpty(list))
  {
    removeFromList(list, list->head->data);
  }
}

// Destroy the list and free memory
void destroyList(List *list)
{
  clearList(list);
  free(list);
}