# UTHREAD: User-level thread library

## Summary

The `uthread` library provides the user with a handful of useful methods to create and manage user-level threads such `uthread_run`, `uthread_create`, `uthread_yield`, `uthread_exit`. Additionally, the `uthread` facilitates the ability to synchronize threads and protect shared memory through the use of semaphores, which are exposed through the endpoints `sem_create`, `sem_destroy`, `sem_down`, `sem_up`. The `uthread` library also supports preemption among threads to prevent starvation.

## Queue Implementation

The queue supports constant time FIFO operation through the use of a linked-list data structure. This is important since the queue is internally used within the thread and semaphore implementations - thus, having a low latency queue is crucial to the efficient functionality of the user-thread library.

queue_create

- This method creates a new empty queue data structure on the heap in O(1) time

queue_destroy

- This method frees a queue data structure to free up memory on the heap
- This is an O(1) operation since it only frees an already empty queue

queue_enqueue

- This method creates a node which holds the data to enqueue
- It then appends this node to the back of the queue (tail node in the linked list)
- This is done in O(1) time since we maintain a pointer to the tail node of the linked list

queue_dequeue

- This method removes the front node from the queue (head node of the linked list) and returns its value
- Since deletion of a node from a linked list is constant time, dequeue is O(1)

queue_delete

- This method iterates through the queue (linked list) and finds the node to delete (this is an O(N) operation)
- It then removes this node from the queue, which is an O(1) operation because deletion of nodes from a linked-list is constant time

queue_iterate

- This method iterates through the queue (linked list) and runs a function of each node's data
- Since the function may delete the current node while iterating, we solve this issue by storing the next nodes value prior to running the function of the current node
- If the function inserts new nodes, the iterate method can handle it since the linked list iterated till it reaches the tail (thus, it automatically handles infinite insertions to the queue)

queue_length

- This method returns the length of the queue in constant time
- This is done in O(1) time since we store an integer within the queue's struct to maintain its length (this integer is maintained as we increment/decrement it respectively on enqueues, dequeues, and deletes)

## UThread Implementation


## Semaphore Implementation


## Preemption Implementation


## Memory Management

