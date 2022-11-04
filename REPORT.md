# UTHREAD: User-level thread library

## Summary

The `uthread` library provides the user with a handful of useful methods to create and manage user-level threads such `uthread_run`, `uthread_create`, `uthread_yield`, `uthread_exit`. Additionally, the `uthread` facilitates the ability to synchronize threads and protect shared memory through the use of semaphores, which are exposed through the endpoints `sem_create`, `sem_destroy`, `sem_down`, `sem_up`. The `uthread` library also supports preemption among threads to prevent starvation.

## Queue Implementation

The queue supports constant time FIFO operation through the use of a linked-list data structure. This is important since the queue is internally used within the thread and semaphore implementations - thus, having a low latency queue is crucial in enabling an efficient user-thread library. The structure of the queue is as follows:

```c
typedef struct node* node_t;
typedef struct queue* queue_t;

struct node {
	void* data;
	node_t next;
};

struct queue {
	node_t front, back;
	unsigned length;
};
```

The queue supports the following methods:

`queue_create`

- This method creates a new empty queue data structure on the heap in O(1) time

`queue_destroy`

- This method de-allocates a queue data structure to free up memory on the heap
- This is an O(1) operation since it only frees an already empty queue

`queue_enqueue`

- This method creates a node which holds the data to enqueue
- It then appends this node to the back of the queue (tail node in the linked list)
- This is done in O(1) time since we maintain a pointer to the tail node of the linked list

`queue_dequeue`

- This method removes the front node from the queue (head node of the linked list) and returns its value
- Since deletion of a node from a linked list is constant time, dequeue is O(1)

`queue_delete`

- This method iterates through the queue (linked list) and finds the node to delete (this is an O(N) operation)
- It then removes this node from the queue, which is an O(1) operation because deletion of nodes from a linked-list is constant time

`queue_iterate`

- This method iterates through the queue (linked list) and runs a function of each node's data
- Since the function may delete the current node while iterating, we solve this issue by storing the next nodes value prior to running the function of the current node
- If the function inserts new nodes, the iterate method can handle it since the linked list iterated till it reaches the tail (thus, it automatically handles infinite insertions to the queue)

`queue_length`

- This method returns the length of the queue in constant time
- This is done in O(1) time since we store an integer within the queue's struct to maintain its length (this integer is maintained as we increment/decrement it respectively on enqueues, dequeues, and deletes)

## UThread Implementation

The uthread API provides a public interface to enable users to create, schedule, and manage threads. To do this, the API maintains a TCB struct (Thread Control Block). Each TCB represents information regarding a single thread and holds important information such as that thread's state, CPU registers, and it's stack. The definiton of a TCB is shown below:

```c
enum State {
	Idle,
	Ready,
	Blocked,
	Running
};

struct uthread_tcb {
	uthread_ctx_t ctx;
	enum State state;
	void* stack;
};
```

Additionally, we maintain 2 global variables that can be accessible from any thread. This allows us to easily context switch regardless of the thread we are currently executing. The first global variable we maintain is a queue (`queue_t g_queue`) which stores TCBs of all the threads we are running. This enables us to efficiently schedule threads in a round robin fashion. We also maintain the current running thread (`struct uthread_tcb* g_current_tcb`) which is useful when context switching. With these global variables shared among all threads, the uthread API provides the following public interface to all users:

**Public Endpoints:**

`uthread_run`
- This method creates an idle thread TCB representing the original execution thread and enqueues it
- It then creates a new thread TCB for the new thread it needs to run and enqueues it
- Next, the idle thread continually yields (`uthread_yield`) until it's the only thread remaining in the queue (essentially blocking itself until every other thread has finished)
- This method is also responsible for enabling preemption if the user requests it

`uthread_create`
- This method creates a new TCB for the new thread it needs to run and enqueues it

`uthread_yield`
- This method stops execution of the current thread, changes its state to `Ready`, and enqueues it
- It then dequeues the next available thread from the queue, changes its state to `Running`, and context switches to it

`uthread_exit`
- This method halts execution of the current thread, deletes it from the queue, and yields to the next available thread (`uthread_yield`)

**Private Endpoints:**

`uthread_block`
- Change current thread's state to `Blocked`
- Yield to next available thread (`uthread_yield`)

`uthread_unblock`
- Change current thread's state to `Ready`

*Note: All methods in the uthread public API disables preemption before running and enables it once complete. This is important since we don't want to be preempted while switching contexts.*

## Semaphore Implementation

The semaphore API provides a public interface for users to control access to common resources by multiple threads. This was done by initializing the semaphore to a count (usually the number of available resources). If a thread successfully takes that resource, then the count is incremented. When a resource isn't available, the thread is blocked until that resource becomes available again. The internal details for the semaphores definition and the publicly exposed methods are shown below. The semaphore struct hold a data member of type `queue_t` - this is used to hold blocked processes that can later be unblocked once resources free up.

```c
struct semaphore {
	queue_t queue;
	size_t count;
};
```

`sem_create`
- This method allocated memory for a semaphore struct and initializes it's values

`sem_destroy`
- This method de-allocates the memory for a semaphore thereby destroying it

`sem_down`
- This method check if resources are available (semaphore's count is greater than 0)
- If resources are not available
    - add current thread to the semaphore's queue of blocked threads
    - block current thread and yield to next available thread (`uthread_block`)
- Decrement count by one

`sem_up`
- Increment count by one
- If any threads are in semaphore's blocked queue, unblock them (`uthread_unblock`)

*Note: All methods in the semaphore API disable preemption before running and enable them once complete. This is important since we don't want to be preempted while in the critical sections of the semaphore logic.*

## Preemption Implementation


## Testing

## Memory Management

