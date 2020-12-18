# Windows System Programming

**_Window system programming from the perspective of synchronous and asynchronous Input/Output._**
Windows system programming is a programming field closely related to the Windows operating system.

In this field, you can develop various software by understanding and utilizing the internal structure and functions of the Windows operating system.

Through this, you can implement higher performance and functionality at the operating system level.

In addition, Windows system programming is closely related to hardware, so it is possible to understand the operation principle of the hardware and implement the interface between hardware and software.

With these attractive aspects, Windows system programming has become a field where you can develop more efficient and high-performance software based on a deep understanding of the operating system and hardware.

<br/>

### Visual Studio Setting

-   In Solution 'WindowsSystemProgramming' Propertry Pages
-   'Current selection' Check
-   Build & Run

<br/>

<br/>

## 1. Synchronization

### 1-1. Thread & Synchronization

**Windows system elements required for thread synchronization**

**Concepts and Needs of Synchronization**

-   **Elements of Synchronization**
    -   Scheduling processes, Kernel objects, Threads, Thread scheduling
    -   Thread Programming Pattern

**_Thread_**

└─1. [CreateThread](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/01_CreateThread/CreateThread.cpp)

└─2. [ThreadExcept](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/02_ThreadExcept/ThreadExcept.cpp)

└─3. [MyThread](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/03_MyThread/MyThread.cpp)

<br/>

-   **Thread Synchronization**
    -   The Meaning and Necessity of Synchronicization from the Perspective of **Atomicity** and **Instruction Reordering**

**_Instruction Reordering_**

└─4. InterLock

└─5. MemBarrier

<br/>

<br/>

---

### 1-2. Synchronize with Kernel Objects

**Describes the sync wait functions and sync-only kernel objects provided by Windows**

**Usefulness and utilization of threads or window messages**

-   **Thread Synchronization API**
    -   Description of the standby function that waits for threads to synchronize
    -   Understand the relationship between the standby function and the kernel object

└─6. WaitThread

└─7. MyThread2

└─8. WaitMultiThreads

<br/>

-   **Synchronization objects for data protection**
    -   Describe Mutex and Semaphore from the perspective of protection of shared resources

└─9. MutexTest

└─10. MutexNoti

**_Semaphore_**

Mutexes provide exclusive access to a shared resource, while semaphores control access to a pool of resources.

└─11. [SemaphoreTest](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/11_SemaphoreTest/SemaphoreTest.cpp)

└─12. TPSemaphore

└─13. WQSemaphore

<br/>

-   **Synchronization object for flow control**
    -   Description of synchronization-only objects for the purpose of controlling and notifying flows between threads.

└─14. ExitWithEvent

└─15. MyThread(3)\_ModifyExit

└─16. EventTest

└─17. MultiSyncWaits

└─18. EventNotify

└─19. WQNotify

└─20. EventNotify2

**_Using Events for RW Lock_**

└─21. EvtRWLock

└─22. [EvtRWLock2](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/22_EvtRWLock2/EvtRWLock2.cpp)

**_Waitable Timer_**

└─23. [WaitableTimerTest](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/23_WaitableTimerTest/WaitableTimerTest.cpp)

└─24. [WaitableTimerTest2](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/24_WaitableTimerTest2/WaitableTimerTest2.cpp)

└─25. [ManualWTTest](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/25_ManualWTTest/ManualWTTest.cpp)
