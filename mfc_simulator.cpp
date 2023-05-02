#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <queue>
#include <array>

using namespace std;

const int NUMBER_OF_THREADS = 15;
const int NUMBER_OF_RESOURCES = 20;
const string FILE_NAME = "test_threads.txt";

// Create a struct to simulate threads
struct sim_thread {
    int tid;        // Thread ID
    int priority;   // Priority number
    int burst_time; // Execution time
    int arrival_time; // Time of Arrival
    int needed_resource;    // Used to simulate resources being used 
};

void print_thread(struct sim_thread t);

int main() {
    // Simulating MFC preemptive scheduling for threads

    // Create the Priority Queues
    queue<sim_thread*> THREAD_PRIORITY_IDLE;
    queue<sim_thread*> THREAD_PRIORITY_LOWEST;
    queue<sim_thread*> THREAD_PRIORITY_BELOW_NORMAL;
    queue<sim_thread*> THREAD_PRIORITY_NORMAL;
    queue<sim_thread*> THREAD_PRIORITY_ABOVE_NORMAL;
    queue<sim_thread*> THREAD_PRIORITY_HIGHEST;
    queue<sim_thread*> THREAD_PRIORITY_TIME_CRITICAL;

    // Create a storage array for the threads
    sim_thread* store_arr[NUMBER_OF_THREADS];

    // Have a blocked queue
    queue<sim_thread*> BLOCKED_THREADS;

    // For simplicity, we will use one time slice for all queues
    int time_slice = 2;         // 2 milliseconds

    // We will use an array of boolean values to simulate resources being taken
    bool resources[NUMBER_OF_RESOURCES];
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        resources[i] = false;
    }

    // Create threads and add them to the array
    ifstream my_file(FILE_NAME);        // use ifstream to read from file
    if(my_file.is_open()) {             // If file is open
        char row[100];                  // Reads each row

        for(int i = 0; i < NUMBER_OF_THREADS && my_file; i++) {
            my_file.getline(row, 100);  // Read each row from file
            // cout << row << endl;
            char *ptr = strtok(row, " ");   // Separate the file contents by space

            store_arr[i] = new sim_thread;  // Make a new thread 

            store_arr[i]->tid = stoi(ptr);  // Set Thread ID
            ptr = strtok(NULL, " ");        // Move pointer to the next number in row

            store_arr[i]->priority = stoi(ptr);     // Set Thread Priority
            ptr = strtok(NULL, " ");

            store_arr[i]->burst_time = stoi(ptr);   // Set Thread Burst Time
            ptr = strtok(NULL, " ");

            store_arr[i]->arrival_time = stoi(ptr); // Set Thread Arrival Time
            ptr = strtok(NULL, " ");

            store_arr[i]->needed_resource = stoi(ptr);  // Set Thread Needed Resource 
            ptr = strtok(NULL, " ");

            print_thread(*store_arr[i]);                // Print for Testing
        }
        
    }

    cout << endl;
    system("Pause");
    return 0;
}

void print_thread(struct sim_thread t) {
    cout << "Thread ID " << t.tid << endl;
    cout << "\tThread Priority " << t.priority << endl; 
    cout << "\tThread Burst Time " << t.burst_time << endl; 
    cout << "\tThread Arrival Time " << t.arrival_time << endl;
    cout << "\tThread Needed Resource " << t.needed_resource << endl;  
}