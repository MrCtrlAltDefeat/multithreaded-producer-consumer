#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//items per producer
#define MAX_ITEMS 20

//circular buffer
typedef struct {
	//array that acts as a buffer
	int* arr;
	int size;
	//next produced item index
		int in;
	//next consumed item index
		int out;
} Buffer;

//creating buffer, semaphores and mutex, these are global
Buffer buffer;
sem_t empty, full;
pthread_mutex_t mutex;

//==================================Producer==================================

void* producer(void* id) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		int r = rand() % 100; //random nmber from 0-99

		//wait for empty slot, then enter with mutex
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);

		//add item to buffer
		buffer.arr[buffer.in] = r;
		printf("[Producer-%d] Produced item: %d\t\t in index %d\n", *(int*)id, r, buffer.in);
		buffer.in = (buffer.in + 1) % buffer.size;

		//leave then signal full slot
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
	printf("[Producer-%d] Finished producing %d items. \n", *(int*)id, MAX_ITEMS);
	pthread_exit(NULL);
}

//==================================Consumer==================================
void* consumer(void* id) {
	while (1) {
		//wait for full slot, then enter with mutex
		sem_wait(&full);
		pthread_mutex_lock(&mutex);

		//nom nom
		int item = buffer.arr[buffer.out];
		printf("[Consumer-%d] Consumed item: %d\t\tfrom index %d\n", *(int*)id, item, buffer.out);
		buffer.out = (buffer.out + 1) % buffer.size;

		//leave then signal empty slot
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);

		if (item == -1)
			break;
	}
	printf("[Consumer-%d ] Finished consuming. Now poisoned by its creator, it dies, betrayed. \n", *(int*)id);
	pthread_exit(NULL);
}

//==================================Main==================================
int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Bad input. The correct input format is as follows:\n\"/s <numProducers> <numConsumers> <buffersize>\"\n\n", argv[0]);
		return 1;
	}
	int numProducers, numConsumers, bufferSize;
	int shouldExit = 0;

	//check for O's or otherwise bad inputs
	if (!(numProducers = atoi(argv[1]))) {
		printf("Invalid number of producers!\n");
		shouldExit = 1;
	}
	if (!(numConsumers = atoi(argv[2]))) {
		printf("Invalid number of consumers!\n");
		shouldExit = 1;
	}
	if (!(bufferSize = atoi(argv[3]))) {
		printf("Invalid buffer size!\n");
		shouldExit = 1;
	}
	if (shouldExit == 1) {
		printf("\n");
		exit(1);
	}

	//buffer innit
	buffer.arr = (int*)malloc(bufferSize * sizeof(int));
	buffer.size = bufferSize;
	buffer.in = 0;
	buffer.out = 0;

	//init synch stuff
	sem_init(&empty, 0, bufferSize); //all slots empty initially
	sem_init(&full, 0, 0); //no slots empty initially
	pthread_mutex_init(&mutex, NULL);

	pthread_t producers[numProducers];
	pthread_t consumers[numConsumers];

	//putting id's in an array, so they have dereferancable addresses
	int ids[numProducers > numConsumers ? numProducers : numConsumers];
	if (numProducers > numConsumers) for (int i = 0; i < numProducers; i++) ids[i] = i + 1;
	else for (int i = 0; i < numConsumers; i++) ids[i] = i + 1;

	//create producer threads
	for (int i = 0; i < numProducers; i++) {
		//give 3 tries to create thread
		int attempt;
		for (attempt = 0;
			pthread_create(&producers[i], NULL, producer, ids + i) && attempt < 3;
			attempt++);
		//I'11 give you ONE last try !!
		if (attempt == 3)
			if (pthread_create(&producers[i], NULL, producer, ids + i)) {
				printf("Error: failed to create producer thread\n\n");
				exit(1);
			}
	}
	//create consumer threads
	for (int i = 0; i < numConsumers; i++) {
		//give 3 tries to create thread
		int attempt;
		for (attempt = 0;
			pthread_create(&consumers[i], NULL, consumer, ids + i) && attempt < 3;
			attempt++);
		//I' 11 give you ONE last try !!
		if (attempt == 3)
			if (pthread_create(&consumers[i], NULL, consumer, ids + i)) {
				printf("Error: failed to create consumer thread\n\n");
				exit(1);
			}
	}

	//wait for producers
	for (int i = 0; i < numProducers; i++)
		pthread_join(producers[i], NULL);

	//add the poison pills
	for (int i = 0; i < numConsumers; i++) {
		//wait for empty slot, then enter with mutex
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);

		//add killPill to buffer
		buffer.arr[buffer.in] = -1;
		printf("Kill pill inserted\t\t\t in index %d\n", buffer.in);
		buffer.in = (buffer.in + 1) % buffer.size;

		//leave then signal full slot
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}

	//wait for consumers
	for (int i = 0; i < numConsumers; i++)
		pthread_join(consumers[i], NULL);

	printf("\n");
	return 0;
}