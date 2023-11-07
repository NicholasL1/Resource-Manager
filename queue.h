/**
 *  queue.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Queue data structure to hold tasks in the resource manager.
 *
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"

// Node structure for the queue
typedef struct QueueNode
{
  Task *data;
  struct QueueNode *next;
} QueueNode;

// Queue structure
typedef struct Queue
{
  QueueNode *front;
  QueueNode *rear;
  int size;
} Queue;

// Function prototypes
Queue *createQueue();
int isEmpty(Queue *queue);
int getSize(Queue *queue);
void enqueue(Queue *queue, Task *task);
Task *dequeue(Queue *queue);
void destroyQueue(Queue *queue);

#endif