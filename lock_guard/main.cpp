#include <iostream>
#include <mutex>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

bool done = false;

std::mutex m;

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

void worker(string id)
{
    while( !done ) {
        {
            LockGuard::lock_guard l(m);
            cout << "starting work - id: " << id << endl;
            auto start = chrono::high_resolution_clock::now();

            // Seed with a real random value, if available
            std::random_device r;

            // Choose a random mean between 1 and 6
            std::default_random_engine e1(r());
            std::uniform_int_distribution<int> uniform_dist(500, 2000);
            int mean = uniform_dist(e1);
            this_thread::sleep_for(chrono::milliseconds(mean));
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> elapsed = end-start;
            cout << "done - id: " << id << ", worked: " << elapsed.count() << " ms\n";;
        }
    }
}

int main()
{
    // Start 4 threads
	std::thread t1(worker, "thread 1");
	std::thread t2(worker, "thread 2");
	std::thread t3(worker, "thread 3");
	std::thread t4(worker, "thread 4");

	t1.join();
	t2.join();
	t3.join();
	t4.join();

    return 0;
}
