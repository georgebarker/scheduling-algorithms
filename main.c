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

int const FCFS = 1;
int const SJF = 2;
int const RR = 3;

int const MANUAL_INPUT = 1;
int const INPUT_FROM_FILE = 2;

typedef enum { false, true } bool;

float averageWaitTime, averageTurnAroundTime;

struct Process {
	int processId;
	int arrivalTime;
	int burstTime;
	int turnAroundTime;
	int waitTime;
};

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

void display(struct Process processes[10], int numberOfProcesses) {
	printf("PID\tAT\tBT\tTaT\tWT\n");
	for (int i = 0; i < numberOfProcesses; i++) {
		printf("%d\t%3d\t%3d\t%3d\t%3d\n", processes[i].processId,
				processes[i].arrivalTime, processes[i].burstTime,
				processes[i].turnAroundTime, processes[i].waitTime);
	}

	printf("Average Turn Around Time: %f\nAverage WaitTime:%f\n",
			averageTurnAroundTime, averageWaitTime);
}

void firstComeFirstServed(struct Process processes[10], int numberOfProcesses) {
	sortProcessesByArrivalTime(processes, numberOfProcesses);

	processes[0].waitTime = 0;
	averageTurnAroundTime = processes[0].turnAroundTime =
			processes[0].burstTime;
	int totalBurstTime = processes[0].burstTime;

	for (int i = 1; i < numberOfProcesses; i++) {
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
	printf("Hit sjf!");
}

void roundRobin(struct Process processes[10], int numberOfProcesses) {
	printf("Hit RR!");
}

struct Process constructProcess(int pid, int at, int bt) {
	struct Process process;
	process.processId = pid;
	process.arrivalTime = at;
	process.burstTime = bt;

	return process;
}

void performAlgorithm(struct Process processes[10], int numberOfProcesses, int algorithm) {
	if (algorithm == FCFS) {
        firstComeFirstServed(processes, numberOfProcesses);
    } else if (algorithm == SJF) {
        shortestJobFirst(processes, numberOfProcesses);
    } else if (algorithm == RR) {
        roundRobin(processes, numberOfProcesses);
    } else {
		printf("Wrong algorithm!");
    }
}

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

int main(int argc, char **argv) {
	int algorithm = algorithmSelection();
	int processInputSelection = processSelection();

    if (processInputSelection == MANUAL_INPUT) {
        manualInput(algorithm);
    } else if (processInputSelection == INPUT_FROM_FILE) {
        inputFromFile(algorithm);
    } else {
        printf("Wrong input chosen!");
    }
        
	return 0;
}


