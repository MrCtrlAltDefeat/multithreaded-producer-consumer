To compile:
	create a file by typing "nano producer_consumer.c" into your chosen directory
	copy the code into producer_consumer.c
	run "gcc -o producer_consumer producer_consumer.c -pthread" to compile the code and link the pthread library

To run:
        ./producer_consumer <num_producers> <num_consumers> <buffer_size>


To test:
        ./producer_consumer 30 8 3

	This runs the program with 30 producer threads, 8 consumer threads, and a circular buffer size of 3.



        Sample Output:
                [Producer-1] Produced item: 42		  in index 0
                [Producer-2] Produced item: 17		  in index 1
                [Consumer-1] Consumed item: 42		from index 1
                [Producer-27] Produced item: 88		  in index 2
                [Consumer-4] Consumed item: 17		from index 2
		...
                [Producer-1] Finished producing 20 items.
		Kill pill inserted			  in index 0
                [Consumer-7] Consumed item: -1		from index 0
		[Consumer-7] Finished consuming. Now poisoned by its creator, it dies, betrayed.