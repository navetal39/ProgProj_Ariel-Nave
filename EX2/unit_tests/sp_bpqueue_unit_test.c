#include "../SPPoint.h"
#include "unit_test_util.h"
#include "../SPBPriorityQueue.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

bool QueueElementCompare(BPQueueElement *e1, BPQueueElement *e2) {
	return ((e1->index == e2->index) && (e1->value == e2->value));
}

static bool testQueueCreate() {
	SPBPQueue* created_queue = spBPQueueCreate(3);
	ASSERT_TRUE(spBPQueueGetMaxSize(created_queue) == 3 && spBPQueueSize(created_queue) == 0);
	spBPQueueDestroy(created_queue);
	return true;
}

static bool testQueueCopy() {
	BPQueueElement *e1, *e2;
	e1 = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	e2 = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	SPBPQueue* created_queue = spBPQueueCreate(3);
	SPBPQueue* coppied_queue = spBPQueueCopy(created_queue);
	ASSERT_TRUE(coppied_queue != NULL && spBPQueueGetMaxSize(coppied_queue) == 3 && spBPQueueSize(coppied_queue) == 0);
	spBPQueueEnqueue(created_queue, 8, 1.0);
	spBPQueueEnqueue(created_queue, 2, 3.0);
	spBPQueueEnqueue(created_queue, 1, 1.12);
	spBPQueueEnqueue(created_queue, 3, 9.5);
	spBPQueueEnqueue(created_queue, 2, 0.12);
	coppied_queue = spBPQueueCopy(created_queue);
	ASSERT_TRUE(spBPQueueSize(coppied_queue) == 3);
	spBPQueuePeek(coppied_queue, e1);
	spBPQueuePeek(created_queue, e2);
	ASSERT_TRUE(e1->index == 2);
	ASSERT_TRUE(QueueElementCompare(e1, e2));
	spBPQueuePeekLast(coppied_queue, e1);
	spBPQueuePeekLast(created_queue, e2);
	ASSERT_TRUE(QueueElementCompare(e1, e2));

	spBPQueueDestroy(created_queue);
	spBPQueueDestroy(coppied_queue);
	free(e1);
	free(e2);
	return true;
}


static bool testQueueSizeAndMaxSize() {
	SPBPQueue* created_queue = spBPQueueCreate(3);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 0 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueEnqueue(created_queue, 8, 1.0);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 1 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueEnqueue(created_queue, 2, 3.0);
	spBPQueueEnqueue(created_queue, 1, 1.12);
	spBPQueueEnqueue(created_queue, 3, 9.5);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 3 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueEnqueue(created_queue, 2, 0.12);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 3 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueDequeue(created_queue);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 2 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 0 && spBPQueueGetMaxSize(created_queue) == 3);

	spBPQueueDestroy(created_queue);
	return true;
}


static bool testEnqueueAndDequeue() {
	SPBPQueue* created_queue = spBPQueueCreate(3);
	BPQueueElement* e1;
	e1 = (BPQueueElement*) malloc(sizeof(BPQueueElement));

	spBPQueueEnqueue(created_queue, 8, 1.0);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 8 && e1->value == 1.0);

	spBPQueueEnqueue(created_queue, 2, 1.12);
	spBPQueueEnqueue(created_queue, 1, 1.12);
	spBPQueueEnqueue(created_queue, 3, 9.5);

	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 8 && e1->value == 1.0);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 2 && e1->value == 1.12);

	spBPQueueEnqueue(created_queue, 2, 0.12);
	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 2 && e1->value == 0.12);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);

	spBPQueueDequeue(created_queue);
	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 8 && e1->value == 1.0);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);

	spBPQueueDequeue(created_queue);
	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);

	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	ASSERT_TRUE(spBPQueueSize(created_queue) == 0);

	spBPQueueDestroy(created_queue);
	free(e1);
	return true;
}

static bool testPeekFirstAndLast() {
	SPBPQueue* created_queue = spBPQueueCreate(3);
	BPQueueElement* e1;

	e1 = (BPQueueElement*) malloc(sizeof(BPQueueElement));

	spBPQueueEnqueue(created_queue, 8, 1.0);
	spBPQueueEnqueue(created_queue, 2, 3.0);
	spBPQueueEnqueue(created_queue, 1, 1.12);

	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 8 && e1->value == 1.0);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 2 && e1->value == 3.0);

	spBPQueueEnqueue(created_queue, 3, 9.5);
	spBPQueueEnqueue(created_queue, 2, 0.12);

	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 2 && e1->value == 0.12);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);

	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);

	spBPQueuePeek(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);
	spBPQueuePeekLast(created_queue, e1);
	ASSERT_TRUE(e1->index == 1 && e1->value == 1.12);

	spBPQueueDestroy(created_queue);
	free(e1);
	return true;
}


static bool testGetMinAndMax() {
	SPBPQueue* created_queue = spBPQueueCreate(3);

	spBPQueueEnqueue(created_queue, 8, 1.0);
	spBPQueueEnqueue(created_queue, 2, 3.0);
	spBPQueueEnqueue(created_queue, 1, 1.12);

	ASSERT_TRUE(spBPQueueMinValue(created_queue) == 1.0);
	ASSERT_TRUE(spBPQueueMaxValue(created_queue) == 3.0);

	spBPQueueEnqueue(created_queue, 3, 9.5);
	spBPQueueEnqueue(created_queue, 2, 0.12);

	ASSERT_TRUE(spBPQueueMinValue(created_queue) == 0.12);
	ASSERT_TRUE(spBPQueueMaxValue(created_queue) == 1.12);

	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);

	ASSERT_TRUE(spBPQueueMinValue(created_queue) == 1.12);
	ASSERT_TRUE(spBPQueueMaxValue(created_queue) == 1.12);

	spBPQueueDestroy(created_queue);
	return true;
}

static bool testEmptyAndFull() {
	SPBPQueue* created_queue = spBPQueueCreate(3);
	ASSERT_TRUE(spBPQueueIsEmpty(created_queue));
	spBPQueueEnqueue(created_queue, 8, 1.0);

	ASSERT_TRUE(!(spBPQueueIsEmpty(created_queue)));

	spBPQueueEnqueue(created_queue, 2, 3.0);
	spBPQueueEnqueue(created_queue, 1, 1.12);

	ASSERT_TRUE(spBPQueueIsFull(created_queue));
	ASSERT_TRUE(!(spBPQueueIsEmpty(created_queue)));

	spBPQueueEnqueue(created_queue, 3, 9.5);

	ASSERT_TRUE(spBPQueueIsFull(created_queue));

	spBPQueueEnqueue(created_queue, 2, 0.12);

	ASSERT_TRUE(spBPQueueIsFull(created_queue));

	spBPQueueDequeue(created_queue);

	ASSERT_TRUE(!(spBPQueueIsFull(created_queue)));
	ASSERT_TRUE(!(spBPQueueIsEmpty(created_queue)));

	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);
	spBPQueueDequeue(created_queue);

	ASSERT_TRUE(!(spBPQueueIsFull(created_queue)));
	ASSERT_TRUE(spBPQueueIsEmpty(created_queue));

	spBPQueueDestroy(created_queue);
	return true;
}

int main() {
	RUN_TEST(testQueueCreate);
	RUN_TEST(testQueueCopy);
	RUN_TEST(testQueueSizeAndMaxSize);
	RUN_TEST(testEnqueueAndDequeue);
	RUN_TEST(testPeekFirstAndLast);
	RUN_TEST(testGetMinAndMax);
	RUN_TEST(testEmptyAndFull);
	return 0;
}


