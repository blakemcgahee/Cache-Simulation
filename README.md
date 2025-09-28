# Cache-Simulation

1. Introduction
The objective of this analysis is to evaluate the performance of different cache designs by examining their hit rates. A cache's performance is a critical factor in a computer system, directly impacting overall speed and efficiency. By simulating a cache with various configurations, we can study key parameters. This includes associativity, cache size, and replacement policy, influence the cache's ability to serve memory requests from a processor. This paper details the methodology used, presents the results from a series of simulation runs, and concludes with an analysis of how each parameter affects the cache's performance. The final goal is to understand the trade-offs and design principles that lead to an optimal cache configuration.
2. Description of Tests
To conduct this analysis, a series of simulations were performed using a cache simulator on several memory access trace files. The primary goal was to isolate and measure the impact of one parameter at a time while holding others constant. The following parameters varied across the tests:
Cache Size: Ranging from 256 bytes to 16,384 bytes, with each value being a power of two. This range was chosen to observe the hit rate's behavior from a very small cache to a more substantial size.
Associativity: Three primary configurations were tested:
•	Direct Mapped: A 1-way set associative cache.
•	Set Associative: Caches with 2-way, 4-way, and 8-way associativity.
•	Fully Associative: A cache with a single set containing all the blocks.
Block Size: Varied across different runs to see its effect on hit rate. Common power-of-two values such as 16, 32, 64, and 128 bytes were used to observe the impact on spatial locality.
Replacement Policy: For all set-associative and fully associative caches, two common replacement policies were examined:
•	Least Recently Used (LRU): The block that has not been accessed for the longest time is replaced.
•	First-In, First-Out (FIFO): The block that has been in the set for the longest time is replaced.
The following table summarizes the parameters used for the analysis.
Test Parameter	Values Examined	Justification
Cache Size	256, 512, 1024, 2048, 4096, 8192, 16384 bytes	To observe how hit rate scales with total cache capacity.
Associativity	Direct-mapped, 2-way, 4-way, 8-way, Fully-associative	To compare the trade-offs between increased complexity and reduced conflict misses.
Block Size	16, 32, 64, 128 bytes	To observe the effect of spatial locality and potential block-size-induced capacity misses.
Replacement Policy	LRU, FIFO	To compare the effectiveness of different strategies for managing blocks in set-associative and fully-associative caches.
