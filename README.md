# linux-system-monitoring-tool
C program that will report different metrics of the utilization of a linux system as described below.

The program should accept several command line arguments:

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

 


How did I solve the problem?

For the problem of acquiring and reading all the data I needed I looked over the listed modules, documentation, and files on the assignment page. I started with the system information data. At first I started looking through a bunch of files such as /proc/version, /proc/sys/kernel/, etc. But then I realized the listed modules did all the work for me. I used <sys/utsname.h> for the system information, <utmp.h> for the user data, and <sys/sysinfo.h> for the memory data. These modules did the work of acquiring the data for me. For the cpu info however, I read the file /proc/stat & /proc/cpuinfo and used strtol() get the required data. 

For the problem of calculating cpu usage I saw the website linked on piazza 127: https://www.kgoettler.com/post/proc-stat/ From where I got the formula for calculating cpu usage. This formula makes sense to me. CPU usage is basically the percent of time in your sample where the cpu is actually running and doing calculations and stuff, while the rest of the time is where the cpu stays idle or waiting. 

For the problem of refreshing the screen and getting the output to look like the demo video, I researched ESCape codes, and found a code which allowed me to clear the terminal. After this I just stored my memory samples in arrays and messed around with loops to solve the display and refresh problem. I had my arrays that stored samples then I basically had two loops, one that printed samples and one that printed new lines. So as the program ran I had variables that made the new line loop run fewer loops and the sample loop run more loops. 

For the problem of command line arguments: I just had a variable flags for --system, --user, --sequential. So when I checked for the arguments using if statements I would update the corresponding variables and have the program call certain functions based on the flags. Then for samples and delays I just made clever use of which position the '=' sign was to differentiate. 

Funtions:

void print_mem_info(int samples, int i, float total_ram[], float used_ram[], float total_swap[], float used_swap[])

This function takes in float arrays total_ram[], used_ram[], total_swap[], and used_swap[]. These arrays contain the memory sample data which is used for printing. The int samples and int i are to determine how many samples have already been taken by just doing samples - i, since i was an iterator from the loop that the function gets called from. This allows print_mem_info to print the correct number of blank lines and memory samples to help give the refresh effect.

void print_system_information()

This function makes use of the utsname module and prints the required system information. 

void print_users()

This function makes use of the utmp module and prints the user data.

void set_times(long int *user, long int *nice, long int *system, long int *idle, long int *iowait, long int *irq, long int *softirq, long int *steal)

This function takes in pointers for all those cpu time measurements found in /proc/stat. The function reads the first line from /proc/stat and uses strtol() to store the times in the addresses given by the pointers.

void print_cpu_info(long int *prev_t_total, long int *prev_t_usage)

This function takes in long int *prev_t_total, and long int *prev_t_usage. The values stored in these addresses are needed for the function to calculate cpu usage and print it. The calculation used for cpu usage is found at https://www.kgoettler.com/post/proc-stat/ Once that is done the function takes the new t_total and new t_usage it calculated, and assigns it to *prev_t_total and *prev_t_usage. So that when the function gets called again with these pointers it's then able to make another cpu usage calculation.

How to run the program: 
You may run any of these 3 variations.

	No arguments other than name of executable:	./a.out 
	2 arguments for samples and delay without "--":	./a.out 8 3
	Any number of arguments but all have "--":	
	./a.out --system --sequential --tdelay=3 --sample=15


