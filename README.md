Project Description:

This project includes the use of threads to build and solve the producer consumer problem. I first create 5 threads, with one thread for distributor, two threads for consumer
type 1 and two for consumer type 2. In my build of the problem, the producer is called inside a loop within the distributor thread. The producer returns a product and increments
a counter of the buffer of a certain product type. After the producer runs, the distributor (if there are open slots in the buffer) enters critical section where it takes the product produced by the producer, and 
puts it inside a buffer. The consumer thread (type 1 or type 2) waits for an open slot in the buffer, if a slot opens, it enters a critical section to consume a product and decrement the counter. It also 
prints data (product type, product value, thread id, counter) to log information to a file. 
