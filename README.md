To compile type "make"
=
To run the code "./data_av" for singlethread 
and "./data_av -m" for multithread execution.

File reading:
Only formatted lines are processed.
If a line has max but, not min that line is ommited.
Total values are temperature that have been processed.
The program assumes all of the files are in the same
folder as the executable.


Report and Observations
=
	Clock Time:

	mulithread average over 50 runs = 174536
	singlethread average over 50 runs = 172704

Printing the values seems to be faster on multithread,
but the overall program takes longer to terminate.
This might be because when joining the threads a thread,
for example thread 1, could be running, but thread 3 might be finished
since pthread_join is still waiting for thread 1 to finish thread 3 waits
for thread 1 to finish to join. This might result in slower termination 
on multithread mode. Commenting out the locks and shared variables doesn't seem
to be affecting the clock time.

Randomness:
There is randomness in a sense that when printing out the values, the order in which
city's values are printing is random. Since every thread is assigned to read its own
file, whichever thread gets into the critical section faster will be printed out first.
This causes randomness. When running on linux servers the clock time is also random,
there are instances where the both of the averages of clock times are around 90k,
but in general, the average is around the given average provided. When the locks are
commented out it took 23 tries to get the wrong result due to race condition. But,
that does not mean every 23 tries one will be wrong. It is totally random. 

Synchronization:
The critical section in the program consists of comparing minimum and maximium overall
temperatures, it's relative cities, and total values processed. When running without locks
most of the time it these values are correct, but to be consistent and for getting the right
values a mutex lock is used in this section of the code.

Different OS:
This program has been ran on macOS Monterey version 12.5, this particular machine has 8 cores.
The clock time of this program is significantly slower on multithread mode around 520k, while
on singlethread mode the clock time is around 50k. But, printing the values seems to be faster
on multithread mode, but termination takes longer.
