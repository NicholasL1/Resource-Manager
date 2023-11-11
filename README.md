# Optimistic/Banker's Algorithm Resource Manager

This program emulates a resource manager capable of running with two scheduling algorithms, _First Come First Served_, and _Banker's Algorithm_. The program takes in task activity information and will output the information corresponding to how the time taken for task to complete.

To compile the code and create an executable, run:

**_gcc -o banker banker.c.c activityQueue.c.c list.c_**

This will create a _banker_ executable that you can now use to run the resource manager. To run on the command line, provide input in the following format:

**_./banker (input.txt)_**

Where the input.txt contains the the number of tasks, the number of resource, and their corresponding unit amounts

## activity.h

The activity struct describes the shape of an activity/instruction. Each activity contains:

- char name; _The activity's name_
- int task_number; _The activity's task number_
- int resource_type; _The activity's resource type_
- int unit_amount; _The amount needed from resource type_

## activityQueue.h / activityQueue.c - activityQueue data structure

The activityQueue.h file represents a queue data structure which the resource manager utilizes to the input sequences. It contains:

### QueueNode
Represents a single node of the queue
- Activity *data; _Node's data_
- struct QueueNode *next; _The next node_

### Queue
Queue struct
- QueueNode *front; _The beginning node of the queue_
- QueueNode *rear; _The last node of the queue_
- int size; _The length of the queue_

**Functions**
- Queue *createQueue(); _Initializes a queue_
- int isEmptyActivityQueue(Queue *activityQueue); _Check if queue is empty_
- int getSizeActivityQueue(Queue *activityQueue); _Get the size of the queue_
- void enqueueActivity(Queue *activityQueue, Activity *activity); _Adds element to end of queue_
- Activity *dequeueActivity(Queue *activityQueue); _Takes element from the front of queue_
- void destroyActivityQueue(Queue *activityQueue); _Clear the queue_ 

## list.h / list.c - list data structure

The list.h file represents a list data structure that holds the blocked activities. It contains:

### ListNode
Represents a single node of the list
- Activity *data; _Node's data_
- struct ListNode *next _The next node_

### List
List struct
- ListNode *head _The had of the list_
- int size _The size of the list_

**Functions**
- List *createList(); _Initializes a list_
- int isEmpty(List *list); _Check if the list is empty_
- int getSize(List *list); _Get the size of the list_
- void appendToList(List *list, Activity *activity); _Add to the end of the list_
- void removeFromList(List *list, Activity *activity); _Remove from front of the list_
- void removeAtIndex(List *list, int index); _Remove at a certain index of the list_
- void removeByTaskNumber(List *list, int task_number); _Remove from the list by a task number_
- void clearList(List *list); _Clear all the elements of the list_
- void destroyList(List *list); _Clear all elements and free the space in memory_

## banker.h / banker.c - Resoource Manager

The banker file takes a command line input given by a .txt file with the corresponding task activities.

The first line of the .txt file gives the number of tasks, the number of resources, and the amount of units per each of the resources given sequentially.

The next set of activities represents the instructions corresponding to the first task. Each activity contains a name, the task it is instructing, the resource it uses, and the units it uses.

Each set of instructions then respond to the remaining tasks.