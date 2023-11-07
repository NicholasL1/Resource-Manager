/**
 *  activityQueue.c
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Queue data structure to hold activities in the resource manager.
 *
 */

#include <stdlib.h>
#include "activity.h"
#include "activityQueue.h"

// Create a new activityQueue
Queue *createActivityQueue()
{
  Queue *activityQueue = (Queue *)malloc(sizeof(Queue));
  if (activityQueue == NULL)
  {
    return NULL; // Memory allocation failed
  }
  activityQueue->front = activityQueue->rear = NULL;
  activityQueue->size = 0; // Initialize the size to 0
  return activityQueue;
}

// Check if the activityQueue is empty
int isEmptyActivityQueue(Queue *activityQueue)
{
  return (activityQueue->front == NULL);
}

// Get the size of the activityQueue
int getSizeActivityQueue(Queue *activityQueue)
{
  return activityQueue->size;
}

// Add a new activity to the activityQueue
void enqueueActivity(Queue *activityQueue, Activity *activity)
{
  QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
  if (newNode == NULL)
  {
    return; // Memory allocation failed
  }
  newNode->data = activity;
  newNode->next = NULL;

  if (isEmptyActivityQueue(activityQueue))
  {
    activityQueue->front = activityQueue->rear = newNode;
  }
  else
  {
    activityQueue->rear->next = newNode;
    activityQueue->rear = newNode;
  }

  activityQueue->size++; // Increase the size
}

// Remove and return the front activity from the activityQueue
Activity *dequeueActivity(Queue *activityQueue)
{
  if (isEmptyActivityQueue(activityQueue))
  {
    return NULL; // Queue is empty
  }
  QueueNode *temp = activityQueue->front;
  Activity *activity = temp->data;
  activityQueue->front = temp->next;
  free(temp);
  activityQueue->size--; // Decrease the size
  return activity;
}

// Destroy the activityQueue and free memory
void destroyActivityQueue(Queue *activityQueue)
{
  while (!isEmptyActivityQueue(activityQueue))
  {
    dequeueActivity(activityQueue);
  }
}