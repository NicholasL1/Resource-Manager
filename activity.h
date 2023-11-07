/**
 *  activity.h
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Representation of a activity in the system.
 *
 */

#ifndef ACTIVITY_H
#define ACTIVITY_H

// representation of a a process
typedef struct Activity
{
  char *name;
  int task_number;
  int resource_type;
  int unit_amount;
} Activity;

#endif