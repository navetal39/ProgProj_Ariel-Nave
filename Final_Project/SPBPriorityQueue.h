#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 *
 * the queue has maxSize variable (int)- which determines the maximal length of the queue.
 * the queue has size variable (int)- which determines the number of elements in the queue.
 * the queue has arr variable (array of elements) - which stores the elements that are in the queue
 *
 *
 * The following functions are supported:
 *
 * spBPQueueCreate          - creates a new empty queue
 * spBPQueueCopy            - creates a copy of a given queue
 * spBPQueueDestroy         - free all resources associated with the queue
 * spBPQueueClear           - resets the queue
 * spBPQueueSize            - a getter of the real size of the queue
 * spBPQueueGetMaxSize      - a getter of the maximum size of the queue
 * spBPQueueEnqueue         - inserting an element to the queue with given index and value
 * spBPQueueDequeue         - dequeue the minimum element from the queue
 * spBPQueuePeek            - a getter of the minimal element in the queue (by value)
 * spBPQueuePeekLast        - a getter of the maximal element in the queue (by value)
 * spBPQueueMinValue        - a getter of the minimal value of an element in the queue
 * spBPQueueMaxValue        - a getter of the maximal value of an element in the queue
 * spBPQueueIsEmpty         - checks wether the queue is empty
 * spBPQueueIsFull          - checks wether the queue is full
 *
 */

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;



/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new queue in the memory.
 * Allocates the array of queue elements for the queue with size of maxSize
 * setting all of the array to zeros
 * @return
 * NULL in case allocation failure ocurred 
 * Otherwise, a pointer to the new queue is returned
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * Allocates a copy of the given queue 
 * copying the parameters of the source queue to the new one
 * @param source - The source queue
 * @assert (source != NUlL)
 * @return
 * NULL in case memory allocation fails
 * Others a copy of source is returned.
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Free all memory allocation associated with queue,
 * if queue is NULL nothing happens.
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * setting the array in the queue to zeros (reseting it)
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * A getter for the number of elements in queue
 *
 * @param queue - the source queue
 * @assert source != NULL
 * @return
 * the real size of the queue (not max size)
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * A getter for the nmax size of the queue
 *
 * @param queue - the source queue
 * @assert source != NULL
 * @return
 * the maximum size of the queue (not max size)
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * inserting an element to the queue with given index and value
 * by searching the right place to insert it (by value)
 *
 * @param queue - the source queue
 * index (int) fro the new element
 * value (double) fro the new element
 * @return
 * a massage that indicates what went wrong / went good 
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * dequeuing the element with the minimal value from the queue (if the same value - with the minimal index)
 *
 * @param queue - the source queue
 * @return
 * a massage that indicates what went wrong / went good 
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * 
 * a getter of the minimal element in the queue (by value)
 *
 * @param queue - the source queue
 * @return
 * a massage that indicates what went wrong / went good 
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * a getter of the maximal element in the queue (by value)
 *
 * @param queue - the source queue
 * @return
 * a massage that indicates what went wrong / went good 
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * returns the minimum value of an element in the queue
 *
 * @param queue - the source queue
 * @assert source != NULL && source->arr != NULL
 * @return
 * the minimum value in the queue
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * returns the maximum value of an element in the queue
 *
 * @param queue - the source queue
 * @assert source != NULL && source->arr != NULL
 * @return
 * the maximum value in the queue
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * returns true if and only if the queue is empty
 *
 * @param queue - the source queue
 * @assert source != NULL
 * @return
 * boolean value that is true only in the condition above
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * returns true if and only if the queue is full
 *
 * @param queue - the source queue
 * @assert source != NULL
 * @return
 * boolean value that is true only in the condition above
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
