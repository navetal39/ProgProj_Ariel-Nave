#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 * Encapsulates a priority queue of a given size.
 * The elements of the queue have index and value and are ordered
 * by their values. 
 * 
 * The following functions are supported:
 *  
 * spBPQueueCreate			-Creates a new queue of given size
 * spBPQueueCopy			-Creates a new copy of a given queue
 * spBPQueueDestroy			-Frees all memory allocation associated with given queue
 * spBPQueueClear			-Removes all elemensts from the queue
 * spBPQueueSize			-Returns size of a given queue
 * spBPQueueGetMaxSize		-Returns the maximal size of a given queue
 * spBPQueueEnqueue			-Inserts a new element (of given index and value) to a given queue
 * spBPQueueDequeue			-Removes the element with the lowest value
 * spBPQueuePeek			-Returns a copy of the element with the lowest value
 * spBPQueuePeekLast		-Returns a copy of the element with the highest value
 * spBPQueueMinValue		-Returns the minimum value in the queue
 * spBPQueueMaxValue		-Returns the maximum value in the queue
 * spBPQueueIsEmpty			-Returns true if the queue is empty
 * spBPQueueIsFull			-Returns true if the queue is full
 * 
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

/** Type for defining the elements **/
typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting
 *
 * The following messages are defined:
 * SP_BPQUEUE_OUT_OF_MEMORY		-error allocating memory
 * SP_BPQUEUE_FULL				-queue is full, error in inserting
 * SP_BPQUEUE_EMPTY				-queue is empty, error in extracting
 * SP_BPQUEUE_INVALID_ARGUMENT	-NULL queue, error, invalid argument
 * SP_BPQUEUE_SUCCESS			-success message
 *
 **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new SPBPQueue in the memory
 * Given a maximum size for the queue
 * Creates an array of size maxSize of BPQueueElements(allocation required)
 * @param maxSize - Maximum size of created queue
 * @assert maxSize > 0
 * @return
 * A new SPBPQueue with maximum size maxSize
 */
SPBPQueue* spBPQueueCreate(int maxSize);


/**
 * Allocates a copy of the given SPBPQueue
 *
 * Creates an array of size maxSize of BPQueueElements(allocation required)
 * @param source - queue to be copied
 * @assert source != NULL
 * @return
 * A new SPBPQueue with same elments as source
 * and NULL in case memory allocation error occurs
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Frees all memory allocation associated with source,
 * if source is NULL nothing happens.
 */

void spBPQueueDestroy(SPBPQueue* source);


/**
 * removes all elemensts from the queue
 * @param source - queue to be clear
 * @assert source != NULL
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * returns size of queue
 * @param source - the queue that we want it's size
 * @assert source != NULL
 * @return
 * the size of the queue
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * returns the maximal size of the queue
 * @param source - the queue that we want it's maximal size
 * @assert source != NULL
 * @return
 * the maximal size of the queue
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * inserts new element (index and value) to the queue
 * @param source - the queue that we want to insert to it
 * @param index - the index of the element we enqueue
 * @param value - the value of the element we enqueue
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT 	if source is NULL
 * SP_BPQUEUE_FULL 				if queue is full and value is too large to insert the new element
 * SP_BPQUEUE_SUCCESS 			if the element has been enqueued
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * removes the element with the lowest value
 * @param source - the queue that we want to insert to it
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT 	if source is NULL
 * SP_BPQUEUE_EMPTY 			if queue is empty
 * SP_BPQUEUE_SUCCESS 			if the element has been dequeued
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * puts a copy of the element with the lowest value in given address
 * @param source - queue to peek from
 * @param res - the adress to write the lowest value
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT 	if source is NULL
 * SP_BPQUEUE_EMPTY				if queue is empty
 * SP_BPQUEUE_SUCCESS			if the element has been peeked
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * puts a copy of the element with the highest value in given address
 * @param source - queue to peek from
 * @param res - the adress to write the highest value
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT 	if source is NULL
 * SP_BPQUEUE_EMPTY				if queue is empty
 * SP_BPQUEUE_SUCCESS			if the element has been peeked
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * returns the minimum value in given queue
 * @param source - queue we want it's minimal value
 * @assert source != NULL and source's size is larger than zero
 * @return
 * the minimal value of the queue
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * returns the maximum value in given queue
 * @param source - queue we want it's minimal value
 * @assert source != NULL and source's size is larger than zero
 * @return
 * the maximal value of the queue
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * returns true if the queue is empty
 * @param source - queue to check
 * @assert source != NULL
 * @return
 * true if queue is empty
 * false o.w.
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * returns true if the queue is full
 * @param source - queue to check
 * @assert source != NULL
 * @return
 * true if queue is full
 * false o.w.
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
