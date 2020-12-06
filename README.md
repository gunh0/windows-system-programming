# Windows System Programming

***Window system programming from the perspective of synchronous and asynchronous Input/Output.***

<br/>

<br/>

# 1. Synchronization (동기화)

### 1-1. Thread & Synchronization (스레드와 동기화)

**Windows system elements required for thread synchronization**

**Concepts and Needs of Synchronization**

---

- **Elements of Synchronization / 동기화의 요소**
  - Scheduling processes, Kernel objects, Threads, Thread scheduling
  - Thread Programming Pattern

>1. [CreateThread](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/01_CreateThread/CreateThread.cpp)
>2. [ThreadExcept](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/02_ThreadExcept/ThreadExcept.cpp)
>3. [MyThread](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/03_MyThread/MyThread.cpp)

<br/>

- **Thread Synchronization / 스레드 동기화**
  - The Meaning and Necessity of Synchronicization from the Perspective of **Atomicity** and **Instruction Reordering**

>4. [InterLock](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/04_InterLock/InterLock.cpp)
>5. [MemBarrier](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/05_MemBarrier/MemBarrier.cpp)

<br/>

<br/>

### 1-2. Synchronize with Kernel Objects (커널 객체와 동기화)

**Describes the sync wait functions and sync-only kernel objects provided by Windows**

**Usefulness and utilization of threads or window messages**

-----

- **Thread Synchronization API / 스레드 동기화 API**
  - Description of the standby function that waits for threads to synchronize
  - Understand the relationship between the standby function and the kernel object

> 6. [WaitThread](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/06_WaitThread/WaitThread.cpp)
> 7. [MyThread2](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/07_MyThread2/MyThread.cpp)
> 8. [WaitMultiThreads](https://github.com/devgunho/Windows_System_Programming/blob/master/WindowsSystemProgramming/08_WaitMultiThreads/WaitMultiThreads.cpp)

<br/>

- **Synchronization objects for data protection / 데이터 보호를 위한 동기화 객체**
  - Describe Mutex and Semaphore from the perspective of protection of shared resources

> 9. MutexTest
> 10. MutexNoti
> 11. SemaphoreTest
> 12. TPSemaphore
> 13. WQSemaphore

<br/>

- **Synchronization object for flow control / 흐름 제어를 위한 동기화 객체** 
  - Description of synchronization-only objects for the purpose of controlling and notifying flows between threads.

> 14. ExitWithEvent
> 15. MyThread(3)_ModifyExit
> 16. EventTest
> 17. MultiSyncWaits
> 18. EventNotify
> 19. WQNotify
> 20. EventNotify2
> 21. EvtRWLock

