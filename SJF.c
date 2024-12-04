#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    printf("Nhập thông tin cho từng tiến trình:\n");
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Process %d:\n", i + 1);
        printf("  iArrival: ");
        scanf("%d", &P[i].iArrival);
        printf("  iBurst: ");
        scanf("%d", &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("PID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart,
               P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}

void exportGanttChart(int n, PCB P[]) {
    printf("Gantt Chart:\n");

    // Hàng tiến trình
    for (int i = 0; i < n; i++) {
        printf("| P%d ", P[i].iPID);
    }
    printf("|\n");

    // Hàng thời gian
    for (int i = 0; i < n; i++) {
        printf("%d   ", P[i].iStart);
    }
    printf("%d\n", P[n - 1].iFinish);
}


void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < (*n) - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        int condition = 0;
        if (iCriteria == SORT_BY_ARRIVAL) {
            condition = P[j].iArrival < pivot.iArrival;
        } else if (iCriteria == SORT_BY_PID) {
            condition = P[j].iPID < pivot.iPID;
        } else if (iCriteria == SORT_BY_BURST) {
            condition = P[j].iBurst < pivot.iBurst;
        }
        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    float totalAWT = 0;
    for (int i = 0; i < n; i++) {
        totalAWT += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", totalAWT / n);
}

void calculateATaT(int n, PCB P[]) {
    float totalATaT = 0;
    for (int i = 0; i < n; i++) {
        totalATaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalATaT / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    int currentTime = 0;
    while (iTerminated < iNumberOfProcess) {
        // Đưa tiến trình có thể vào ReadyQueue
        while (iRemain > 0 && Input[0].iArrival <= currentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        // Sắp xếp ReadyQueue theo thời gian chạy (SJF)
        quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

        if (iReady > 0) {
            // Xử lý tiến trình đầu tiên trong ReadyQueue
            PCB currentProcess = ReadyQueue[0];
            removeProcess(&iReady, 0, ReadyQueue);

            currentProcess.iStart = (currentTime > currentProcess.iArrival) ? currentTime : currentProcess.iArrival;
            currentProcess.iFinish = currentProcess.iStart + currentProcess.iBurst;
            currentProcess.iResponse = currentProcess.iStart - currentProcess.iArrival;
            currentProcess.iWaiting = currentProcess.iResponse;
            currentProcess.iTaT = currentProcess.iFinish - currentProcess.iArrival;

            currentTime = currentProcess.iFinish;
            pushProcess(&iTerminated, TerminatedArray, currentProcess);
        } else {
            currentTime++; // Nếu không có tiến trình nào sẵn sàng, tăng thời gian
        }
    }
    
    printf("\n===== Process Information =====\n");
    printProcess(iTerminated, TerminatedArray);

    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}
