#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 * An encapsulation of a queue of elements.
 * The queue has a maximal number of elements it can contain
 * And an integer which represents the size of the queue
 * Each queue element has a positive index and a doube type value
 *
 * The following functions are supported:
 *
 * spBPQueueCreate			- Creates an empty queue with a given maximal capacity
 * spBPQueueCopy			- Creates a copy of a given queue
 * spBPQueueDestroy			- Frees all memory allocation associated with the queue
 * spBPQueueClear			- Removes all the elements in the queue
 * spBPQueueSize			- Returns the number of elements in the queue
 * spBPQueueGetMaxSize		- Returns the maximal capacity of the queue
 * spBPQueueEnqueue			- Inserts an element to the queue
 * spBPQueueDequeue			- Removes the element with the lowest value from the queue
 * spBPQueuePeek			- Returns a copy of the element with the lowest value
 * spBPQueuePeekLast		- Returns a copy of the element with the highest value
 * spBPQueueMinValue		- Returns the minimal value in the queue
 * spBPQueueMaxValue		- Returns the maximal value in the queue
 * spBPQueueIsEmpty			- Returns true if the queue is empty
 * spBPQueueIsFull			- Returns true if the queue is full
 *
 */

typedef struct sp_bpq_element_t 
{
	int index;
	double value;
} BPQueueElement;

typedef struct sp_bp_queue_t
{
	BPQueueElement *data;
	int maxSize;
	int size;
} SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Given the maximal size the function allocates a new queue in the memory
 * such that:
 * The new queue is empty
 * The new queue's maximal size is as the maximal size given
 * The new queue's size is 0
 * 
 * @param maxSize - The maximal size of the new queue
 * @return
 * NULL in case of an error
 * Otherwise, the new queue is returned
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * Allocates a copy of the given queue.
 *
 * Given the queue source, the functions returns a
 * new queue such that:
 * The elements in the copy are the same as the elements in the source queue
 * The maximal size of the copy is the same as the mazimal size of the source queue
 * The size of the copy is the same as the size of the source queue
 *
 * @param source - The source queue
 * @assert (source != NULL)
 * @return
 * NULL in case of an error
 * Otherwise a copy of source is returned.
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Given the queue source  the function 
 * frees all memory allocation associated with the queue
 *
 * @param source - The source queue
 * @assert source != NULL
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * Given the queue source the function removes all elements in the queue
 *
 * @param source - The source queue
 * @assert source != NULL
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * A getter for the size of the queue
 *
 * @param source - The source queue
 * @assert source != NULL
 * @return
 * The size of the source queue
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * A getter for the maximal size of the queue
 *
 * @param source - The source queue
 * @assert source != NULL
 * @return
 * The maximal size of the source queue
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * Given the queue source an index N and a value V
 * the function inserts an element E to the queue such that:
 * E's index is N and E's value is V
 *
 * @param source - The source queue
 * @param index - The index of the new element
 * @param value - The value of the new element
 * @assert source != NULL
 * @return
 * SP_BPQUEUE_FULL if the queue is full
 * SP_BPQUEUE_INVALID_ARGUMENT if source == NULL
 * Otherwise, SP_BPQUEUE_SUCCESS
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * Given the queue source  the function removes the element with the lowest value
 *
 * @param source - The source queue
 * @return
 * SP_BPQUEUE_EMPTY if the queue is empty
 * SP_BPQUEUE_INVALID_ARGUMENT if source == NULL
 * Otherwise, SP_BPQUEUE_SUCCESS
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Given the queue source  and a queue element, res, the function changes res to be
 * a copy of the element with the lowest value.
 *
 * We denote the element with the lowest value by m,
 * At the end res's value is m's value
 * and res's index is m's index
 *
 * @param source - The source queue
 * @param res - A queue element that would become a copy of m
 * @return
 * SP_BPQUEUE_EMPTY if the queue is empty
 * SP_BPQUEUE_INVALID_ARGUMENT if source == NULL
 * Otherwise, SP_BPQUEUE_SUCCESS
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Given the queue source and a queue element, res, the function changes res to be
 * a copy of the element with the highest value.
 *
 * We denote the element with the highest value by M,
 * At the end res's value is M's value
 * and res's index is M's index
 *
 * @param source - The source queue
 * @param res - A queue element that would become a copy of M
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if source == NULL or res == NULL
 * SP_BPQUEUE_EMPTY if the queue is empty
 * Otherwise, SP_BPQUEUE_SUCCESS
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * Given the queue source  the function returns the minimal element value in the queue.
 *
 * @param source - The source queue
 * 
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if source == NULL or res == NULL
 * SP_BPQUEUE_EMPTY if the queue is empty
 * Otherwise, SP_BPQUEUE_SUCCESS
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * Given the queue source  the function returns the maximal element value in the queue.
 *
 * @param source - The source queue
 * @assert source != NULL and the queue is not empty
 * @return
 * the maximal value in the queue
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * Given the queue source  the function checks if the queue is empty.
 *
 * @param source - The source queue
 * @assert source != NULL
 * @return
 * True if the queue is empty
 * Otherwise, False.
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * Given the queue source  the function checks if the queue is full.
 *
 * @param source - The source queue
 * @assert source != NULL
 * @return
 * True if the queue is full
 * Otherwise, False.
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
