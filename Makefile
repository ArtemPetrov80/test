all: shmem

shmem:
	gcc shmem.c -o shmem -lrt

clean:
	rm shmem