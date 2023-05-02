#include <iostream>
#include <iomanip>
#include <queue>

using namespace std;

int main() {


    // Simulating MFC preemptive scheduling for threads

    // Create a struct to simulate threads
    struct sim_thread {
        int tid;        // Thread ID
        int priority;   // Priority number
        int burst_time; // Execution time
        int arrival_time; // Time of Arrival 
    };

    // Create the Priority Queues
    queue<sim_thread> THREAD_PRIORITY_IDLE;
    queue<sim_thread> THREAD_PRIORITY_LOWEST;
    queue<sim_thread> THREAD_PRIORITY_BELOW_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_ABOVE_NORMAL;
    queue<sim_thread> THREAD_PRIORITY_HIGHEST;
    queue<sim_thread> THREAD_PRIORITY_TIME_CRITICAL;


    cout << endl;
    system("Pause");
    return 0;
}