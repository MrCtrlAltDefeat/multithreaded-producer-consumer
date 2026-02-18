# Multithreaded Producer–Consumer System (POSIX Threads)

A C implementation of the classic **producer–consumer** synchronization problem using **POSIX threads (`pthread`)** and a **circular buffer**.

Originally completed earlier and published to GitHub later as part of my portfolio.

---

## Build

### Linux / macOS (or WSL)
~~~bash
gcc -o producer_consumer src/producer_consumer.c -pthread
~~~

---

## Run
~~~bash
./producer_consumer <num_producers> <num_consumers> <buffer_size>
~~~

### Example
~~~bash
./producer_consumer 30 8 3
~~~

---

## Notes
- Uses a **circular buffer** shared between producers and consumers.
- Uses synchronization primitives to ensure thread-safe access to the buffer.
