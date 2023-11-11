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

int finishedTasks;

int *totalTime;
int *timeWaited;
int **requestedUnits;
int *abortedTasks;
int *completedTasks;
int *computingTasks;
int *bankersAbortCheck;
int *skipTasks;
int **initialClaims;
char **messages;

void readInput(int num_of_tasks, FILE *fp, Queue *task_activities[]);
// Algos
void optimistic_resource_manager(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[]);
void bankers_algorithm(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[]);
// Activity handling
void handleActivities(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int releasedUnits[], int timer);
void handleActivitiesBA(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int releasedUnits[], int timer);
// Request handling
void handleRequest(int index, int resource_type, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities);
void handleRequestBA(int index, int resource_type, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int timer);
// Released Handling
int handleRelease(int index, Queue *task_activities[]);
// Checking blocked tasks
void checkBlockedTasks(int resource_units[], List *blockedActivities, int blockedTaskIDs[]);
void checkBlockedTasksBA(int resource_units[], List *blockedActivities, int blockedTaskIDs[], Queue *task_activities[], int timer, int num_of_resources);
// Updating blocked task IDs
void updateBlockedTaskIDs(int num_of_tasks, int blockedTaskIDs[], List *blockedActivities);
// Aborting
void abortTasks(int num_of_tasks, Queue *task_activities[], List *blockedActivities, int resource_units[], int blockedTaskIDs[]);
void bankersAbort(int task_index, char *message, Queue *task_activities[], int resource_units[]);
// Error check for BA
char *errorCheck(int resource_units[], int task_index, int resource_index, int timer, int unit_amount);

int main(int argc, char *argv[])
{
  FILE *fp1;
  FILE *fp2;

  argc = argc;

  int num_of_tasks;
  int num_of_resources;

  if (argc != 2)
  {
    printf("Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  fp1 = fopen(argv[1], "r");
  fp2 = fopen(argv[1], "r");
  if (fp1 == NULL || fp2 == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  // Read the number of tasks and the number of resources
  fscanf(fp1, "%d %d", &num_of_tasks, &num_of_resources);

  // Create an array to store the units of each resource
  int resource_units[num_of_resources];

  // Read the units of each resource resource x units = num_of_resources[x-1]
  for (int i = 0; i < num_of_resources; i++)
  {
    if (fscanf(fp1, "%d", &resource_units[i]) != 1)
    {
      printf("Error reading resource units from the file.\n");
      fclose(fp1);
      return 1;
    }
  }

  // Create an array of queues to hold activities for each task
  Queue *task_activitiesORM[num_of_tasks];
  Queue *task_activitiesBA[num_of_tasks];
  readInput(num_of_tasks, fp1, task_activitiesORM);

  rewind(fp2);
  // Skip the first line in fp1
  while (fgetc(fp2) != '\n')
  {
    // Continue reading characters until a newline is encountered
  }

  readInput(num_of_tasks, fp2, task_activitiesBA);

  if (num_of_tasks > 0)
  {
    double percentage = 0;
    int totalTimeTaken = 0;
    int totalTimeWaited = 0;
    // // Optimistic resource manager

    optimistic_resource_manager(num_of_tasks, num_of_resources, resource_units, task_activitiesORM);
    printf("\tFIFO\n");
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
      free(requestedUnits[i]);
    }
    percentage = ((double)totalTimeWaited / totalTimeTaken * 100);
    printf("total\t%d\t%d\t%.0f%%\n", totalTimeTaken, totalTimeWaited, percentage);

    // Bankers algorithm

    percentage = 0;
    totalTimeTaken = 0;
    totalTimeWaited = 0;
    bankers_algorithm(num_of_tasks, num_of_resources, resource_units, task_activitiesBA);
    printf("\tBANKER'S\n");
    for (int i = 0; i < num_of_tasks; i++)
    {
      if (strcmp(messages[i], "") != 0)
      {
        printf("%s\n", messages[i]);
      }
    }
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
      free(requestedUnits[i]);
    }
    percentage = ((double)totalTimeWaited / totalTimeTaken * 100);
    printf("total\t%d\t%d\t%.0f%%\n", totalTimeTaken, totalTimeWaited, percentage);
  }

  fclose(fp1);
  fclose(fp2);

  return 0;
}

void readInput(int num_of_tasks, FILE *fp, Queue *task_activities[])
{
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
}

// Main method for the optimistic resource manager
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
  while (finishedTasks < num_of_tasks) // While the amount of finished tasks is less than the number of tasks
  {
    // Check blocked tasks, then handle the activities sequentially
    checkBlockedTasks(resource_units, blockedActivities, blockedTaskIDs);
    handleActivities(num_of_tasks, num_of_resources, resource_units, task_activities, blockedTaskIDs, blockedActivities, releasedUnits, timer);

    // If the amount of blocked tasks is the same as the number of tasks - finished ones and they are not 0, then abort the tasks
    if (blockedActivities->size == num_of_tasks - finishedTasks && blockedActivities->size != 0)
    {
      abortTasks(num_of_tasks, task_activities, blockedActivities, resource_units, blockedTaskIDs);
    }
    updateBlockedTaskIDs(num_of_tasks, blockedTaskIDs, blockedActivities); // Update the blocked task IDs
    // Allocate resources back to their resource type after resources are released
    for (int i = 0; i < num_of_resources; i++)
    {
      resource_units[i] += releasedUnits[i];
      releasedUnits[i] = 0;
    }
    timer++;
  }
}

// Method for the bankers algorithm for resource management
void bankers_algorithm(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[])
{
  // Free memory
  free(totalTime);
  free(timeWaited);
  free(requestedUnits);
  free(abortedTasks);
  free(completedTasks);
  free(computingTasks);

  // Allocate memory to totalTime, timeWaited, requestedUnits, and abortedTasks arrays
  totalTime = (int *)malloc(num_of_tasks * sizeof(int));
  timeWaited = (int *)malloc(num_of_tasks * sizeof(int));
  requestedUnits = (int **)malloc(num_of_tasks * sizeof(int *));
  abortedTasks = (int *)malloc(num_of_tasks * sizeof(int));
  completedTasks = (int *)malloc(num_of_tasks * sizeof(int));
  computingTasks = (int *)malloc(num_of_tasks * sizeof(int));
  initialClaims = (int **)malloc(num_of_tasks * sizeof(int *));
  bankersAbortCheck = (int *)malloc(num_of_tasks * sizeof(int));
  skipTasks = (int *)malloc(num_of_tasks * sizeof(int));
  messages = (char **)malloc(num_of_tasks * sizeof(char *));

  // Allocate memory for each row of requestedUnits (represents tasks for each resource type)
  for (int i = 0; i < num_of_tasks; i++)
  {
    bankersAbortCheck[i] = 0;
    initialClaims[i] = (int *)malloc(num_of_resources * sizeof(int));
    messages[i] = (char *)malloc(500 * sizeof(char));
    requestedUnits[i] = (int *)malloc(num_of_resources * sizeof(int));
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
    // Checks blocked tasks then handle activities
    checkBlockedTasksBA(resource_units, blockedActivities, blockedTaskIDs, task_activities, timer, num_of_resources);
    handleActivitiesBA(num_of_tasks, num_of_resources, resource_units, task_activities, blockedTaskIDs, blockedActivities, releasedUnits, timer);

    // If any of the tasks need to be aborted, abort them and release their requested resources
    for (int i = 0; i < num_of_tasks; i++)
    {
      if (bankersAbortCheck[i] == 1)
      {
        bankersAbort(i, messages[i], task_activities, resource_units); // Abort the i+1 task and release its requestedUnits
      }
    }
    updateBlockedTaskIDs(num_of_tasks, blockedTaskIDs, blockedActivities); // Update blocked task IDs
    // Reallocate released units to resources
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
      // If the activity is compute, set the array and the tasks index to the amount it needs to wait, and dequeue the activity
      else if (strcmp(task_activities[i]->front->data->name, "compute") == 0)
      {
        hold = task_activities[i]->front->data->resource_type - 1;
        computingTasks[i] = hold;
        dequeueActivity(task_activities[i]);
      }
    }
  }
}

// Handling the activities for bankers
void handleActivitiesBA(int num_of_tasks, int num_of_resources, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int releasedUnits[], int timer)
{
  int hold = 0;
  char *message;
  for (int i = 0; i < num_of_tasks; i++)
  {
    if (blockedTaskIDs[i] != 1 && abortedTasks[i] != 1 && completedTasks[i] != 1)
    {
      // If the blocked task's request was handled already, skip it
      if (skipTasks[i] == 1)
      {
        skipTasks[i] = 0;
      }
      else
      {
        if (computingTasks[i] > 0)
        {
          computingTasks[i]--;
        }
        else if (strcmp(task_activities[i]->front->data->name, "initiate") == 0)
        {
          // Determine if the claim is greater than the resources amount on the initiate, if it is set the truth array to true
          initialClaims[i][task_activities[i]->front->data->resource_type - 1] = task_activities[i]->front->data->unit_amount;
          message = (char *)malloc(500 * sizeof(char));
          message = errorCheck(resource_units, i, task_activities[i]->front->data->resource_type, timer, 0);
          if (strcmp(message, "") != 0)
          {
            bankersAbortCheck[i] = 1;
            messages[i] = message;
          }
          else
          {
            dequeueActivity(task_activities[i]);
          }
        }
        else if (strcmp(task_activities[i]->front->data->name, "request") == 0)
        {
          handleRequestBA(i, task_activities[i]->front->data->resource_type, resource_units, task_activities, blockedTaskIDs, blockedActivities, timer);
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
      }
    }
  }
}

// Handle the request activity
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

// Handle request for bankers
void handleRequestBA(int index, int resource_type, int resource_units[], Queue *task_activities[], int blockedTaskIDs[], List *blockedActivities, int timer)
{
  // If the amount of remaining resources for that resource type is greater than or equal to than the initial claim of the task, then we can grant the request
  if (resource_units[resource_type - 1] >= initialClaims[index][resource_type - 1] - requestedUnits[index][resource_type - 1])
  {
    // Check if the requested amounts are greater than the tasks initial claim in the error check, if it is then abort
    char *message = (char *)malloc(500 * sizeof(char));
    message = errorCheck(resource_units, index, task_activities[index]->front->data->resource_type, timer, task_activities[index]->front->data->unit_amount);
    if (strcmp(message, "") != 0)
    {
      bankersAbortCheck[index] = 1;
      messages[index] = message;
    }
    else
    {
      resource_units[resource_type - 1] -= task_activities[index]->front->data->unit_amount;
      requestedUnits[index][resource_type - 1] += task_activities[index]->front->data->unit_amount;
      dequeueActivity(task_activities[index]);
    }
  }
  else // Otherwise, the initial claim is larger than the remaining resources of that resource type and we must block the task as it is not safe
  {
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

// Handle the release activity
int handleRelease(int index, Queue *task_activities[])
{
  int releasedUnits = task_activities[index]->front->data->unit_amount;
  dequeueActivity(task_activities[index]);
  return releasedUnits;
}

// Check if any tasks can be unblocked
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

// Check if any tasks can be unblocked for BA
void checkBlockedTasksBA(int resource_units[], List *blockedActivities, int blockedTaskIDs[], Queue *task_activities[], int timer, int num_of_resources)
{
  ListNode *current = blockedActivities->head;
  int index = 0;
  int hold = 0;
  int check = 0;

  while (current != NULL)
  {
    Activity *activity = current->data;
    if (activity->name == NULL)
    {
      break;
    }

    // Check if unit amount is less than resource_units[activity->resource_type - 1]
    hold = activity->resource_type - 1;
    // Compare the claims of the tasks for that resource against the amount of units for that resource to determine if the task can be unblocked
    for (int i = 0; i < num_of_resources; i++)
    {
      if (initialClaims[activity->task_number - 1][i] > resource_units[i])
      {
        check = 1;
      }
    }
    // If the unit amount of the request is less than or equal to the resource units and the above case is false for all its resource types, then unblock
    if (activity->unit_amount <= resource_units[hold] && check == 0)
    {
      removeAtIndex(blockedActivities, index); // Remove the element at the current index
      blockedTaskIDs[activity->task_number - 1] = 0;
      skipTasks[activity->task_number - 1] = 1;
      handleRequestBA(activity->task_number - 1, activity->resource_type, resource_units, task_activities, blockedTaskIDs, blockedActivities, timer);
    }
    check = 0;
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

// Method to error check for bankers algorithm
char *errorCheck(int resource_units[], int task_index, int resource_index, int timer, int unit_amount)
{
  // If the unit amount is non existant, check initial claim
  if (unit_amount == 0)
  {
    if (initialClaims[task_index][resource_index - 1] > resource_units[resource_index - 1])
    {
      char *string = (char *)malloc(500 * sizeof(char));
      sprintf(string, "During cycle %d-%d of Banker's algorithms, Task %d's initial claim (%d) exceeds the amount of resources present (%d); aborted", timer, timer + 1, task_index + 1, initialClaims[task_index][resource_index - 1], resource_units[resource_index - 1]);
      return string;
    }
  }
  else
  {
    // If a tasks requested units exceeds its initial claim
    if ((requestedUnits[task_index][resource_index - 1] + unit_amount) > initialClaims[task_index][resource_index - 1])
    {
      char *string = (char *)malloc(500 * sizeof(char));
      sprintf(string, "During cycle %d-%d of Banker's algorithms, Task %d's request (%d) exceeds its claim (%d); aborted; %d units available next cycle", timer, timer + 1, task_index + 1, requestedUnits[task_index][resource_index - 1] + unit_amount, initialClaims[task_index][resource_index - 1], requestedUnits[task_index][resource_index - 1]);
      return string;
    }
  }
  return "";
}

// Abort the given task because it failed the bankers algorithm error check and released its requested units
void bankersAbort(int task_index, char *message, Queue *task_activities[], int resource_units[])
{
  int resource_index = 0;
  resource_index = task_activities[task_index]->front->data->resource_type - 1;
  resource_units[resource_index] += requestedUnits[task_index][task_activities[task_index]->front->data->resource_type - 1]; // Units are allocated back to the resource
  abortedTasks[task_index] = 1;
  finishedTasks++;                                   // Adding truth condition to task
  destroyActivityQueue(task_activities[task_index]); // Remove the tasks remaining activity list
  bankersAbortCheck[task_index] = 0;
}