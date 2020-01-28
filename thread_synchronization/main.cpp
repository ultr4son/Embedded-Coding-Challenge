// @author : ian cain
// @date   : 2018
// @b      : Coding challenge for interview candidates to show ability to use and synchronize threads
//

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>


constexpr auto THREAD_1 = "thread 1";
constexpr auto THREAD_2 = "thread 2";
constexpr auto THREAD_3 = "thread 3";
constexpr auto THREAD_NONE = "";

using namespace std;

namespace LockGuard {
	/*
	  Implementation of std::lock_guard
	  A lock guard wraps a mutex such that it is locked when the lock guard is constructed and unlocked when the lock guard is deconstructed.
	  Lock guards are non-copyable.
	*/
	class lock_guard {
	public:
		/*
		  Construct a lock guard and lock the given mutex.
		*/
		lock_guard(std::mutex& m) {
			mux = &m;
			mux->lock();
		}

		/*
		  Deconstruct the lock guard and unlock the mutex given in the constructor.
		*/
		~lock_guard() {
			mux->unlock();
		}

		//Enforce non-copyability
		lock_guard(const lock_guard&) = delete;
		lock_guard& operator=(const lock_guard&) = delete;

	private:
		std::mutex* mux;
	};
};

std::mutex m;

bool done = false;

condition_variable cv;

//Signal that declares what thread should be currently executing. Should be the id of a thread or THREAD_NONE. 
string condition_signal = THREAD_NONE;

/*
	Creates a predicate that compares the given signal to condition_signal
*/
auto when_signal(string signal) {
	return [=]() { return condition_signal == signal; };
}

/*
	Function that defines the sequential behavior of each thread. thread_id must be one of THREAD_1, THREAD_2, THREAD_3.
*/
const string next_id(const string thread_id) {
	if (thread_id == THREAD_1) {
		return THREAD_2;
	}
	if (thread_id == THREAD_2) {
		return THREAD_3;
	}
	if (thread_id == THREAD_3) {
		return THREAD_1;
	}
	
	throw "Invalid thread id: " + thread_id;
}

void worker(string id)
{
	std::unique_lock<std::mutex> lck(m);
	cout << id << " starting, waiting" << endl;

	while( !done ) {

		//Wait for condition_signal to be this thread's id.
		cv.wait(lck, when_signal(id));

		//When the signal is recived, do the "work"
        cout << "starting work - id: " << id << endl;	

		auto start = chrono::high_resolution_clock::now();
		
		std::random_device r;

		//Wait for 1 to 5 seconds
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(1000, 5000);
		int wait_time = uniform_dist(e1);
		this_thread::sleep_for(chrono::milliseconds(wait_time));

		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, milli> elapsed = end - start;
        
		cout << "done - id: " << id << ", worked: " << elapsed.count() << " ms\n";

    	condition_signal = next_id(id);

		// notify next waiting thread using condition_variable
		cv.notify_all();
	
	}
}

int main()
{
	cout << "main: starting all threads" << endl;

    // Start 3 threads
	std::thread t1(worker, THREAD_1);
	std::thread t2(worker, THREAD_2);
	std::thread t3(worker, THREAD_3);

	

    // Kick off thread chain using condition variable
	{
		LockGuard::lock_guard lck(m);
		
		//Start thread 1
		condition_signal = THREAD_1;
	}

	cv.notify_all();
	
	t1.join();
	t2.join();
	t3.join();
    return 0;
}
