/**
 *  task.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Representation of a task in the system.
 *
 */

#ifndef PROCESS_H
#define PROCESS_H

// representation of a a process
typedef struct Task
{
  int initial_claim;
  int currently_allocated;
} Task;

#endif