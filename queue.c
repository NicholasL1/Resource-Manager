/**
 *  queue.c
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Queue data structure to hold tasks in the resource manager.
 *
 */

#include <stdlib.h>
#include "queue.h"

// Create a new queue
Queue *createQueue()
{
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  if (queue == NULL)
  {
    return NULL; // Memory allocation failed
  }
  queue->front = queue->rear = NULL;
  queue->size = 0;
  return queue;
}

// Check if the queue is empty
int isEmpty(Queue *queue)
{
  return (queue->front == NULL);
}

// Get the size of the queue
int getSize(Queue *queue)
{
  return queue->size;
}

// Add a new task to the queue
void enqueue(Queue *queue, Task *task)
{
  QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
  if (newNode == NULL)
  {
    return; // Memory allocation failed
  }
  newNode->data = task;
  newNode->next = NULL;

  if (isEmpty(queue))
  {
    queue->front = queue->rear = newNode;
  }
  else
  {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }

  queue->size++;
}

// Remove and return the front task from the queue
Task *dequeue(Queue *queue)
{
  if (isEmpty(queue))
  {
    return NULL; // Queue is empty
  }
  QueueNode *temp = queue->front;
  Task *task = temp->data;
  queue->front = temp->next;
  free(temp);
  queue->size--;
  return task;
}

// Destroy the queue and free memory
void destroyQueue(Queue *queue)
{
  while (!isEmpty(queue))
  {
    dequeue(queue);
  }
  free(queue);
}