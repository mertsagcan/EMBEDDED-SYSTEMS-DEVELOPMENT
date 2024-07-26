#include "common.h"
#include "hash.h"

/**
 * Hash task
 */

TASK(HASH_TASK)
{
	while (1) {
		WaitEvent(HASH_START_EVENT);
		ClearEvent(HASH_START_EVENT);
		
		compute_hash(hash_received, hash_to_send);
		hash_done = 1;
	}

	TerminateTask();
}
