/*
 * main.c
 * 
 * @author George Barker
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

int const FCFS = 1;
int const SJF = 2;
int const RR = 3;

int const MANUAL_INPUT = 1;
int const INPUT_FROM_FILE = 2;

// Boolean type is not always available in every version of C, so it has been defined here.
typedef enum { false, true } bool;


//Some variables that are used by all algorithms, and so can be global.
int totalBurstTime;
float averageWaitTime, averageTurnAroundTime;

/*
A struct that models a Process has been used to make the code more readable,
as oposed to using multiple arrays. Full names have been given to all the properties,
to ensure that the reader knows exactly what the properties mean. (i.e., processId instead of PID).
*/
struct Process {
    int processId;
    int arrivalTime;
    int burstTime;
    int turnAroundTime;
    int waitTime;
    int completionTime;
    int remainingBurstTime;
};

/*
This method is used to cleanly retrieve the algorithm the user wishes to perform.
*/
int algorithmSelection() {
    int selection;
    printf("Welcome to the scheduling algorithm program.\n"
            "Which algorithm do you want to run?\n"
            "1. First Come First Served (FCFS)\n"
            "2. Shortest Job First (SJF)\n"
            "3. Round Robin (RR)\n\n"
            "Enter a selection: ");
    scanf("%d", &selection);
    return selection;
}

/*
This method is used to cleanly retrieve the way in which the user wants to enter their processes;
either manually on the CLI or from a CSV file.
*/
int processSelection() {
    int selection;
    printf("How do you want to enter your processes?\n\n"
            "1. Enter processes manually\n"
            "2. Use processes from a CSV file\n"
            "3. Exit\n\n"
            "Enter a selection: ");

    scanf("%d", &selection);
    return selection;

}

/*
A utility method to sort an array of Processes by their arrival time in ascending order.
*/
void sortProcessesByArrivalTime(struct Process* processes,
        int numberOfProcesses) {
    int j, i;
    for (i = 1; i < numberOfProcesses; i++) {
        for (j = 0; j < numberOfProcesses - i; j++) {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

/*
A utility method to sort an array of Processes by their burst time in ascending order.
*/
void sortProcessesByBurstTime(struct Process* processes,
        int numberOfProcesses) {
    int j, i;
    for (i = 1; i < numberOfProcesses; i++) {
        for (j = 0; j < numberOfProcesses - i; j++) {
            if (processes[j].burstTime > processes[j + 1].burstTime) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

/*
The display method is the final step of the program;
it takes the Processes and outputs them to the user on the screen,
as well as creating a text file so that they can easily re-use the output.
The file name uses the current UNIX timestamp, to avoid issues with name clashing,
and to uniquely distinguish each file.
*/
void display(struct Process processes[10], int numberOfProcesses) {
	FILE *file;
	char fileName[32];
	sprintf(fileName, "scheduling-result-%d.txt", (int) time(NULL));
	file = fopen(fileName, "w+");
	
    printf("PID\tAT\tBT\tCT\tTaT\tWT\n");
    fprintf(file, "PID\tAT\tBT\tCT\tTaT\tWT\n");
    for (int i = 0; i < numberOfProcesses; i++) {
        printf("%d\t%3d\t%3d\t%3d\t%3d\t%3d\n", 
        processes[i].processId,
        processes[i].arrivalTime,
        processes[i].burstTime,
        processes[i].completionTime,
        processes[i].turnAroundTime,
        processes[i].waitTime);
        
        fprintf(file, "%d\t%3d\t%3d\t%3d\t%3d\t%3d\n",
        processes[i].processId,
        processes[i].arrivalTime,
        processes[i].burstTime,
        processes[i].completionTime,
        processes[i].turnAroundTime,
        processes[i].waitTime);
    }

    printf("Average Turn Around Time: %.2f\nAverage Wait Time: %.2f\n",
            averageTurnAroundTime, averageWaitTime);
	fprintf(file, "Average Turn Around Time: %.2f\nAverage Wait Time: %.2f\n",
            averageTurnAroundTime, averageWaitTime);
            
	fclose(file);
    
    printf("The result has been outputted to the current working directory.\nFile name: %s\n", fileName);
    
}

/*
Advantages of FCFS:
 	- It is a simple algorithm
 	- It is easy to understand
 	- Suitable for batch systems

Disadvantages of FCFS:
	- It is non pre-emptive, meaning processes MUST run until they've finished.
	- Processes that are at the back of the queue that could have processed quickly, 
		have to wait for potentially longer processes that arrived first.
	- It is not very efficient in terms of I/O
	- It is not suitable for instances where it is important that each processes 
	is given equal amounts of processing time.
*/
void firstComeFirstServed(struct Process processes[10], int numberOfProcesses) {
	//Sort processes by arrival time, because we're processing them first come, first served.
    sortProcessesByArrivalTime(processes, numberOfProcesses);

    //Do work on the first process.
    processes[0].waitTime = 0;
    processes[0].completionTime = processes[0].arrivalTime + processes[0].burstTime;
    averageTurnAroundTime = processes[0].turnAroundTime =
            processes[0].burstTime;
    totalBurstTime = processes[0].burstTime;

    for (int i = 1; i < numberOfProcesses; i++) {
    	//We can now calculate future processes.
		processes[i].completionTime = processes[i - 1].completionTime + processes[i].burstTime;
        processes[i].waitTime = totalBurstTime - processes[i].arrivalTime;
        totalBurstTime += processes[i].burstTime;
        averageWaitTime += processes[i].waitTime;
        processes[i].turnAroundTime = processes[i].waitTime
                + processes[i].burstTime;
        averageTurnAroundTime += processes[i].turnAroundTime;
    }
    averageTurnAroundTime /= numberOfProcesses;
    averageWaitTime /= numberOfProcesses;

    display(processes, numberOfProcesses);
}

void shortestJobFirst(struct Process processes[10], int numberOfProcesses) {
	//Sort processes by arrival time, in order to retrieve the first process.
    sortProcessesByArrivalTime(processes, numberOfProcesses);
    //Do work on the first Process.
    processes[0].waitTime = 0;
    processes[0].completionTime = processes[0].burstTime + processes[0].arrivalTime;
    averageTurnAroundTime = processes[0].turnAroundTime = processes[0].completionTime - processes[0].arrivalTime;
    averageWaitTime = processes[0].waitTime = processes[0].turnAroundTime - processes[0].burstTime;
    totalBurstTime = processes[0].burstTime;
    
    //Now we need to get an array of Processes, but without the first, so that we can sort by Burst Time.
    struct Process processesWithoutFirst[numberOfProcesses - 1];
    for (int i = 0; i < numberOfProcesses - 1; i++) {
        processesWithoutFirst[i] = processes[i + 1];
    }
    
    sortProcessesByBurstTime(processesWithoutFirst, numberOfProcesses - 1);
    
    struct Process sortedByBurstWithFirst[numberOfProcesses];
    sortedByBurstWithFirst[0] = processes[0];
    
    //Once we've sorted By Burst Time, we can add the first arrived process back onto a new array.
    
    for (int i = 1; i <= numberOfProcesses; i++) {
        sortedByBurstWithFirst[i] = processesWithoutFirst[i - 1];
    }
    
    //We've got the correct order we need to process in, Shortest Job First, now we can loop through and do work.
    for (int i = 1; i <= numberOfProcesses; i++) {
    	//Check that the Process has actually arrived - if not, move it back, and pick up the next available one.
        if (sortedByBurstWithFirst[i].arrivalTime > sortedByBurstWithFirst[i - 1].completionTime) {
            struct Process notArrivedYet = sortedByBurstWithFirst[i];
            sortedByBurstWithFirst[i] = sortedByBurstWithFirst[i + 1];
            sortedByBurstWithFirst[i + 1] = notArrivedYet;
            
        }

        //Do work on the process. 
        sortedByBurstWithFirst[i].completionTime = sortedByBurstWithFirst[i - 1].completionTime + sortedByBurstWithFirst[i].burstTime;
        sortedByBurstWithFirst[i].turnAroundTime = sortedByBurstWithFirst[i].completionTime - sortedByBurstWithFirst[i].arrivalTime;
        sortedByBurstWithFirst[i].waitTime = sortedByBurstWithFirst[i].turnAroundTime - sortedByBurstWithFirst[i].burstTime;
        averageTurnAroundTime += sortedByBurstWithFirst[i].turnAroundTime;
        averageWaitTime += sortedByBurstWithFirst[i].waitTime;
    }

    averageTurnAroundTime /= numberOfProcesses;
    averageWaitTime /= numberOfProcesses;
    
    display(sortedByBurstWithFirst, numberOfProcesses);
}

/*
This method allows the Round Robin algorithm to easily check if a process has already been added to the queue,
to avoid it being repeatedly added when it is still waiting its  turn to be processed.
*/
bool isAlreadyOnQueue(struct Process *readyQueue[100], struct Process *processToEvaluate, int readyQueuePosition, int readyQueueSize) {
	if (readyQueueSize == 0) {
		return false;
	}
	
	for (int i = readyQueuePosition; i <= readyQueueSize; i++) {
		if (readyQueue[i]->processId == processToEvaluate->processId) {
			return true;
		}
	}
	return false;
}

void roundRobin(struct Process processes[10], int numberOfProcesses) {
	//In Round Robin, a Time Quantum is needed, which is the equal time that each process will be allocated to do work.
	//Time Quantums must be a postive number.
	int timeQuantum = 0;
	
	printf("Enter a Time Quantum (TQ): ");
    scanf("%d", &timeQuantum);
    
    if (timeQuantum <= 0) {
		printf("Time quantum is not valid, must be a positive number!\n");
		return;
	}
    
	int completedProcesses = 0;
	int currentTime = 0;

	// A Ready Queue is established, and the first Process to arrive is added to it.
	int readyQueuePosition = 0;
	int readyQueueSize = 0;
	struct Process *readyQueue[100];
	readyQueue[0] = &processes[0];
	  
	//We'll loop through until all processes have been completed.
	while (completedProcesses != numberOfProcesses) {
		//If the Remaining Burst time is greater than the TQ, it won't have finished, so we'll take the TQ from the RBT and move on.
		if (timeQuantum < readyQueue[readyQueuePosition]->remainingBurstTime) {
			currentTime = currentTime + timeQuantum;
			readyQueue[readyQueuePosition]->remainingBurstTime = readyQueue[readyQueuePosition]->remainingBurstTime - timeQuantum;
		} else if (timeQuantum == readyQueue[readyQueuePosition]->remainingBurstTime) {
			//If the TQ is equal to the RBT, then the process is complete. 
			currentTime = currentTime + timeQuantum;
			readyQueue[readyQueuePosition]->remainingBurstTime = 0;
			readyQueue[readyQueuePosition]->completionTime = currentTime;
		} else if (timeQuantum > readyQueue[readyQueuePosition]->remainingBurstTime) {
			//If the TQ is greater than the RBT, the process can be completed in less time than has been allocated to it.
			currentTime = currentTime + readyQueue[readyQueuePosition]->remainingBurstTime;
			readyQueue[readyQueuePosition]->remainingBurstTime = 0;
			readyQueue[readyQueuePosition]->completionTime = currentTime;
		}

		//If our process is complete, we can fill in the information on it and increment the completedProcesses variable.
		if (readyQueue[readyQueuePosition]->remainingBurstTime == 0) {
			completedProcesses++;
			readyQueue[readyQueuePosition]->turnAroundTime = readyQueue[readyQueuePosition]->completionTime - readyQueue[readyQueuePosition]->arrivalTime;
			readyQueue[readyQueuePosition]->waitTime = readyQueue[readyQueuePosition]->turnAroundTime - readyQueue[readyQueuePosition]->burstTime;
			
			averageTurnAroundTime += readyQueue[readyQueuePosition]->turnAroundTime;
			averageWaitTime += readyQueue[readyQueuePosition]->waitTime;
		}
			
		//Put the next available processes on the queue
		for (int i = 0; i <= numberOfProcesses - 1; i++) {
			if (processes[i].arrivalTime <= currentTime 
				&& processes[i].remainingBurstTime != 0 
				&& &processes[i].processId != &readyQueue[readyQueuePosition]->processId
				&& !isAlreadyOnQueue(readyQueue, &processes[i], readyQueuePosition, readyQueueSize)) {
				readyQueue[++readyQueueSize] = &processes[i];
			}
		}

		//If the process we're dealing with is still not complete, it also needs adding back onto the queue.
		if (readyQueue[readyQueuePosition]->remainingBurstTime != 0) {
			readyQueue[++readyQueueSize] = readyQueue[readyQueuePosition];
		}

		readyQueuePosition++;
	}
	
	averageTurnAroundTime /= numberOfProcesses;
    averageWaitTime /= numberOfProcesses;
	display(processes, numberOfProcesses);
}

/*
This method acts similarly to a constructor in an object-oriented programming language.
It is used to cleanly and easily construct an instance of a Process 'struct'.
*/
struct Process constructProcess(int pid, int at, int bt) {
    struct Process process;
    process.processId = pid;
    process.arrivalTime = at;
    process.burstTime = bt;
    process.remainingBurstTime = bt;

    return process;
}

/*
The algorithm is performed based on what the user has entered, and their input is matched against
some pre-defined constants.
*/
void performAlgorithm(struct Process processes[10], int numberOfProcesses, int algorithm) {
    if (algorithm == FCFS) {
        firstComeFirstServed(processes, numberOfProcesses);
    } else if (algorithm == SJF) {
        shortestJobFirst(processes, numberOfProcesses);
    } else if (algorithm == RR) {
        roundRobin(processes, numberOfProcesses);
    } else {
        printf("Wrong algorithm!\n");
    }
}

/*
Manual input allows users to enter the data on-the-go, with ability to break from the process.
*/
void manualInput(int algorithm) {
    struct Process processes[10];
    int breakEntry = 1;
    int count = 0;
    while (breakEntry == 1) {
        int pid;
        int at;
        int bt;
        printf("Enter a Process ID (PID): ");
        scanf("%d", &pid);
        printf("Enter an Arrival Time (AT): ");
        scanf("%d", &at);
        printf("Enter a Burst Time (BT): ");
        scanf("%d", &bt);

        struct Process process = constructProcess(pid, at, bt);
        processes[count] = process;
        count++;

        printf("Do you want to add another process?\n"
                "1. Yes\n"
                "2. No\n'"
                "Enter a selection: ");
        scanf("%d", &breakEntry);

    }

    performAlgorithm(processes, count, algorithm);

}

bool isEmpty(char *text) {
    return strlen(text) <= 1;
}

/*
File input is via CSV, and has a maximum of ten processes for the purpose of demonstrating
this application.
Empty lines are skipped to avoid issues in parsing the data and not being able to construct
a Process.
*/
void inputFromFile(int algorithm) {
    struct Process processes[10];
    char fileName[64];
    printf("Enter the path to your CSV file (e.g. Assignment-Datasets-CSV/dataset1.csv): ");
    scanf("%s", fileName);
    FILE *file = fopen(fileName, "r");
    char line[64];
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (isEmpty(line)) {
            continue;
        }
        /* strtok() splits the line of the file by the delimiter,
         atoi() then converts this into an integer. */
        int pid = atoi(strtok(line, ","));
        int at = atoi(strtok(NULL, ","));
        int bt = atoi(strtok(NULL, ","));
        
        struct Process process = constructProcess(pid, at, bt);
        processes[count] = process;
        count++;
        
    }
    
    performAlgorithm(processes, count, algorithm);
}

/*
The main function is where the program begins;
it takes a user input for what algorithm the user wishes to use,
and how they wish to enter their data.
*/
int main(int argc, char **argv) {
    int algorithm = algorithmSelection();
    int processInputSelection = processSelection();

    if (processInputSelection == MANUAL_INPUT) {
        manualInput(algorithm);
    } else if (processInputSelection == INPUT_FROM_FILE) {
        inputFromFile(algorithm);
    } else {
        printf("Wrong input chosen!\n");
    }
        
    return 0;
}


