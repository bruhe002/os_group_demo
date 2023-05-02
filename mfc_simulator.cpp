#include <iostream>
#include <iomanip>
#include <queue>
#include <array>

using namespace std;

int main() {
    // Simulating MFC preemptive scheduling for threads

    // Create a struct to simulate threads
    struct sim_thread {
        int tid;        // Thread ID
        int priority;   // Priority number
        int burst_time; // Execution time
        int arrival_time; // Time of Arrival
        int needed_resource;    // Used to simulate resources being used 
    };

    // Create the Priority Queues
    queue<sim_thread> THREAD_PRIORITY_IDLE;
    queue<sim_thread> THREAD_PRIORITY_LOWEST;
    queue<sim_thread> THREAD_PRIORITY_BELOW_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_ABOVE_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_HIGHEST;
    queue<sim_thread> THREAD_PRIORITY_TIME_CRITICAL;

    // Have a blocked queue
    queue<sim_thread> BLOCKED_THREADS;

    // For simplicity, we will use one time slice for all queues
    int time_slice = 2;         // 2 milliseconds

    // We will use an array of boolean values to simulate resources being taken
    bool resources[100];
    for(int i = 0; i < 100; i++) {
        resources[i] = false;
    }

    cout << endl;
    system("Pause");
    return 0;
}