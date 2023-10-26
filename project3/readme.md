Project 3: Results and Analysis

Naif Jabir (https://github.com/naifjabir/naif/)
===============================================

ECSE 4320: Advanced Computer Systems Fall 2023

Due: 25-Oct-2023

Commands:
=========
We use four main commands to do our testing:
Read-only -> sudo fio --filename=/dev/sdb --rw=read --direct=1 --runtime=20 --numjobs=1 --time_based --group_reporting --name=seq_read --ioengine=sync --iodepth_batch=1 --bs=4k

Write-only -> sudo fio --filename=/dev/sdb --rw=write --direct=1 --runtime=20 --numjobs=1 --time_based --group_reporting --name=seq_write --ioengine=sync --bs=32k --iodepth_batch=1 --bs=4k

50:50 Read-Write -> sudo fio --filename=/dev/sdb --rw=randrw --direct=1 --runtime=20 --numjobs=1 --time_based --group_reporting --name=50_50_random_read_and_write --ioengine=sync --iodepth_batch=1 --bs=4k

70:30 read-write -> sudo fio --filename=/dev/sdb --rw=randrw --direct=1 --runtime=20 --numjobs=1 --time_based --group_reporting --name=50_50_random_read_and_write --rwmixread=70 --ioengine=sync --iodepth_batch=1 --bs=4k

we change io_depth_batch to 1, 16, 128 and 1024 jobs issued

we change bs to 4k, 16k, 32k and 128k bytes

Command explanation:
====================

"filename" is used to designate a location where we perform our read and write test to avoid overwriting data in our disk and helps us avoid corrupting or deleting data.

/dev/sbd is the second hard drive the system uses to do scanning and operations, a place where we can safely test our fio (flexible i/o tester).

"rw" is a command used to specify what testing we are doing, read-only, write only, or a ratio of the two, or other options such as random reads and random writes.

"direct" lets us choose whether we use non-buffered i/o or not, we set this to 1 to use non-buffered i/o

"runtime" is the amount of sec we run for the test.

"numjobs" is the number of clones (processes/threads performing the same workload) of this job.

"time_based" specifies that the operation is time based, so it will keeping testing, even if it has to perform the same job (read and write of a registered we already read 
or wrote to).

"group_reporting" displays per-group reports instead of per-job when numjobs is specified.

"name" is just a name to help seperate operations when we do a lot of testing, you can change this to whatever you are comfortable with

"ioengine" defines how the job issues I/O, we use sync which does basic read(2) or write(2) I/O.

"iodepth_batch" specifies number of I/Os to submit at once (we need to use this one because iodepth cannot be used with synchronous engines).

"bs" is block size for I/O units. Default: 4k

"rwmixread" is percentage of a mixed workload that should be reads. Default: 50.

finally we have 4 rw parameters "read" for read only, "write" for write only, "randrw" for random read and write

Results And Analysis
====================
All of our charts for throughput and latency exhibits the queuing theory and one is provided directly here as an example (others can be seen in our excel sheet)
As we increase storage queue depth (hence increase data access workload stress), SSD will achieve higher resource utilization and hence higher throughput, but meanwhile the latency of each data access request will be longer

We see almost almost linear increase of the throughput, but we see an exponential increase in latency 
![image](https://github.com/naifjabir/naif/assets/144380230/a088ab33-6b4f-49f6-9de4-dee4364e296b)
![image](https://github.com/naifjabir/naif/assets/144380230/5a0ce32e-cfce-45a5-aad1-9baead00f0f4)

(one other note we wanted to include was that for some data access at the 99.5th percentile and higher, we see latency increase to 100x times the 50th percentile value for the read operations and to 10000x times the50th percentile value for the write operations)

