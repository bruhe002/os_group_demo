#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <queue>
#include <array>
#include <vector>

using namespace std;

const int NUMBER_OF_THREADS = 15;
const int NUMBER_OF_RESOURCES = 20;
const int TIME_SLICE_VALUE = 2;
const string FILE_NAME = "test_threads.txt";

// Create a struct to simulate threads
struct sim_thread {
    int tid;        // Thread ID
    int priority;   // Priority number
    int burst_time; // Execution time
    int arrival_time; // Time of Arrival
    int needed_resource;    // Used to simulate resources being used 
    bool has_resource = false; // Identifies if thread has resource
};

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
vector<sim_thread*> BLOCKED_THREADS;

// We will use an array of boolean values to simulate resources being taken
bool resources[NUMBER_OF_RESOURCES] = {false}; 

void print_thread(const struct sim_thread t);
void signal(int freed_idx);
void add_to_queue(struct sim_thread *t);
bool empty_store_arr();
void arriving_thread(struct sim_thread *t, int current_thread);

int main() {
    // Simulating MFC preemptive scheduling for threads

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

    // Set up variables that will replicate the "System"
    int clock_time = 0;
    int currentPriority = 7;
    // For simplicity, we will use one time slice for all queues
    int time_slice = TIME_SLICE_VALUE;         // 2 milliseconds

    /*
        There will be a loop that will check the clock_time per iteration
        For each iteration, the system will add new threads to their respective queues
        if their arrival time matches clock_time (USE add_to_queue METHOD!)

        The first process in the current priority queue will run until one of three things occur,
            1. It finishes execution
            2. The time slice expires
            3. A higher priority queue is no longer empty

        If 1 occurs, delete the thread pointer, and start running a new thread
        If 2 occurs, add the current running thread to the back of its assigned queue, reset the time slice, and run a new thread
        If 3 occurs, immediately move the current running thread to the back of its queue, reset the time slice, change the currentPriority,
            and run a new thread

        Once a thread begins running, immediately check to see if it needs a resource (-1 means it doesn't need a resource)
        If a resource is needed, use the thread's needed_resource attribute as the index to "resources" array defined earlier.
        If the resource at the index is FALSE, set it to TRUE, and set the thread's has_resource attribute to TRUE.
        If the resource is already TRUE, and the Thread's has_resource is FALSE, the thread becomes blocked 
            => push it to the blocked queue
        If the resource is already TRUE, and the Thread's has_resource is TRUE, the thread was the one who acquired the resource, 
            continue running as normal

        There will be a function defined SIGNAL which will loop through the block queue and check if the blocked threads' needed_resource
            have been changed to FALSE
        If so, move the blocked thread back to its priority queue.

        The attributes of the threads are public and can be changed directly. Use burst_time as a counter for remaining time the thread has to execute.

        The condition to end the main loop I still need to work out. We can stop when all threads have completed, so when the store_arr is full of 
            nullptrs? 
    */
    cout << endl;
    system("Pause");
    return 0;
}

void print_thread(const struct sim_thread t) {
    cout << "Thread ID " << t.tid << endl;
    cout << "\tThread Priority " << t.priority << endl; 
    cout << "\tThread Burst Time " << t.burst_time << endl; 
    cout << "\tThread Arrival Time " << t.arrival_time << endl;
    cout << "\tThread Needed Resource " << t.needed_resource << endl;  
}


/*
    Checks the blocked queue to return a thread to its priority queue based on 
    a resource that was just free

    Block queue is a vector, so all the functionalities of a queue and array
*/
void signal(int freed_idx) {
    for(int i = 0; i < BLOCKED_THREADS.size(); i++) {
        if(resources[freed_idx] == BLOCKED_THREADS[i]->needed_resource) {
            add_to_queue(BLOCKED_THREADS[i]);
            BLOCKED_THREADS.erase(BLOCKED_THREADS.begin() + i);
        }
    }
}


/*
    Called whenever a thread will be added to a queue when clock time matches the arrival time
    Use the argument thread's priority attribute as a selector in a switch statement
    Each case will match the queues level of priority
        7 -> THREAD_PRIORITY_TIME_CRITICAL
        1 -> THREAD_PRIORITY_IDLE
    Based on the priority, push it to the appropriate queue
*/
void add_to_queue(struct sim_thread *t) {
    switch(t->priority) {
        case 1:
            THREAD_PRIORITY_IDLE.push(t);
            break;
        case 2:
            THREAD_PRIORITY_LOWEST.push(t);
            break;
        case 3:
            THREAD_PRIORITY_BELOW_NORMAL.push(t);
            break;
        case 4:
            THREAD_PRIORITY_NORMAL.push(t);
            break;
        case 5:
            THREAD_PRIORITY_ABOVE_NORMAL.push(t);
            break;
        case 6:
            THREAD_PRIORITY_ABOVE_NORMAL.push(t);
            break;
        case 7:  
            THREAD_PRIORITY_HIGHEST.push(t);
            break;

    }
}

/*
    Checks if all the pointers in the array are nullptr
    If so, return true
*/
bool empty_store_arr() {
    bool result = true;
    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        if(store_arr[i] != nullptr) {
            result = false;
        }
    }

    return result;
}


/*
    Checks if a thread's arriving time matches the current time
    if so return, add to queue
*/
void arriving_thread(struct sim_thread *t, int current_time) {
    if(t->arrival_time == current_time) {
        add_to_queue(t);
    }
}