#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3
#define MAX_PROCESS 100


typedef struct {
    int iPID;               
    int iArrival, iBurst;    
    int iOriginalBurst;      
    int iStart, iFinish;  
    int iWaiting, iResponse, iTaT; 
} PCB;


typedef struct {
    int iPID;   
    int start;  
    int end;    
} Gantt;


Gantt GanttChart[MAX_PROCESS];
int iGanttIndex = 0;


void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("Enter details for Process P%d:\n", i + 1);
        printf("Arrival Time: ");
        scanf("%d", &P[i].iArrival);
        printf("Burst Time: ");
        scanf("%d", &P[i].iBurst);

        P[i].iPID = i + 1;
        P[i].iOriginalBurst = P[i].iBurst; 
        P[i].iStart = -1;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = -1;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\nPID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               P[i].iPID, P[i].iArrival, P[i].iOriginalBurst, 
               P[i].iStart, P[i].iFinish, 
               P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}


void exportGanttChart(int n, PCB P[]) {
    printf("\nGantt Chart:\n");

    // Dòng khung tiến trình với dấu "="
    for (int i = 0; i < iGanttIndex; i++) {
        printf("|");
        int burstWidth = GanttChart[i].end - GanttChart[i].start; // Độ dài khung = thời gian thực thi
        for (int j = 0; j < burstWidth; j++) {
            printf("=");
        }
    }
    printf("|\n");

    // Hiển thị tên tiến trình, căn giữa đoạn khung
    for (int i = 0; i < iGanttIndex; i++) {
        int burstWidth = GanttChart[i].end - GanttChart[i].start;
        int padding = burstWidth - 2; // Chừa khoảng trống
        printf("| P%-*d", padding, GanttChart[i].iPID);
    }
    printf("|\n");

    // Hiển thị thời gian bên dưới
    printf("%-2d", GanttChart[0].start); // Thời gian bắt đầu
    for (int i = 0; i < iGanttIndex; i++) {
        int burstWidth = GanttChart[i].end - GanttChart[i].start;
        printf("%*d  ", burstWidth, GanttChart[i].end); // Thời gian kết thúc căn thẳng
    }
    printf("\n");
}

void calculateAWT(int n, PCB P[]) {
    float totalWaitingTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", totalWaitingTime / n);
}


void calculateATaT(int n, PCB P[]) {
    float totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalTurnaroundTime += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
}


int getNextProcess(int currentTime, int n, PCB P[]) {
    int shortestIndex = -1;
    int minBurst = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (P[i].iArrival <= currentTime && P[i].iBurst > 0) { // Tiến trình sẵn sàng
            if (P[i].iBurst < minBurst || 
                (P[i].iBurst == minBurst && P[i].iArrival < P[shortestIndex].iArrival)) {
                shortestIndex = i;
                minBurst = P[i].iBurst;
            }
        }
    }
    return shortestIndex;
}

// Thuật toán SJF (Preemptive)
void sjfPreemptive(int n, PCB P[]) {
    int completed = 0;
    int currentTime = 0;
    int previousPID = -1;

    while (completed < n) {
        int index = getNextProcess(currentTime, n, P);

        if (index == -1) { // Không có tiến trình sẵn sàng
            currentTime++;
            continue;
        }

        if (previousPID != P[index].iPID) {
            // Gantt Chart
            if (previousPID != -1 && iGanttIndex > 0) {
                GanttChart[iGanttIndex - 1].end = currentTime;
            }
            GanttChart[iGanttIndex].iPID = P[index].iPID;
            GanttChart[iGanttIndex].start = currentTime;
            iGanttIndex++;
        }

        if (P[index].iResponse == -1) { // Chỉ tính thời gian phản hồi lần đầu
            P[index].iResponse = currentTime - P[index].iArrival;
        }

        if (P[index].iStart == -1) { // Ghi nhận thời gian bắt đầu
            P[index].iStart = currentTime;
        }

        P[index].iBurst--; // Thực thi tiến trình
        currentTime++;

        if (P[index].iBurst == 0) { // Tiến trình hoàn thành
            P[index].iFinish = currentTime;
            P[index].iTaT = P[index].iFinish - P[index].iArrival;
            P[index].iWaiting = P[index].iTaT - P[index].iOriginalBurst;
            completed++;
        }

        previousPID = P[index].iPID;
    }

    // Cập nhật Gantt Chart lần cuối
    if (iGanttIndex > 0) {
        GanttChart[iGanttIndex - 1].end = currentTime;
    }
}

// Hàm chính
int main() {
    PCB Input[MAX_PROCESS];
    int iNumberOfProcess;

    printf("Please input number of processes: ");
    scanf("%d", &iNumberOfProcess);

    inputProcess(iNumberOfProcess, Input);
    sjfPreemptive(iNumberOfProcess, Input);
    printProcess(iNumberOfProcess, Input);
    printf("\n===== SJF Preemptive Scheduling =====\n");
   
    exportGanttChart(iNumberOfProcess, Input);
    calculateAWT(iNumberOfProcess, Input);
    calculateATaT(iNumberOfProcess, Input);

    return 0;
}
