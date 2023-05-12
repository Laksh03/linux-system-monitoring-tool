#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>


//print_mem_info prints the memory data stored in the array arguments
void print_mem_info(int samples, int i, float total_ram[], float used_ram[], float total_swap[], float used_swap[]){
	printf("---------------------------------------\n");
	printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
	
	//This for loop prints the memory samples stored in array arguments
	for(int x = 0; x < i + 1; x++){
		printf("%.2f GB / %.2f GB  -- %.2f GB / %.2f GB\n", used_ram[x], total_ram[x], used_swap[x], total_swap[x]);
	}
	
	//This for loop prints new lines for the specific amount of memory samples yet to be taken using int samples and int i.
	for(int x = 0; x < samples - i - 1; x++){
		printf("\n");
	}
	
	printf("---------------------------------------\n");
}


//print_system_information prints system information using utsname module.
void print_system_information(){
	//struct from utsname module stores system information
	struct utsname *system_information;
	system_information = malloc(sizeof(struct utsname));
	
	//uname function initializes the fields of struct utsname system_information
	int num = uname(system_information);

	//Checking for errors
	if(num == -1){
		perror("uname");
	}

	//Printing all system information from system_information fields.
	printf("---------------------------------------\n");
	printf("### System Information ###\n");
	printf(" System Name = %s\n", system_information->sysname);
	printf(" Machine Name = %s\n", system_information->nodename);
	printf(" Version = %s\n", system_information->version);
	printf(" Release = %s\n", system_information->release);
	printf(" Architecture = %s\n", system_information->machine);
	printf("---------------------------------------\n");	

	free(system_information);
}


//print_users function prints current users using utmp module
void print_users(){
	struct utmp *info; // struct utmp stores info about user sessions
	setutent(); //opens utmp file for reading

	printf("### Sessions/users ###\n");
	info = getutent(); //reads from utmp file
	while (info != NULL) {
		if (info->ut_type == USER_PROCESS) { //check if info is type USER_PROCESS 
			printf("%s       %s %s\n", info->ut_user, info->ut_line, info->ut_host);
		}
		info = getutent(); // reads from utmp file
	}
	endutent(); // closes utmp file;
	printf("---------------------------------------\n");
}


//set_times takes in pointers for each category of cpu time usage shown in /proc/stat. Then reads the file and assigns the corresponding values to pointers. 
void set_times(long int *user, long int *nice, long int *system, long int *idle, long int *iowait, long int *irq, long int *softirq, long int *steal){
	FILE *file = fopen("/proc/stat", "r");
	char *line = malloc(sizeof(char) * 400);
	char *next;
	fgets(line, 400, file);
	
	*user = strtol(line+4, &next, 10);
	*nice = strtol(next, &next, 10);
	*system = strtol(next, &next, 10);
	*idle = strtol(next, &next, 10);
	*iowait = strtol(next, &next, 10);
	*irq = strtol(next, &next, 10);
	*softirq = strtol(next, &next, 10);
	*steal = strtol(next, &next, 10);

	free(line);
	fclose(file);
}

//print_cpu_info calulates & prints the cpu usage. Function takes in pointers to previous total cpu time and total cpu usage time. 
void print_cpu_info(long int *prev_t_total, long int *prev_t_usage){
	FILE *file = fopen("/proc/cpuinfo", "r");
	char *line = malloc(sizeof(char) * 400);

	//Reading the first 12 lines of /proc/cpuinfo to get to cores per cpu line. 
	for(int i = 0; i < 12; i++){
		fgets(line, 400, file);
	}

	free(line);
	line = malloc(sizeof(char) * 400);
	fgets(line, 400, file);

	fclose(file);

	//printing cores per cpu. 
	printf("Number of %s", line);
	free(line);

	//The current total cpu time and usage time are needed for cpu usage calculation. So we initialize pointers for the cpu times to be stored.
	long int *user = malloc(sizeof(long int));
	long int *nice = malloc(sizeof(long int));
	long int *system = malloc(sizeof(long int));
	long int *idle = malloc(sizeof(long int)); 
	long int *iowait = malloc(sizeof(long int)); 
	long int *irq = malloc(sizeof(long int)); 
	long int *softirq = malloc(sizeof(long int));
	long int *steal = malloc(sizeof(long int));
	long int t_total, t_idle, t_usage;
	float cpu_usage_percent;

	//Reading current cpu times through set_times function call.
	set_times(user, nice, system, idle, iowait, irq, softirq, steal);

	//Calculating the percent of cpu use
	t_total = *user + *nice + *system + *idle + *iowait + *irq + *softirq + *steal;
	t_idle = *idle + *iowait;
	t_usage = t_total - t_idle - *prev_t_usage;
	t_total -= *prev_t_total;
	cpu_usage_percent = ((float)t_usage/t_total) * 100;

	//printing cpu usage.
	printf("CPU Usage: %.2f\n", cpu_usage_percent);

	//Setting prev total and usage time to current values to prepare for the next cpu usage sample calculation
	*prev_t_total = t_total + *prev_t_total;
	*prev_t_usage = t_usage + *prev_t_usage;

	free(user);
	free(nice);
	free(system);
	free(idle);
	free(iowait);
	free(irq);
	free(softirq);
	free(steal);
}


int main(int argc, char **argv){
	//Clearing terminal 
	printf("\033c");

	//Declaring and initializing default sample and time (delay). As well as the flags. 
	int samples = 10, time = 1;
	int system_flag = 0, user_flag = 0, sequential_flag = 0;
	int iteration = 1;

	//if command line arguments present but arguments don't have '-', then samples & time are updated from default 10 samples and 1 sec to user inputs.
	if(argc != 1 && argv[1][0] != '-'){
		samples = strtol(&argv[1][0], NULL, 10);
		time = strtol(&argv[2][0], NULL, 10);
	}else{ //Command line arguments have '-'
		for(int i = 1; i < argc; i++){ // loop through all command line arguments checking the type of argument and updating corresponding flag.
			if(strcmp(&argv[i][0], "--system") == 0){
				system_flag = 1;
			}else if(strcmp(&argv[i][0], "--user") == 0){
				user_flag = 1;
			}else if(strcmp(&argv[i][0], "--sequential") == 0){
				sequential_flag = 1;
			}else if(argv[i][9] == '=' && strlen(&argv[i][0]) > 10){ // Check for "--samples=n" argument
				samples = strtol(&argv[i][10], NULL, 10); // Updating samples variable with user input
			}else if(argv[i][8] == '=' && strlen(&argv[i][0]) > 9){ // check for "--tdelay=T" argument
				time = strtol(&argv[i][9], NULL, 10); // Updating time (delay) variable with user input.
			}
		}
	}



	//struct sysinfo will store all memory data within its fields
	struct sysinfo *meminfo = malloc(sizeof(struct sysinfo));
	int num;
	float total_ram[samples], used_ram[samples], total_swap[samples], used_swap[samples]; // Arrays to store all memory samples taken. 

	//Declaring pointers to get calculate the prev total time & usage values for the cpu usage calculation. 
	long int *user = malloc(sizeof(long int));
	long int *nice = malloc(sizeof(long int));
	long int *system = malloc(sizeof(long int));
	long int *idle = malloc(sizeof(long int)); 
	long int *iowait = malloc(sizeof(long int)); 
	long int *irq = malloc(sizeof(long int)); 
	long int *softirq = malloc(sizeof(long int));
	long int *steal = malloc(sizeof(long int));
	long int *prev_t_total = malloc(sizeof(long int));
	long int *prev_t_usage = malloc(sizeof(long int));

	//Initializing the times by using set_times function to read through /proc/stat
	set_times(user, nice, system, idle, iowait, irq, softirq, steal);
	*prev_t_total = *user + *nice + *system + *idle + *iowait + *irq + *softirq + *steal;
	*prev_t_usage = *prev_t_total - *idle - *iowait;
	
	free(user);
	free(nice);
	free(system);
	free(idle);
	free(iowait);
	free(irq);
	free(softirq);
	free(steal);


	//Short delay for difference between cpu time reads. 
	sleep(1);

	//for loop for the samples that will be taken.
	for(int i = 0; i < samples; i++){
		
		printf("Nbr of samples: %d -- every %d secs\n", samples, time);
		
		//if in suquential mode the loop will print out the interation number. 
		if(sequential_flag == 1){
			printf(">>> iteration %d\n", iteration);
			iteration++;
		}
		
		//If --user argument was not present, then memory info will be printed.
		if(user_flag != 1){
			num = sysinfo(meminfo); // initializing memory data into the fields of meminfo.

			//Error checking. 
			if(num == -1){
				perror("sysinfo");
			}
			
			//Storing the memory samples into arrays for printing
			total_ram[i] = (float)(meminfo->totalram * meminfo->mem_unit) * 9.31 * pow(10, -10);
			used_ram[i] = total_ram[i] - (float)(meminfo->freeram * meminfo->mem_unit) * 9.31 * pow(10, -10);
			total_swap[i] = (float)((meminfo->totalram + meminfo->totalswap) * meminfo->mem_unit) * 9.31 * pow(10, -10);
			used_swap[i] = total_swap[i] - (float)((meminfo->freeswap + meminfo->freeram)* meminfo->mem_unit) * 9.31 * pow(10, -10);	
			
			//Printing memory samples by sending the arrays and loop data as arguments to get desired printing outputs
			print_mem_info(samples, i, total_ram, used_ram, total_swap, used_swap);
		}
		
		//If --system argument was not entered then print user information
		if(system_flag != 1){
			print_users();
		}

		//If --user argument was not entered then print cpu information.
		if(user_flag != 1){
			print_cpu_info(prev_t_total, prev_t_usage);
		}

		//Function call to print system information
		print_system_information();
		
		//The delay between samples
		sleep(time);

		//If --sequential argument was not entered then erase screen for new outputs. 
		if(sequential_flag != 1){
			if(i != samples - 1){
				printf("\033c");
			}
		}
	}


	free(prev_t_total);
	free(prev_t_usage);
	free(meminfo);

	return 0;
}