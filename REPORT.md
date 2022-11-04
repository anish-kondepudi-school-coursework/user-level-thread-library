# UTHREAD: User-level thread library

## Summary

The `uthread` library provides the user the ability to create new threads and schedule their execution in a round-robin manner. The library achieves this by providing a handful of useful methods including `uthread_run`, `uthread_create`, `uthread_yield`, and `uthread_exit`. Additionally, the `uthread` library facilitates the ability to synchronize threads and protect shared memory through the use of semaphores, which are exposed through the endpoints `sem_create`, `sem_destroy`, `sem_down`, and `sem_up`. The `uthread` library also supports preemption among threads to prevent starvation through the incorporation of an interrupt-based scheduler.

## Queue Implementation

The queue supports FIFO operations in constant time through the use of a linked-list data structure. This is important since the queue is used internally by the thread and semaphore implementations. Thus, having a low latency queue is crucial in enabling an efficient user-thread library. The structure of the queue is as follows:

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
- It then removes this node from the queue, which is an O(1) operation because deletion of a node from a linked-list is constant time

`queue_iterate`

- This method iterates through the queue (linked list) and runs a function on each node's data
- Since the function may delete the current node while iterating, we solve this issue by storing the next node's value prior to running the function on the current node
- If the function enqueues new nodes, the iterate method can handle it since the linked list iterates till it reaches the tail (thus, it automatically handles infinite insertions to the queue)

`queue_length`

- This method returns the length of the queue in constant time
- This is done in O(1) time since we store an integer within the queue's struct to maintain its length
- This integer is maintained as we increment/decrement it respectively on enqueues, dequeues, and deletes

## UThread Implementation

The uthread API provides a public interface that enables users to create, schedule, and manage threads. To do this, the API maintains a TCB (Thread Control Block) struct. Each TCB represents information regarding a single thread and holds important information such as that thread's state, CPU registers, and its stack. The definition of a TCB is shown below:

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

Additionally, we maintain 2 global variables that can be accessed from any thread. This allows us to easily context switch regardless of the thread we are currently executing. The first global variable we maintain is a queue (`queue_t g_queue`) which stores TCBs of all the threads we are running. This enables us to efficiently schedule threads in a round robin fashion. We also maintain the current running thread (`struct uthread_tcb* g_current_tcb`) which is useful when context switching. With these global variables shared among all threads, the uthread API provides the following public interface to all users:

**Public Endpoints:**

`uthread_run`
- This method creates an idle thread TCB representing the original execution thread and enqueues it
- It then creates a new thread TCB for the new thread it needs to run and enqueues it
- Next, the idle thread continually yields (`uthread_yield`) until it's the only thread remaining in the queue (essentially blocking itself until every other thread has finished)
- This method is also responsible for enabling preemption if the user requests it (`preempt_start`)

`uthread_create`
- This method creates a new TCB for the new thread it needs to run and enqueues it

`uthread_yield`
- This method stops execution of the current thread, changes its state to `Ready`, and enqueues it
- It then dequeues the next available thread from the queue, changes its state to `Running`, and context switches to it

`uthread_exit`
- This method halts execution of the current thread, deletes it from the queue, and yields to the next available thread (`uthread_yield`)

**Private Endpoints:**

`uthread_block`
- This method changes the current thread's state to `Blocked`
- It then yields to the next available thread (`uthread_yield`)

`uthread_unblock`
- This method changes the current thread's state to `Ready`

*Note: All methods in the uthread public API disable preemption before running and enable it once complete. This is important since we don't want to be preempted while performing critical thread management operations.*

## Semaphore Implementation

The semaphore API provides a public interface for users to control access to common resources used by multiple threads. This is done by initializing the semaphore to a count (usually the number of available resources). If a thread successfully takes that resource, then the count is decremented. If a thread releases that resource, the count is incremented. When a resource isn't available, the thread is blocked until that resource becomes available again. The internal details for the semaphore's definition and the publicly exposed methods are shown below. The semaphore struct holds a data member of type `queue_t` - this is used to hold blocked processes that can later be unblocked once resources free up.

```c
struct semaphore {
	queue_t queue;
	size_t count;
};
```

`sem_create`
- This method allocates memory for a semaphore struct and initializes its values

`sem_destroy`
- This method de-allocates the memory for a semaphore thereby destroying it

`sem_down`
- This method checks if resources are available (semaphore's count is greater than 0)
- If resources are not available:
    - this method adds the current thread to the semaphore's queue of blocked threads
    - then it blocks the current thread and yields to the next available thread (`uthread_block`)
- Then it decrements the semaphore's count by one

`sem_up`
- This method increments the count by one
- If any threads are in the semaphore's blocked queue, it unblocks them (`uthread_unblock`)

*Note: All methods in the semaphore API disable preemption before running and enable them once complete. This is important since we don't want to be preempted while in the critical sections of the semaphore logic.*

## Preemption Implementation

Preemption forcefully switches context between multiple running threads to prevent starvation. Since the current non-preemption implementation only switches between threads on `uthread_yield` being called, if a thread never yields, it could starve other threads, thus demonstrating the necessity of preemption.

Preemption is implemented by using signals, specifically the `SIGVTALRM` signal which is raised by `setitimer`. When the current process receives `SIGVTALRM`, an alarm handler method is initiated. This method calls `uthread_yield` which allows the next ready thread to run and then sets another timer using `setitimer`. To enable preemption, several global variables are used since they can be easily accessed regardless of the current thread:

```c
struct sigaction sa; // Used for SIGVTALRM handler
bool preempt_active = false; // Used to disable/enable all preemption
bool signals_paused = false; // Used to temporarily pause/resume preemption
```

Using these global variables, the preemption API exposes several endpoints whose functionalities are defined below:

`preempt_disable`
- This method pauses preemption (sets `signals_paused` to true) thereby stopping `SIGVTALRM` signals from occurring

`preempt_enable`
- This method resumes preemption (sets `signals_paused` to false) thereby allowing `SIGVTALRM` signals to occur
- Additionally, this method calls `setitimer` to begin the `SIGVTALRM` signal loop

`preempt_start`
- This method initializes the alarm handler to handle `SIGVTALRM` signals
- Then, it calls `setitimer` to begin the `SIGVTALRM` signal loop

`preempt_stop`
- This method disables preemption (sets `preempt_active` to false), thereby disabling `preempt_disable` and `preempt_enable`

## Preemption Testing

`test_preempt.c` validates the functionality of preemption by creating a thread, e.g. thread A. Thread A then creates a new thread, e.g. Thread B. Thread A then locks itself in a loop waiting for Thread B to finish. In a situation where preemption is not enabled, Thread A will infinitely loop since Thread B won't be scheduled as Thread A never yields. This would lead to the starvation of Thread B. However, with preemption Thread B will gain control of the CPU and complete its execution. Then Thread A will notice that Thread B has completed and break out of its loop and end its execution. This simple test verifies the functionality of the preemption feature since without preemption the program will result in a process which never terminates.

## Memory Management

One location where objects are dynamically allocated is in `queue.c` where new `node_t`'s are created upon `queue_enqueue`. However, upon `queue_dequeue` and `queue_delete`, these `node_t`'s are free()'ed and therefore don't lead to any memory leaks. Also, the memory allocated for `queue_t` in `queue_create` is free()'ed in `queue_destroy`, further preventing any memory leaks in `queue.c`.

Additionally, in `sem.c`, although `sem_create` malloc()'s memory for the semaphore struct as well as its queue data member, on calling `sem_destroy` that memory is free()'ed and should therefore not lead to any memory leaks.

The same is observed in `uthread.c` where `uthread_tcb_t`'s are dynamically allocated and their stack data members are malloc()'ed. However, upon `uthread_exit` being called, these `uthread_tcb_t`'s are free()'ed along with their stack data members, further preventing memory leaks.

It's important to note that in `uthread.c`, the global variables `g_queue` and `g_current_tcb` are dynamically allocated but never free()'ed. This is because upon the program's completion, these objects will automatically be free()'ed. Since they are only ever malloc()'ed once, this memory on the heap is constant and won't lead to a build up in heap utilization (memory leaks).


