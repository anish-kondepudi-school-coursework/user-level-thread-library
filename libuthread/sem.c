#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t queue;
	int count;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}

