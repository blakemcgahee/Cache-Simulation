/*
 * This program is a fully automated test harness to run the cache simulator
 * for a predefined set of configurations. It does not require any user input.
 * It will read the "swim.trace", "gcc.trace", and other trace files, run a
 * predefined set of simulations for both LRU and FIFO replacement policies,
 * and export all the results to a single CSV file in the 'Exports' folder.
 *
 * NOTE: This version includes all trace files visible in the user's project
 * directory, such as read01.trace and write01.trace.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <list>
#include <sstream>
#include <cstdlib> // Required for the system() function

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

// A helper function to check if a number is a power of two
bool isPowerOfTwo(unsigned int n) {
    if (n == 0) return false;
    return (n & (n - 1)) == 0;
}

// Main Cache class to handle all simulation logic
class Cache {
private:
    std::vector<std::vector<CacheBlock>> cache;
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

    bool is_valid = true;

public:
    // Constructor to initialize the cache and its parameters
    Cache(unsigned int cs, unsigned int bs, unsigned int assoc, const std::string& rp)
        : cache_size(cs), block_size(bs), associativity(assoc), replacement_policy(rp) {

        try {
            if (block_size == 0 || associativity == 0 || cache_size == 0) {
                std::cerr << "Error: Cache parameters (size, block, associativity) must be non-zero.\n";
                is_valid = false;
                return;
            }

            // All sizes and associativities must be a power of two for this simple simulator.
            if (!isPowerOfTwo(cache_size) || !isPowerOfTwo(block_size) || !isPowerOfTwo(associativity)) {
                std::cerr << "Error: Cache size, block size, and associativity must be powers of two.\n";
                is_valid = false;
                return;
            }

            // Ensure cache size is a multiple of block size.
            if (cache_size % block_size != 0) {
                std::cerr << "Error: Cache size must be a multiple of block size.\n";
                is_valid = false;
                return;
            }

            // Calculate the total number of blocks.
            unsigned int total_blocks = cache_size / block_size;

            // Ensure associativity is valid
            if (associativity > total_blocks) {
                std::cerr << "Error: Invalid cache configuration. Associativity (" << associativity
                    << ") cannot be greater than the total number of blocks (" << total_blocks
                    << "). Skipping this configuration.\n";
                is_valid = false;
                return;
            }

            // Calculate the number of sets.
            num_sets = total_blocks / associativity;

            // Log2 calculations
            if (num_sets == 0 || !isPowerOfTwo(num_sets)) {
                std::cerr << "Error: Invalid cache configuration. The number of sets (" << num_sets
                    << ") must be a non-zero power of two.\n";
                is_valid = false;
                return;
            }

            offset_bits = static_cast<unsigned int>(log2(block_size));
            index_bits = static_cast<unsigned int>(log2(num_sets));
            tag_bits = 32 - index_bits - offset_bits;

            // Resize the cache and replacement queues based on the calculated number of sets and associativity.
            cache.resize(num_sets, std::vector<CacheBlock>(associativity));
            replacement_queues.resize(num_sets);
        }
        catch (const std::exception& e) {
            std::cerr << "Error during Cache initialization: " << e.what() << "\n";
            is_valid = false;
        }
    }

    bool is_cache_valid() const {
        return is_valid;
    }

    // Access method to simulate a read or write operation
    void access(char op, unsigned long address) {
        if (!is_valid) return;

        // Count reads and writes
        if (op == 's') {
            writes++;
        }
        else if (op == 'l') {
            reads++;
        }
        else {
            return; // Ignore any other operation types
        }

        // Extract the tag and index from the 32-bit address
        unsigned long tag = address >> (index_bits + offset_bits);
        unsigned long index = (address >> offset_bits) & ((1 << index_bits) - 1);

        bool hit = false;
        int hit_index = -1;

        // Check for a cache hit
        for (int i = 0; i < associativity; ++i) {
            if (cache[index][i].valid && cache[index][i].tag == tag) {
                hit = true;
                hit_index = i;
                break;
            }
        }

        if (hit) {
            hits++;
            if (replacement_policy == "lru") {
                replacement_queues[index].remove(tag);
                replacement_queues[index].push_front(tag);
            }
        }
        else {
            misses++;

            bool found_empty_slot = false;
            int empty_slot_index = -1;
            for (int i = 0; i < associativity; ++i) {
                if (!cache[index][i].valid) {
                    empty_slot_index = i;
                    found_empty_slot = true;
                    break;
                }
            }

            if (found_empty_slot) {
                cache[index][empty_slot_index].valid = true;
                cache[index][empty_slot_index].tag = tag;
                if (replacement_policy == "lru") {
                    replacement_queues[index].push_front(tag);
                }
                else if (replacement_policy == "fifo") {
                    replacement_queues[index].push_back(tag);
                }
            }
            else {
                unsigned long tag_to_evict;
                if (replacement_policy == "lru") {
                    tag_to_evict = replacement_queues[index].back();
                    replacement_queues[index].pop_back();
                    replacement_queues[index].push_front(tag);
                }
                else if (replacement_policy == "fifo") {
                    tag_to_evict = replacement_queues[index].front();
                    replacement_queues[index].pop_front();
                    replacement_queues[index].push_back(tag);
                }

                for (int i = 0; i < associativity; ++i) {
                    if (cache[index][i].tag == tag_to_evict) {
                        cache[index][i].tag = tag;
                        break;
                    }
                }
            }
        }
    }

    // Method to retrieve hits and misses for export
    CacheResults get_results() const {
        double hit_rate = 0.0;
        if (hits + misses > 0) {
            hit_rate = (double)hits / (hits + misses) * 100;
        }
        return { hits, misses, hit_rate };
    }

    // Getter methods for the parameters
    unsigned int get_cache_size() const { return cache_size; }
    unsigned int get_associativity() const { return associativity; }
    unsigned int get_block_size() const { return block_size; }
    const std::string& get_replacement_policy() const { return replacement_policy; }
};

// Function to write a single result row to a CSV file
void writeResultToCSV(std::ofstream& file, const Cache& cache_simulator, const std::string& trace_filename) {
    const CacheResults results = cache_simulator.get_results();
    file << cache_simulator.get_replacement_policy() << ","
        << cache_simulator.get_associativity() << ","
        << cache_simulator.get_cache_size() << ","
        << cache_simulator.get_block_size() << ","
        << results.hits << ","
        << results.misses << ","
        << std::fixed << std::setprecision(2) << results.hit_rate << ","
        << trace_filename << "\n";
}

// Struct to hold a single test case
struct TestCase {
    unsigned int cache_size;
    unsigned int block_size;
    unsigned int associativity;
    std::string replacement_policy;
    std::string trace_filename;
};

// Main function to run all simulations
int main() {
    std::vector<TestCase> test_cases = {
        // --- swim.trace with LRU ---
        {1024, 64, 1, "lru", "swim.trace"},
        {2048, 64, 1, "lru", "swim.trace"},
        {4096, 64, 1, "lru", "swim.trace"},
        {8192, 64, 1, "lru", "swim.trace"},
        {16384, 64, 1, "lru", "swim.trace"},

        {1024, 64, 4, "lru", "swim.trace"},
        {2048, 64, 4, "lru", "swim.trace"},
        {4096, 64, 4, "lru", "swim.trace"},
        {8192, 64, 4, "lru", "swim.trace"},
        {16384, 64, 4, "lru", "swim.trace"},

        {16384, 16, 4, "lru", "swim.trace"},
        {16384, 32, 4, "lru", "swim.trace"},
        {16384, 64, 4, "lru", "swim.trace"},
        {16384, 128, 4, "lru", "swim.trace"},

        {16384, 64, 1, "lru", "swim.trace"},
        {16384, 64, 2, "lru", "swim.trace"},
        {16384, 64, 4, "lru", "swim.trace"},
        {16384, 64, 8, "lru", "swim.trace"},
        {16384, 64, 256, "lru", "swim.trace"},

        // --- swim.trace with FIFO ---
        {1024, 64, 1, "fifo", "swim.trace"},
        {2048, 64, 1, "fifo", "swim.trace"},
        {4096, 64, 1, "fifo", "swim.trace"},
        {8192, 64, 1, "fifo", "swim.trace"},
        {16384, 64, 1, "fifo", "swim.trace"},

        {1024, 64, 4, "fifo", "swim.trace"},
        {2048, 64, 4, "fifo", "swim.trace"},
        {4096, 64, 4, "fifo", "swim.trace"},
        {8192, 64, 4, "fifo", "swim.trace"},
        {16384, 64, 4, "fifo", "swim.trace"},

        {16384, 16, 4, "fifo", "swim.trace"},
        {16384, 32, 4, "fifo", "swim.trace"},
        {16384, 64, 4, "fifo", "swim.trace"},
        {16384, 128, 4, "fifo", "swim.trace"},

        {16384, 64, 1, "fifo", "swim.trace"},
        {16384, 64, 2, "fifo", "swim.trace"},
        {16384, 64, 4, "fifo", "swim.trace"},
        {16384, 64, 8, "fifo", "swim.trace"},
        {16384, 64, 256, "fifo", "swim.trace"},

        // --- gcc.trace with LRU ---
        {1024, 64, 1, "lru", "gcc.trace"},
        {2048, 64, 1, "lru", "gcc.trace"},
        {4096, 64, 1, "lru", "gcc.trace"},
        {8192, 64, 1, "lru", "gcc.trace"},
        {16384, 64, 1, "lru", "gcc.trace"},

        {1024, 64, 4, "lru", "gcc.trace"},
        {2048, 64, 4, "lru", "gcc.trace"},
        {4096, 64, 4, "lru", "gcc.trace"},
        {8192, 64, 4, "lru", "gcc.trace"},
        {16384, 64, 4, "lru", "gcc.trace"},

        {16384, 16, 4, "lru", "gcc.trace"},
        {16384, 32, 4, "lru", "gcc.trace"},
        {16384, 64, 4, "lru", "gcc.trace"},
        {16384, 128, 4, "lru", "gcc.trace"},

        {16384, 64, 1, "lru", "gcc.trace"},
        {16384, 64, 2, "lru", "gcc.trace"},
        {16384, 64, 4, "lru", "gcc.trace"},
        {16384, 64, 8, "lru", "gcc.trace"},
        {16384, 64, 256, "lru", "gcc.trace"},

        // --- gcc.trace with FIFO ---
        {1024, 64, 1, "fifo", "gcc.trace"},
        {2048, 64, 1, "fifo", "gcc.trace"},
        {4096, 64, 1, "fifo", "gcc.trace"},
        {8192, 64, 1, "fifo", "gcc.trace"},
        {16384, 64, 1, "fifo", "gcc.trace"},

        {1024, 64, 4, "fifo", "gcc.trace"},
        {2048, 64, 4, "fifo", "gcc.trace"},
        {4096, 64, 4, "fifo", "gcc.trace"},
        {8192, 64, 4, "fifo", "gcc.trace"},
        {16384, 64, 4, "fifo", "gcc.trace"},

        {16384, 16, 4, "fifo", "gcc.trace"},
        {16384, 32, 4, "fifo", "gcc.trace"},
        {16384, 64, 4, "fifo", "gcc.trace"},
        {16384, 128, 4, "fifo", "gcc.trace"},

        {16384, 64, 1, "fifo", "gcc.trace"},
        {16384, 64, 2, "fifo", "gcc.trace"},
        {16384, 64, 4, "fifo", "gcc.trace"},
        {16384, 64, 8, "fifo", "gcc.trace"},
        {16384, 64, 256, "fifo", "gcc.trace"},

        // --- Additional trace files from user's directory ---
        {1024, 64, 4, "lru", "read01.trace"},
        {2048, 64, 4, "fifo", "read02.trace"},
        {4096, 128, 8, "lru", "read03.trace"},
        {8192, 32, 2, "fifo", "read04.trace"},
        {16384, 64, 4, "lru", "read05.trace"},
        {1024, 16, 1, "fifo", "read06.trace"},
        {2048, 64, 8, "lru", "read08.trace"},
        {4096, 64, 4, "fifo", "write01.trace"}
    };

    system("mkdir Exports");

    std::ofstream output_file("Exports/all_results.csv", std::ios_base::trunc);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not create output file Exports/all_results.csv. "
            << "Please check file permissions.\n";
        return 1;
    }
    output_file << "Policy,Associativity,CacheSize,BlockSize,Hits,Misses,HitRate,TraceFile\n";

    for (const auto& test_case : test_cases) {
        std::cout << "------------------------------------\n";
        std::cout << "Running simulation for:\n";
        std::cout << " - Replacement Policy: " << test_case.replacement_policy << "\n";
        std::cout << " - Cache Size: " << test_case.cache_size << " bytes\n";
        std::cout << " - Block Size: " << test_case.block_size << " bytes\n";
        std::cout << " - Associativity: " << test_case.associativity << "-way\n";
        std::cout << " - Trace File: " << test_case.trace_filename << "\n";
        std::cout << "------------------------------------\n";

        Cache cache_simulator(test_case.cache_size,
            test_case.block_size,
            test_case.associativity,
            test_case.replacement_policy);

        if (!cache_simulator.is_cache_valid()) {
            std::cout << "Skipping this invalid configuration.\n";
            continue;
        }

        std::ifstream trace_file(test_case.trace_filename);
        if (!trace_file.is_open()) {
            std::cerr << "Error: Could not open trace file '" << test_case.trace_filename << "'. Please ensure the file exists and is in the current working directory.\n";
            continue;
        }

        char op;
        unsigned long address;
        unsigned int size;
        std::string line;
        unsigned int lines_read = 0;
        unsigned int successful_accesses = 0;
        while (std::getline(trace_file, line)) {
            lines_read++;
            if (line.empty()) continue;
            std::stringstream ss(line);
            ss >> op >> std::hex >> address >> size;
            if (ss.good() || ss.eof()) {
                cache_simulator.access(op, address);
                successful_accesses++;
            }
            else {
                std::cerr << "Warning: Skipping malformed line " << lines_read << ": '" << line << "'\n";
            }
        }
        trace_file.close();

        writeResultToCSV(output_file, cache_simulator, test_case.trace_filename);
        std::cout << "Results appended to all_results.csv\n";
    }

    output_file.close();
    std::cout << "\nAll simulations have been completed. Check the 'Exports' folder for your single CSV file.\n";
    std::cout << "If the program still failed, please check the console for specific error messages.\n";

    return 0;
}
