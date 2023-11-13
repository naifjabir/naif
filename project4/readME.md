Project 1: Results and Analysis
===============================
Submission Information
Naif Jabir (https://github.com/naifjabir)

ECSE 4320: Advanced Computer Systems Fall 2023

Due: 11-Nov-2023

(1) Readme that clearly explains the structure/usage of your code, and how your code utilizes multi-threading and SIMD to speed up
==================================================================================================================================
The code is structured into 4 files, 1 being the dictionary encoding and creation file and the other 3 being the query files that give the run time for a element search
the dictionary file has 3 main parts, the "read_raw_column" to get the list of string values we have in our column.txt file, the "parallel_dictionary_encode" to make a dictionary of values-key pairs to help us define our list of indices we make right after and finally the "write_output" to put our results into an output file to read from later. We also make sure to use adjacent_differences to decrease the footprint of the data. Next up we have the three query files which have the same structure. We read in the input file (which is the output file we created from our dictionary encoding) and then do different types of search. Our vanilla search just uses the original cloumn.txt file to find the indices, we search through the dictionary then the list of indices afterwards and finally we optimize the dictionary and indices lookup with SIMD instructions.
The multi-threads help to divide the workload and do similar instructions together at once, and because they are all working at the same time, they reduce execution time by a large amount. The SIMD instructions help to optimize the memory access and we can operate on multiple data items at once so we can do the value comparisons much faster. The threads does seem to have a limit of sorts that we will explain later and the SIMD seems to have some trouble with memory access and deallocation which is why we seperated the SIMD versions of key_search and prefix_key_search into two different files.

(2) Comprehensive experimental results that show the performance of your implementation (both encoding performance and query performance). When measuring the performance, do not count the time of loading file to memory and writing file to SSD. The performance results must contain: 
==================================================================================================================================
    (i) encoding speed performance under different number of threads, 
        Dictionary Creation Time (numbers of threads = 1): 91.1591 seconds
        Dictionary Creation Time (numbers of threads = 4): 33.8162 seconds
        Dictionary Creation Time (numbers of threads = 8): 25.9808 seconds
        Dictionary Creation Time (numbers of threads = 16): 34.3115 seconds
        Dictionary Creation Time (numbers of threads = 32): 45.702 seconds
        Dictionary Creation Time (numbers of threads = 64): 90.1522 seconds
        Dictionary Creation Time (numbers of threads = 128): 109.047 seconds
        Dictionary Creation Time (numbers of threads = 512): 225.48 seconds
        Dictionary Creation Time (numbers of threads = 1024): 240.237 seconds
        
    (ii) single data item search speed performance of your vanilla baseline, dictionary without using SIMD, and dictionary with SIMD,
        Vanilla Search_key_indices Time: 1.29405 seconds
        Dic Search_key_indices Time: 3.6e-06 seconds
        Search_key_indices_simd Time: 4.2e-06 seconds
        
    (iii) prefix scan speed performance of your vanilla baseline, dictionary without using SIMD, and dictionary with SIMD.
        Vanilla Search_key_indices_and_prefixes Time: 1.46866 seconds
        Dic Search_key_indices_and_prefixes Time: 0.0332101 seconds
        Search_key_indices_and_prefixes_simd Time: 4.9e-06 seconds

(3) Show the effectiveness of dictionary coding on reducing the column data size both in memory and on SSD (use integer compression to further reduce on-SSD data footprint)
===================================================================================================================================
Dictionary coding is effective in reducing the column data size both in memory and on SSD by eliminating redundancy and decreasing footprint by using integer compression. This optimizes storage and enhances query performance, making it a valuable technique for managing and storing large datasets. When we have a lot of overlapping values, we can put them all into a dictionary pair and reduce the overall file size for each of these repetitive values and even if all the values are unique, the dictionary makes it much easier to traverse through memory and the integer compression decreases the number of bits we are using so they can be freed up to use for other purposes. This compression is incredibly useful because we see that the data regularly gets into the 1,500,000 range and higher for the indices it scans the data column so it is much easier to store smaller integers.

(4) Analysis and conclusion
===========================
For the effect of threads on our encoding, we see that at a certain point, we get diminishing returns on the number of threads, mainly because we don't have that many CPU cores to work with so we see an optimal number at 8 since that is how many this laptop has. As for SIMD, it drastically decreases the amount of time needed to search through the indices by dividing up the workload and stress so that the execution of each operation goes by much faster. This wasn't the case for the dictionary without SIMD case for the single item search but this may be because the search needed used 1 item, so it was unnecessary to uses more streams to go through a simple loop. However, with the multiple loops from the list of values we get from our selected key, we have lot more loops to go through, so the SIMD is much more optimized. Also there seems to be an issue where it doesn't like having the registers for the SIMD used too much so i decided to divide up the code into more files otherwise we keep getting more segmentation faults by the double freeing the SIMD does.
In conclusion, dictionary encoding is really useful and helps to optimize item search of a list of values and helps to decrease the memory so we can search through much faster. Multi-threading and SIMD further reduce the run time by magnitudes when compared to the vanilla search versions. 
