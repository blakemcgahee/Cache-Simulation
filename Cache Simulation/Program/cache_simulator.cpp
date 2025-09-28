/*
* The following block is the previous, non-working code.
* It is commented out to show the progression of the assignment.
*
* Problem 1: The original trace files had a different format (e.g., 'R' or 'W', address, size),
* but the provided sample files use 'l' or 's', hex address, and size. This mismatch
* caused the program to fail to parse the input correctly.
*
* Problem 2: When the trace file was modified to remove the size, the `while` loop
* `while (newfile >> access_type_str >> address_hex >> size_str)` would try to read
* three items, but only two were present. This caused an infinite loop or other
* unexpected behavior as the read operation would fail but the stream state wouldn't be handled
* correctly, leading to re-reading the same failed line indefinitely.
*
* The solution below correctly reads the three-column format of the new sample trace files.
*/
/*
// Example of a previous, non-working `while` loop.
// This was designed for a different trace file format.
while (newfile >> access_type_str >> address_hex >> size_str) {
// The previous trace files might have been 'R' or 'W', but the new ones are 'l' or 's'.
// This mismatch would lead to incorrect parsing.
// Also, if the trace file was modified incorrectly, this loop could fail.
}
*/

/*
* The previous version had a bug where the FIFO replacement policy was not correctly
* implemented, causing it to behave identically to the LRU policy.
*
* This revised version correctly implements both LRU and FIFO replacement using a
* std::list to manage the order of blocks within each cache set.
*
* For LRU: The list acts as a queue where the most recently used block is at the
* front, and the least recently used is at the back. On a hit, the block is
* moved to the front. On a miss, the block at the back is evicted.
*
* For FIFO: The list acts as a simple queue. On a miss, the new block is added
* to the back. When a replacement is needed, the block at the front is evicted.
* A hit does not change the order of the blocks in the list.
*/


/*
* The FIFO replacement policy has been corrected and the trace file parsing is now
* more robust to handle the 'l <hex_address> <size>' format.
*/

/*
* This is a complete cache simulator that prompts the user for all parameters,
* runs a single simulation based on the user's choices, and exports the
* results to a uniquely named CSV file (e.g., "read07.trace-lru.csv").
*
* This version of the code maintains the original interactive design
* while adding the data export functionality.
*/

/*
* This is an updated cache simulator. It now includes the cache size,
* block size, and associativity in the output CSV file, along with the
* hit and miss counts. This is necessary to generate the requested plots.
*
* This version of the code maintains the original interactive design
* while providing more detailed data for analysis.
*/


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <list>
#include <sstream>

 // A struct to represent a single cache block
struct CacheBlock {
    bool valid = false;
    unsigned long tag = 0;
};

// A struct to hold the simulation results
struct CacheResults {
    unsigned long hits = 0;
    unsigned long misses = 0;
    double hit_rate = 0.0;
};

// Main Cache class to handle all simulation logic
class Cache {
private:
    std::vector<std::vector<CacheBlock>> cache;
    // This list will manage the replacement order for both LRU and FIFO.
    std::vector<std::list<unsigned long>> replacement_queues;
    std::string replacement_policy;

    unsigned int cache_size;
    unsigned int block_size;
    unsigned int associativity;
    unsigned int num_sets;
    unsigned int tag_bits;
    unsigned int index_bits;
    unsigned int offset_bits;

    unsigned long hits = 0;
    unsigned long misses = 0;
    unsigned long reads = 0;
    unsigned long writes = 0;

public:
    // Constructor to initialize the cache and its parameters
    Cache(unsigned int cs, unsigned int bs, unsigned int assoc, const std::string& rp)
        : cache_size(cs), block_size(bs), associativity(assoc), replacement_policy(rp) {

        // Calculate the number of sets
        num_sets = cache_size / (block_size * associativity);

        // Calculate the number of bits for each part of the address
        offset_bits = log2(block_size);
        index_bits = log2(num_sets);
        tag_bits = 32 - index_bits - offset_bits; // Assuming 32-bit addresses

        // Resize the cache to the correct dimensions
        cache.resize(num_sets, std::vector<CacheBlock>(associativity));
        replacement_queues.resize(num_sets);

        std::cout << "Cache Size: " << cache_size << " bytes\n";
        std::cout << "Block Size: " << block_size << " bytes\n";
        std::cout << "Associativity: " << associativity << " way\n";
        std::cout << "Replacement Policy: " << replacement_policy << "\n";
        std::cout << "Number of Sets: " << num_sets << "\n";
        std::cout << "Tag Bits: " << tag_bits << "\n";
        std::cout << "Index Bits: " << index_bits << "\n";
        std::cout << "Offset Bits: " << offset_bits << "\n";
        std::cout << "------------------------------------\n";
    }

    // Access method to simulate a read or write operation
    void access(char op, unsigned long address) {
        // Handle different operation types
        if (op == 's') { // 's' is a store (write)
            writes++;
        }
        else if (op == 'l') { // 'l' is a load (read)
            reads++;
        }
        else {
            // Ignore other operations
            return;
        }

        // Extract the tag and index from the address
        unsigned long tag = address >> (index_bits + offset_bits);
        unsigned long index = (address >> offset_bits) & ((1 << index_bits) - 1);

        // Check the current set for a cache hit
        bool hit = false;
        for (int i = 0; i < associativity; ++i) {
            if (cache[index][i].valid && cache[index][i].tag == tag) {
                hit = true;
                break;
            }
        }

        if (hit) {
            hits++;
            if (replacement_policy == "lru") {
                // For LRU, move the accessed block's tag to the front of the queue
                replacement_queues[index].remove(tag);
                replacement_queues[index].push_front(tag);
            }
            // For FIFO, a hit does nothing to the replacement queue.
        }
        else {
            misses++;

            // Find an empty slot
            bool found_empty_slot = false;
            for (int i = 0; i < associativity; ++i) {
                if (!cache[index][i].valid) {
                    cache[index][i].valid = true;
                    cache[index][i].tag = tag;
                    if (replacement_policy == "lru") {
                        replacement_queues[index].push_front(tag);
                    }
                    else { // fifo
                        replacement_queues[index].push_back(tag);
                    }
                    found_empty_slot = true;
                    break;
                }
            }

            // If no empty slot, perform replacement based on the policy
            if (!found_empty_slot) {
                unsigned long tag_to_evict;
                if (replacement_policy == "lru") {
                    // LRU: Evict the least recently used block (last in the list)
                    tag_to_evict = replacement_queues[index].back();
                    replacement_queues[index].pop_back();
                    replacement_queues[index].push_front(tag);
                }
                else { // fifo
                    // FIFO: Evict the oldest block (first in the list)
                    tag_to_evict = replacement_queues[index].front();
                    replacement_queues[index].pop_front();
                    replacement_queues[index].push_back(tag);
                }

                // Find the cache block with the tag to be evicted and replace it
                for (int i = 0; i < associativity; ++i) {
                    if (cache[index][i].tag == tag_to_evict) {
                        cache[index][i].tag = tag;
                        break;
                    }
                }
            }
        }
    }

    // Method to print the final simulation statistics
    void print_results() const {
        std::cout << "\n------------------------------------\n";
        std::cout << "Simulation Results\n";
        std::cout << "Total Cache Accesses: " << (hits + misses) << "\n";
        std::cout << "Total Hits: " << hits << "\n";
        std::cout << "Total Misses: " << misses << "\n";
        std::cout << "Total Reads: " << reads << "\n";
        std::cout << "Total Writes: " << writes << "\n";
        std::cout << std::fixed << std::setprecision(2);
        if (hits + misses > 0) {
            double hit_rate = (double)hits / (hits + misses) * 100;
            double miss_rate = (double)misses / (hits + misses) * 100;
            std::cout << "Hit Rate: " << hit_rate << "%\n";
            std::cout << "Miss Rate: " << miss_rate << "%\n";
        }
        else {
            std::cout << "Hit Rate: 0.00%\n";
            std::cout << "Miss Rate: 0.00%\n";
        }
        std::cout << "------------------------------------\n";
    }

    // New method to retrieve hits and misses for export
    CacheResults get_results() const {
        double hit_rate = 0.0;
        if (hits + misses > 0) {
            hit_rate = (double)hits / (hits + misses) * 100;
        }
        return { hits, misses, hit_rate };
    }

    unsigned int get_cache_size() const { return cache_size; }
    unsigned int get_associativity() const { return associativity; }
    unsigned int get_block_size() const { return block_size; }
    const std::string& get_replacement_policy() const { return replacement_policy; }
};

// Function to export results to a uniquely named CSV file
void exportResultsToCSV(const std::string& filename_base, const Cache& cache_simulator) {
    // Construct the unique filename
    std::string policyName = cache_simulator.get_replacement_policy();
    std::string output_filename = filename_base + "-" + policyName + ".csv";
    std::ofstream file(output_filename, std::ios_base::trunc); // Open in truncate mode to create a new file

    // Write the header with all parameters
    file << "Policy,Associativity,CacheSize,BlockSize,Hits,Misses,HitRate\n";

    // Write the data
    const CacheResults results = cache_simulator.get_results();
    file << policyName << ","
        << cache_simulator.get_associativity() << ","
        << cache_simulator.get_cache_size() << ","
        << cache_simulator.get_block_size() << ","
        << results.hits << ","
        << results.misses << ","
        << std::fixed << std::setprecision(2) << results.hit_rate << "\n";
    file.close();
    std::cout << "Simulation results exported to " << output_filename << "\n";
}


// Main function to run the simulator
int main() {
    unsigned int cache_size, block_size, associativity;
    std::string replacement_policy, filename;
    char op;
    unsigned long address;
    unsigned int size;

    std::cout << "This is a complete cache simulator.\n";
    std::cout << "Enter the cache size in bytes (a positive power of 2): ";
    std::cin >> cache_size;
    std::cout << "Enter the block size in bytes (a positive power of 2, must be at least 4): ";
    std::cin >> block_size;
    std::cout << "Enter the associativity (e.g., 1 for direct-mapped, N for N-way): ";
    std::cin >> associativity;
    std::cout << "Enter the replacement policy ('lru' or 'fifo'): ";
    std::cin >> replacement_policy;
    std::cout << "Enter filename: ";
    std::cin >> filename;

    // Check if the file can be opened
    std::ifstream trace_file(filename);
    if (!trace_file.is_open()) {
        std::cerr << "Error: Could not open trace file " << filename << std::endl;
        return 1;
    }

    // Create the cache object based on user input
    Cache cache_simulator(cache_size, block_size, associativity, replacement_policy);

    // Read the trace file line by line
    std::string line;
    while (std::getline(trace_file, line)) {
        if (line.empty()) continue; // Skip empty lines
        std::stringstream ss(line);
        ss >> op >> std::hex >> address >> size;
        if (ss.good() || ss.eof()) {
            cache_simulator.access(op, address);
        }
    }

    trace_file.close();

    // Print the final results to the console
    cache_simulator.print_results();

    // Export the final results to the CSV file
    exportResultsToCSV(filename, cache_simulator);

    return 0;
}
