CC = gcc
CPP = g++

OBJS = SPPoint.o SPBPriorityQueue.o SPLogger.o SPConfig.o SPImageProc.o \
  SPKDArray.o SPKDTree.o main_aux.o main.o \

#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG -g

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG -g

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@

SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c

SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.c

SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h SPConfig.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.c

main_aux.o: main_aux.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPBPriorityQueue.h SPKDTree.h SPKDArray.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

main.o: main.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPBPriorityQueue.h SPKDTree.h SPKDArray.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
