CC = g++
CFLAGS = -g -Wall
OCL = -lOpenCL

all:	sequential_bfs parallel_bfs

sequential_bfs:	sequential_bfs.cpp
	$(CC) $(CFLAGS) -o sequential_bfs sequential_bfs.cpp

parallel_bfs: parallel_bfs.cpp
	$(CC) $(CFLAGS) -o parallel_bfs parallel_bfs.cpp $(OCL)

clean:
	rm sequential_bfs parallel_bfs *.out