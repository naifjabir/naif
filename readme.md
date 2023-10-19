Project 2: Structure/Installation/Usage

Submission information: 
-----------------------------------------------
Naif Jabir (https://github.com/naifjabir/naif/)

ECSE 4320: Advanced Computer Systems Fall 2023

Due: 18-Oct-2023

Disclaimer: I have been trying to modify the code to work but I cannot get the last two implementations to work at all so I will simply submit the first 5 combinations and
experimental data for the last from prototype testing.


Experiment parameters include: 
Matrix Sizes: 1000, 2000, 5000, 10000
Data Types: 2 byte fixed, 4 byte fixed, 4 byte float, 8 byte float


Structure/usage/installation of code
====================================================================================================
The first thing I wanted to ensure and test is how much time will each test take ain the worst case?
One operation takes about 1E-8 to do.
I have determined that the amount of time for a basic matrix multiplication is about 1E-8*(MATRIX_SIZE^3)
so realistically, a 5,000x5,000 time will be about 20 minutes and a 10,000x10,000 will be 2.78 hours
I will list the times for basic matrix multiplication for these
The code is configured such that user input picks from 8 choices, 7 being a combination or stand alone of the 3 optimization techniques or the 8th choice being the basic matrix multiplication with no optimization.

The code is structured based on the order of the techniques and the number picked from 1-8 lets users choose what runs they want to see. The code also has user input taken for number of threads whenever we use that technique. The usage of functions decrease the total code as much as possible and comments are added here and there to keep track of what function will run what techniques and optimzation and how. The installation of the code is very easy, you just need to download a few files and you can run it on any linex distribution.
(the distribution will need to have the AVX and Pthread packages installed.)

Conclusion
===================================================================================================
We clearly see that combining 2 of the 3 of the optimization wields a better run time in most cases and combining all 3 techniques allow us to reach record low times that handle data structures of 100 million elements easily. Optimizing these techniques for our systems will give us more system perfornance increase and allows us to make computers cheaper as well as make them more useful for more tasks. We are in an age where we are reaching a upper end to our transistor sizes, to how small our machines can be and and closer to an end of what the best performing computer can give for a certain volume. We will have to focus even more on these optimization research and find ways to we will have to work with the same limitations as we have now, however we have shown that we haven't fully optimized our system just yet, so we definately still have some work left to do.