/**
 *  scheduler.c
 *
 *  Full Name: Nicholas Lachhman
 *  Course section: B
 *  Description of the program: Optimistic resource manager and banker's algorithm program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "activity.h"
#include "list.h"
#include "activityQueue.h"
#include "queue.h"

int finishedTasks;

int *totalTime;
int *timeWaited;
int **requestedUnits;
int *abortedTasks;
int *completedTasks;
int *computingTasks;

void optimistic_resource_manager(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[]);
void initiate_tasks(int num_of_tasks, int num_of_resources, Queue *task_activities[]);
void handleActivities(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int releasedUnits[], int timer);
void handleRequest(int index, int resource_type, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities);
int handleRelease(int index, Queue *task_activities[]);
void checkBlockedTasks(int resource_units[], List *blockedActivities, int blockedTaskIDs[]);
void updateBlockedTaskIDs(int num_of_tasks, int blockedTaskIDs[], List *blockedActivities);
void abortTasks(int num_of_tasks, Queue *task_activities[], List *blockedActivities, int resource_units[], int blockedTaskIDs[]);

int main(int argc, char *argv[])
{
  FILE *fp;

  argc = argc;

  int num_of_tasks;
  int num_of_resources;

  if (argc != 2)
  {
    printf("Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  fp = fopen(argv[1], "r");
  if (fp == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  // Read the number of tasks and the number of resources
  fscanf(fp, "%d %d", &num_of_tasks, &num_of_resources);

  // Create an array to store the units of each resource
  int resource_units[num_of_resources];

  // Read the units of each resource resource x units = num_of_resources[x-1]
  for (int i = 0; i < num_of_resources; i++)
  {
    if (fscanf(fp, "%d", &resource_units[i]) != 1)
    {
      printf("Error reading resource units from the file.\n");
      fclose(fp);
      return 1;
    }
  }

  // Create an array of queues to hold activities for each task
  Queue *task_activities[num_of_tasks];
  for (int i = 0; i < num_of_tasks; i++)
  {
    task_activities[i] = createActivityQueue();
  }

  char action[20];
  int task_number = 0;

  while (fscanf(fp, "%s %d", action, &task_number) == 2)
  {
    // Create an activity
    Activity *new_activity = (Activity *)malloc(sizeof(Activity));

    // Allocate memory for the name field and copy the action string
    new_activity->name = strdup(action);

    new_activity->task_number = task_number;
    fscanf(fp, "%d %d", &new_activity->resource_type, &new_activity->unit_amount);

    // Enqueue the new activity to the corresponding task's queue
    enqueueActivity(task_activities[task_number - 1], new_activity);
  }

  if (num_of_tasks > 0)
  {
    optimistic_resource_manager(num_of_tasks, num_of_resources, resource_units, task_activities);
    double percentage = 0;
    int totalTimeTaken = 0;
    int totalTimeWaited = 0;
    for (int i = 0; i < num_of_tasks; i++)
    {
      if (abortedTasks[i] != 1)
      {
        percentage = ((double)timeWaited[i] / totalTime[i] * 100);
        printf("Task %d\t%d\t%d\t%.0f%%\n", i + 1, totalTime[i], timeWaited[i], percentage);
        totalTimeTaken += totalTime[i];
        totalTimeWaited += timeWaited[i];
      }
      else
      {
        printf("Task %d \taborted\n", i + 1);
      }
    }
    percentage = ((double)totalTimeWaited / totalTimeTaken * 100);
    printf("total\t%d\t%d\t%.0f%%\n", totalTimeTaken, totalTimeWaited, percentage);
    // bankers_algorithm(num_of_tasks, num_of_resources, resource_units, task_activities);
    // cpuUtil = (double)(totalTime - (cpuDowntime - 1)) / totalTime;
    // fprintf(outputFile, "\nFinishing time: %d\n", totalTime - 1);
    // fprintf(outputFile, "CPU Utilization: %.2lf\n", cpuUtil);
    // for (int i = 0; i < num_of_processes; i++)
    // {
    // 	fprintf(outputFile, "Turnaround Process %d: %d\n", i, turnaroundTimes[i]);
    // }
  }

  fclose(fp);

  return 0;
}

void optimistic_resource_manager(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[])
{
  // Allocate memory to totalTime, timeWaited, requestedUnits, and abortedTasks arrays
  totalTime = (int *)malloc(num_of_tasks * sizeof(int));
  timeWaited = (int *)malloc(num_of_tasks * sizeof(int));
  requestedUnits = (int **)malloc(num_of_tasks * sizeof(int *));
  abortedTasks = (int *)malloc(num_of_tasks * sizeof(int));
  completedTasks = (int *)malloc(num_of_tasks * sizeof(int));
  computingTasks = (int *)malloc(num_of_tasks * sizeof(int));

  // Allocate memory for each row of requestedUnits (represents tasks for each resource type)
  for (int x = 0; x < num_of_tasks; x++)
  {
    requestedUnits[x] = (int *)malloc(num_of_resources * sizeof(int));
  }

  // List for the blocked tasks
  List *blockedActivities = createList();

  // Array for the processes blocked (used for determining which ones to skip in handleActivities method)
  int blockedTaskIDs[num_of_tasks];

  // Array for determining how many units from each task need to be released, initialize all values to 0
  int releasedUnits[num_of_resources];
  for (int y = 0; y < num_of_resources; y++)
  {
    releasedUnits[y] = 0;
  }

  int timer = 0;
  finishedTasks = 0;
  while (finishedTasks < num_of_tasks)
  {
    // While there is still input to be interpreted
    checkBlockedTasks(resource_units, blockedActivities, blockedTaskIDs);
    handleActivities(num_of_tasks, num_of_resources, resource_units, task_activities, blockedTaskIDs, blockedActivities, releasedUnits, timer);

    if (blockedActivities->size == num_of_tasks - finishedTasks && blockedActivities->size != 0)
    {
      abortTasks(num_of_tasks, task_activities, blockedActivities, resource_units, blockedTaskIDs);
    }
    updateBlockedTaskIDs(num_of_tasks, blockedTaskIDs, blockedActivities);
    for (int i = 0; i < num_of_resources; i++)
    {
      resource_units[i] += releasedUnits[i];
      releasedUnits[i] = 0;
    }
    timer++;
  }
}

// Handle the incoming activity
void handleActivities(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int releasedUnits[], int timer)
{
  int hold = 0;
  for (int i = 0; i < num_of_tasks; i++)
  {
    if (blockedTaskIDs[i] != 1 && abortedTasks[i] != 1 && completedTasks[i] != 1)
    {
      if (computingTasks[i] > 0)
      {
        computingTasks[i]--;
      }
      else if (strcmp(task_activities[i]->front->data->name, "initiate") == 0)
      {
        dequeueActivity(task_activities[i]);
      }
      else if (strcmp(task_activities[i]->front->data->name, "request") == 0)
      {
        handleRequest(i, task_activities[i]->front->data->resource_type, resource_units, task_activities, blockedTaskIDs, blockedActivities);
      }
      else if (strcmp(task_activities[i]->front->data->name, "release") == 0)
      {
        hold = task_activities[i]->front->data->resource_type - 1;
        requestedUnits[i][hold] -= task_activities[i]->front->data->unit_amount;
        releasedUnits[hold] = handleRelease(i, task_activities);
      }
      else if (strcmp(task_activities[i]->front->data->name, "terminate") == 0)
      {
        totalTime[i] = timer;
        dequeueActivity(task_activities[i]);
        completedTasks[i] = 1;
        finishedTasks++;
      }
      else if (strcmp(task_activities[i]->front->data->name, "compute") == 0)
      {
        hold = task_activities[i]->front->data->resource_type - 1;
        computingTasks[i] = hold;
        dequeueActivity(task_activities[i]);
      }
      // printf("Released: %d\n", task_activities[i]->front->data->resource_type);
    }
  }
}

void handleRequest(int index, int resource_type, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities)
{
  // If there is enough resources to grant the request
  if (resource_units[resource_type - 1] >= task_activities[index]->front->data->unit_amount)
  {
    resource_units[resource_type - 1] -= task_activities[index]->front->data->unit_amount;
    requestedUnits[index][resource_type - 1] += task_activities[index]->front->data->unit_amount;
    dequeueActivity(task_activities[index]);
  }
  else // Otherwise there is not enough resources to grant the request and the task must be put on waiting
  {
    // printf("Task: %d, units: %d\n", task_activities[index]->front->data->task_number, task_activities[index]->front->data->unit_amount);
    // Create temporary activity and add it to blocked queue
    Activity *temp_activity = (Activity *)malloc(sizeof(Activity));
    temp_activity->name = strdup(task_activities[index]->front->data->name);
    temp_activity->task_number = task_activities[index]->front->data->task_number;
    temp_activity->resource_type = task_activities[index]->front->data->resource_type;
    temp_activity->unit_amount = task_activities[index]->front->data->unit_amount;
    appendToList(blockedActivities, temp_activity);
    blockedTaskIDs[index] = 1;
  }
}

int handleRelease(int index, Queue *task_activities[])
{
  int releasedUnits = task_activities[index]->front->data->unit_amount;
  dequeueActivity(task_activities[index]);
  return releasedUnits;
}

void checkBlockedTasks(int resource_units[], List *blockedActivities, int blockedTaskIDs[])
{
  ListNode *current = blockedActivities->head;
  int index = 0;
  int hold = 0;

  while (current != NULL)
  {
    Activity *activity = current->data;

    // Check if unit amount is less than resource_units[activity->resource_type - 1]
    hold = activity->resource_type - 1;
    if (activity->unit_amount <= resource_units[hold])
    {
      removeAtIndex(blockedActivities, index); // Remove the element at the current index
      blockedTaskIDs[activity->task_number - 1] = 0;
    }
    current = current->next;
    index++;
  }
}

// Update the blockedTaskIDs based off the blockedActivities
void updateBlockedTaskIDs(int num_of_tasks, int blockedTaskIDs[], List *blockedActivities)
{
  // Initialize the blockedTaskIDs array to all 0s before update
  for (int i = 0; i < num_of_tasks; i++)
  {
    blockedTaskIDs[i] = 0;
  }

  ListNode *current = blockedActivities->head;

  // Loop through the list to find the activities task numbers that are blocked
  while (current != NULL)
  {
    Activity *activity = current->data;

    // Update waiting time on activity task number
    // printf("Task: %d\n", activity->task_number);

    timeWaited[activity->task_number - 1]++;

    // Set to 1 because blocked
    blockedTaskIDs[activity->task_number - 1] = 1;

    current = current->next;
  }
}

// Abort tasks until we can grant requests if the system is deadlocked
void abortTasks(int num_of_tasks, Queue *task_activities[], List *blockedActivities, int resource_units[], int blockedTaskIDs[])
{
  // Resource type causing deadlock
  int resource_index = blockedActivities->head->data->resource_type - 1;
  // Flag to track if we can break out of the loop
  int canBreak = 0;
  int index = 0;
  int buffer = 0;
  int hold = 0;

  // Loop to find which tasks to abort to break the deadlock
  for (int i = 0; i < num_of_tasks; i++)
  {
    if (completedTasks[i] != 1)
    {
      resource_units[resource_index] += requestedUnits[i][resource_index]; // Units are allocated back to the resource
      hold = task_activities[i]->front->data->task_number - 1;
      abortedTasks[hold] = 1;                       // Adding truth condition to task
      destroyActivityQueue(task_activities[i]);     // Remove the tasks remaining activity list
      removeByTaskNumber(blockedActivities, i + 1); // Remove the task from the blocked activities

      buffer++;
      finishedTasks++; // Incrementing amount of tasks finished

      // Check if amount is greater than or equal to any blocked activity's unit amount
      index = 0;
      ListNode *current = blockedActivities->head;
      while (current != NULL)
      {
        if (resource_units[resource_index] >= current->data->unit_amount)
        {
          canBreak = 1; // Set the flag to break out of the loop
          break;
        }
        current = current->next;
        index++;
      }

      if (canBreak)
      {
        break; // Break out of the loop
      }
    }
  }
}