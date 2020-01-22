Candidates,

We all hate white board challenges. This coding challenge replaces the white board coding tests. You will be able to take this challenge in your most productive and thoughtful environment. It is an opportunity to prove how talented you are. Best part … NO WHITE BOARD coding tests !!!

You have 2 weeks to complete and submit the challenge. It should take you a few hours depending on your current skill level. In comparison normal software interviews take all day long with 4-5, 45 minute or 1 hour white board coding tests. Every member of our team has completed this challenge and universally agrees this is a far better representation of engineering skill than white board tests.

Here is your coding challenge for embedded/c++.  Good luck !!! I sincerely wish you well. I am looking forward to seeing your skills.

Background:
* Code clarity, consistency, organization and correctness are all evaluated.
* All code to be submitted to a github account
* This challenge is not meant to "trick" or "confuse" you in any way.
          It is exactly how I would pose work to one of my engineers.
          Do not make it more complex then it really is.
* All members of the team have taken the challenges
* If you pass I will show you my solution
* Use all resources at your disposal. You are allowed to ask appropriate questions.

Progressive Challenges:
  Warm up challenge for basic c++11 (or greater) knowledge
  Basic Memory Management to demonstrate deep understanding of memory/pointers/data structures
  Platform Dispatcher to show a real world example of combined skills
     Note: My team has already implemented and uses our Platform Command Dispatcher
           for a real world device.

What I am looking for:
   * Coding ability (aka: what you learned in school)
   * Desire to demonstrate your skills (motivation and drive)
   * Ability to take loosely defined requirements and fill in the gaps required
       to accomplish the task. This is not school, this is professional software
       engineering. Tasks are often vague and exploratory in nature. In school
       the answer is already known by the professor and you are solving their "puzzle".
       Professional Engineers rarely get to solve puzzles already completed.
       Most tasks are unknown. It is the fundamental difference between "Engineering"
       and "Manufacturing".
   * Clarity and organization. "Clean your room!"
      Engineering software projects are large and complex and you must
      be able to communicate highly technical ideas and code in a clear and
      concise manner so the rest of the team can help you and add to your efforts.
      Messy "Genius Code" is only workable by the "genius" and this person is
      now competing 1 against many.

      African proverb: If you want to go fast, go alone. If you want to go far, go together.

//
// basic c/c++
//
C++11 threads/synchronization/protection:

1) Reimplement c++11 lock_guard (NOT use std::lock_guard). See lock_guard directory.

2) Use c++11 std::condition_var to synchronize threads
      -- Psuedo Code --
      main:
        1) start: thread 1, thread 2, thread 3
        2) all threads block on condition_signal
        3) main signals condition_signal
        4) join all threads to prevent exit main // gimmee

        thread 1:
           break from condition_signal due to step 3
           sleep (random(1,5))
           signal thread 2
           wait on condition_signal

      example output:
         main: starting all threads
         thread1: starting, waiting.
         thread2: starting, waiting.
         thread3: starting, waiting.
         main: starting thread 1.
         thread1: signal received, doing work ....
         thread1: done with work, signal next thread
         thread2: signal received, doing work ....
         thread2: done with work, signal next thread

         on and on and on until you CTRL-C the program

  3) Use your implementation of c++11 lock_guard (NOT std::lock_guard)
     to protect a common resource in your threading example #2

  4) Platform Dispatcher
     See attached *.cpp file

  5) EXTRA CREDIT:
     Memory Pool
     See attached architecture and files
