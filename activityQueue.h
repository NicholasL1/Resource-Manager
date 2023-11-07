/**
 *  activityQueue.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Queue data structure to hold activities in the resource manager.
 *
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "activity.h"

// Node structure for the activityQueue
typedef struct QueueNode
{
  Activity *data; // Pointer to an Activity
  struct QueueNode *next;
} QueueNode;

// Activity Queue structure
typedef struct Queue
{
  QueueNode *front;
  QueueNode *rear;
  int size; // New field to track the size of the queue
} Queue;

// Function prototypes
Queue *createActivityQueue();
int isEmptyActivityQueue(Queue *activityQueue);
int getSizeActivityQueue(Queue *activityQueue);
void enqueueActivity(Queue *activityQueue, Activity *activity);
Activity *dequeueActivity(Queue *activityQueue);
void destroyActivityQueue(Queue *activityQueue);

#endif