# linux-system-monitoring-tool

--system
        to indicate that only the system usage should be generated


--user

        to indicate that only the users usage should be generated


--graphics  (+2 bonus points)

        to include graphical output in the cases where a graphical outcome is possible as indicated below.


--sequential

        to indicate that the information will be output sequentially without needing to "refresh" the screen (useful if you would like to redirect the output into a file)

 

--samples=N

        if used the value N will indicate how many times the statistics are going to be collected and results will be average and reported based on the N number of repetitions.
If not value is indicated the default value will be 10.


--tdelay=T

        to indicate how frequently to sample in seconds.
If not value is indicated the default value will be 1 sec.

 

The last two arguments can also be considered as positional arguments if not flag is indicated in the corresponding order: samples tdelay.
