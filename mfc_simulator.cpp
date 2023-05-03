#include <windows.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <queue>
#include <array>
#include <vector>

using namespace std;

// Create an enum to track priority values
enum priority { ANY = -1, IDLE=1, LOWEST, BELOW_NORMAL, NORMAL, ABOVE_NORMAL, HIGHEST, TIME_CRITICAL };

const int NUMBER_OF_THREADS = 15;
const int NUMBER_OF_RESOURCES = 20;
const int TIME_SLICE_VALUE = 2;
const int NUMBER_OF_PRIORITY_QUEUES = 7;
const int NUM_CPUS = 2;
const string FILE_NAME = "test_threads.txt";

// Create a struct to simulate threads
struct sim_thread {
    int tid;        // Thread ID
    priority prior;   // Priority number
    int burst_time; // Execution time
    int arrival_time; // Time of Arrival
    int needed_resource;    // Used to simulate resources being used 
    bool has_resource = false; // Identifies if thread has resource
};


// Create the Priority Queues
queue<sim_thread*> THREAD_PRIORITY_IDLE_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_LOWEST_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_BELOW_NORMAL_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_NORMAL_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_ABOVE_NORMAL_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_HIGHEST_QUEUE;
queue<sim_thread*> THREAD_PRIORITY_TIME_CRITICAL_QUEUE;

// Create a storage array for the threads
sim_thread* store_arr[NUMBER_OF_THREADS];

// Have a blocked queue
vector<sim_thread*> BLOCKED_THREADS;

// We will use an array of boolean values to simulate resources being taken
bool resources[NUMBER_OF_RESOURCES] = { false };

// FUNCTIONS
void print_thread(const sim_thread t);
void signal(int freed_idx);
bool block(sim_thread* t);
void add_to_queue(sim_thread* t);
bool empty_store_arr();
void arriving_thread(sim_thread* t, int current_thread);
priority queues_empty(priority);
sim_thread* grab_next();
void print_execution_message(sim_thread* cpu[NUM_CPUS], int current_clock_time);

int main() {
    // Simulating MFC preemptive scheduling for threads

    // Create threads and add them to the array
    ifstream my_file(FILE_NAME);        // use ifstream to read from file
    if (my_file.is_open()) {             // If file is open
        char row[100];                  // Reads each row

        for (int i = 0; i < NUMBER_OF_THREADS && my_file; i++) {
            my_file.getline(row, 100);  // Read each row from file
            // cout << row << endl;
            char* ptr = strtok(row, " ");   // Separate the file contents by space

            store_arr[i] = new sim_thread;  // Make a new thread 

            store_arr[i]->tid = stoi(ptr);  // Set Thread ID
            ptr = strtok(NULL, " ");        // Move pointer to the next number in row

            store_arr[i]->prior = (priority)stoi(ptr);     // Set Thread Priority
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
    priority curr_priorities[NUM_CPUS];
    unsigned timeSection[NUM_CPUS];
    for (unsigned i = 0; i < NUM_CPUS; i++)
        timeSection[i] = 0;
    for (unsigned i = 0; i < NUM_CPUS; i++)
        curr_priorities[i] = ANY;
    // For simplicity, we will use one time slice for all queues
    int time_slice = TIME_SLICE_VALUE;         // 2 milliseconds
    bool blocked = false;
    unsigned completed_threads = 0;

    sim_thread* current_thread[NUM_CPUS];
    for (unsigned i = 0; i < NUM_CPUS; i++)
        current_thread[i] = nullptr;

    /*
        There will be a loop that will check the clock_time per iteration
        For each iteration, the system will add new threads to their respective queues
        if their arrival time matches clock_time (USE arriving_thread METHOD!)

        The first process in the current priority queue will run until one of three things occur,
            1. It finishes execution
            2. The time slice expires
            3. A higher priority queue is no longer empty

        If 1 occurs, delete the thread pointer, and start running a new thread
        If 2 occurs, add the current running thread to the back of its assigned queue, reset the time slice, and run a new thread
        If 3 occurs, immediately move the current running thread to the back of its queue, reset the time slice, change the currentPriority,
            and run a new thread

        Before we decrement the current thread's time, let's check if any of the higher priorities queues are not empty
            Loop from the top and check if the queues are empty up until the current queue.
            If a queue is not empty, immediatly change the current Thread

            Regardless of a change or not, continue running the process and decrement the burst time of the current running thread

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

    // Loop for everything above
    while (completed_threads < NUMBER_OF_THREADS)
    {
        // Start by checking if any threads have arrived
        for (unsigned i = 0; i < NUMBER_OF_THREADS; i++)
        {
            if (store_arr[i] != nullptr)
                arriving_thread(store_arr[i], clock_time);
        }
            
        // Loop through for each CPU
        for (unsigned i = 0; i < NUM_CPUS; i++)
        {
            // Check if the thread is finished
            if (current_thread[i] != nullptr && current_thread[i]->burst_time == 0)
            {
                cout << "\t-------------------------------------------" << endl;
                cout << "\tCPU " << i << " HAS FINISHED THREAD " << current_thread[i]->tid << endl;
                completed_threads++;
                signal(current_thread[i]->needed_resource);
                current_thread[i] = nullptr;
                curr_priorities[i] = ANY;
                 
            }
            sleep(2);       // Sleep after the print
            // If the core is IDLE, a higher priority thread has entered, or the time slice is over
            if (current_thread[i] == nullptr || curr_priorities[i] != queues_empty(curr_priorities[i]) || timeSection[i] == TIME_SLICE_VALUE)
            {
                if (current_thread[i] != nullptr)
                    add_to_queue(current_thread[i]);
                sim_thread* toRun = grab_next();
                if (toRun == nullptr)
                {
                    curr_priorities[i] = ANY;
                    current_thread[i] = nullptr;
                }
                else
                {
                    curr_priorities[i] = toRun->prior;
                    current_thread[i] = toRun;
                }
                timeSection[i] = 0;
            }
            if (current_thread[i] != nullptr)
            {
                current_thread[i]->burst_time--;
                timeSection[i]++;
            }

        }
        print_execution_message(current_thread, clock_time);
        // End with incrementing clock time
        clock_time++;
        
    }

    cout << endl;
    system("Pause");
    return 0;
}

void print_thread(const struct sim_thread t) {
    cout << "Thread ID " << t.tid << endl;
    cout << "\tThread Priority " << t.prior << endl;
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
    for (int i = 0; i < BLOCKED_THREADS.size(); i++) {
        if (freed_idx == BLOCKED_THREADS[i]->needed_resource) {
            add_to_queue(BLOCKED_THREADS[i]);
            BLOCKED_THREADS.erase(BLOCKED_THREADS.begin() + i);
        }
    }
    resources[freed_idx] = false;
}

/*
    Checks if the current running thread's resource is already taken
    If yes, and the thread isn't the holder of the resource isn't TRUE,
    block the thread, return TRUE

    else acquire the resource (change the resource idx and has_resource to TRUE)
    then return FALSE
*/
bool block(sim_thread* t) {
    if (resources[t->needed_resource]) {
        if (!(t->has_resource)) {
            cout << "\t-------------------------------------------" << endl;
            cout << "\tThread " << t->tid << " has been blocked" << endl;
            BLOCKED_THREADS.push_back(t);
            return true;
        }
        return false;
    }
    else {
        resources[t->needed_resource] = true;
        t->has_resource = true;
        return false;
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
void add_to_queue(struct sim_thread* t) {
    switch (t->prior) {
    case IDLE:
        THREAD_PRIORITY_IDLE_QUEUE.push(t);
        break;
    case LOWEST:
        THREAD_PRIORITY_LOWEST_QUEUE.push(t);
        break;
    case BELOW_NORMAL:
        THREAD_PRIORITY_BELOW_NORMAL_QUEUE.push(t);
        break;
    case NORMAL:
        THREAD_PRIORITY_NORMAL_QUEUE.push(t);
        break;
    case ABOVE_NORMAL:
        THREAD_PRIORITY_ABOVE_NORMAL_QUEUE.push(t);
        break;
    case HIGHEST:
        THREAD_PRIORITY_HIGHEST_QUEUE.push(t);
        break;
    case TIME_CRITICAL:
        THREAD_PRIORITY_TIME_CRITICAL_QUEUE.push(t);
        break;

    }
}

/*
    Checks if all the pointers in the array are nullptr
    If so, return true
*/
bool empty_store_arr() {
    bool result = true;
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        if (store_arr[i] != nullptr) {
            result = false;
        }
    }
    return result;
}

/*
    Checks if a thread's arriving time matches the current time
    if so return, add to queue
*/
void arriving_thread(struct sim_thread* t, int current_time) {
    if (t->arrival_time == current_time) {
        add_to_queue(t);
    }
}

/*
    Checks if all queues are empty
*/
priority queues_empty(priority abovePriority = ANY) {
    priority max = abovePriority;
    switch (abovePriority)
    {
    case ANY:
        if (!THREAD_PRIORITY_IDLE_QUEUE.empty())
            max = IDLE;
    case IDLE:
        if (!THREAD_PRIORITY_LOWEST_QUEUE.empty())
            max = LOWEST;
    case LOWEST:
        if (!THREAD_PRIORITY_BELOW_NORMAL_QUEUE.empty())
            max = BELOW_NORMAL;
    case BELOW_NORMAL:
        if (!THREAD_PRIORITY_NORMAL_QUEUE.empty())
            max = NORMAL;
    case NORMAL:
        if (!THREAD_PRIORITY_ABOVE_NORMAL_QUEUE.empty())
            max = ABOVE_NORMAL;
    case ABOVE_NORMAL:
        if (!THREAD_PRIORITY_HIGHEST_QUEUE.empty())
            max = HIGHEST;
    case HIGHEST:
        if (!THREAD_PRIORITY_TIME_CRITICAL_QUEUE.empty())
            max = TIME_CRITICAL;
    }
    return max;
}

// A function to return the highest priority from the relative queue
sim_thread* grab_next()
{
    sim_thread* val = nullptr;
    while (!THREAD_PRIORITY_TIME_CRITICAL_QUEUE.empty())
    {
        val = THREAD_PRIORITY_TIME_CRITICAL_QUEUE.front();
        THREAD_PRIORITY_TIME_CRITICAL_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_HIGHEST_QUEUE.empty())
    {
        val = THREAD_PRIORITY_HIGHEST_QUEUE.front();
        THREAD_PRIORITY_HIGHEST_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_ABOVE_NORMAL_QUEUE.empty())
    {
        val = THREAD_PRIORITY_ABOVE_NORMAL_QUEUE.front();
        THREAD_PRIORITY_ABOVE_NORMAL_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_NORMAL_QUEUE.empty())
    {
        val = THREAD_PRIORITY_NORMAL_QUEUE.front();
        THREAD_PRIORITY_NORMAL_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_BELOW_NORMAL_QUEUE.empty())
    {
        val = THREAD_PRIORITY_BELOW_NORMAL_QUEUE.front();
        THREAD_PRIORITY_BELOW_NORMAL_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_LOWEST_QUEUE.empty())
    {
        val = THREAD_PRIORITY_LOWEST_QUEUE.front();
        THREAD_PRIORITY_LOWEST_QUEUE.pop();
        if (!block(val))
            return val;
    }
    while (!THREAD_PRIORITY_IDLE_QUEUE.empty())
    {
        val = THREAD_PRIORITY_IDLE_QUEUE.front();
        THREAD_PRIORITY_IDLE_QUEUE.pop();
        if (!block(val))
            return val;
    }
    return val;
}

void print_execution_message(sim_thread* cpu[NUM_CPUS], int current_clock_time) {
    cout << "\nAt time " << current_clock_time << ":" << endl;
    string msg = "";
    for (int i = 0; i < NUM_CPUS; i++) {

        if (cpu[i] != nullptr) {
            msg = "Currently running THREAD " + to_string(cpu[i]->tid) + " with priority " + to_string(cpu[i]->prior);
        }
        else {
            msg = "Currently Idle";
        }
        cout << "\tCPU " << i + 1 << ": " << msg << endl;
    }
}