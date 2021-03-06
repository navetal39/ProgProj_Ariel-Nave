CC = gcc
OBJS = sp_kd_array_unit_test.o SPKDArray.o SPPoint.o SPBPriorityQueue.o SPLogger.o
EXEC = sp_kd_array_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
sp_kd_array_unit_test.o: $(TESTS_DIR)/sp_kd_array_unit_test.c $(TESTS_DIR)/unit_test_util.h SPKDArray.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPKDArray.o: SPKDArray.c SPKDArray.h 
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
