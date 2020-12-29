# Windows System Programming

> Windows system programming from the perspective of synchronous and asynchronous Input/Output.

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

<br/>

-   **Elements of Synchronization**
    -   Scheduling processes, Kernel objects, Threads, Thread scheduling
    -   Thread Programming Pattern

**_Thread_**

└─1. [CreateThread](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/01_CreateThread/CreateThread.cpp)

└─2. [ThreadExcept](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/02_ThreadExcept/ThreadExcept.cpp)

└─3. [MyThread](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/03_MyThread/MyThread.cpp)

<br/>

-   **Thread Synchronization**
    -   The Meaning and Necessity of Synchronicization from the Perspective of **Atomicity** and **Instruction Reordering**

**_Instruction Reordering_**

└─4. [InterLock](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/04_InterLock/InterLock.cpp)

└─5. [MemBarrier](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/05_MemBarrier/MemBarrier.cpp)

<br/>

<br/>

---

### 1-2. Synchronization with Kernel Objects

**Describes the sync wait functions and sync-only kernel objects provided by Windows**

**Usefulness and utilization of threads or window messages**

<br/>

-   **Thread Synchronization API**
    -   Description of the standby function that waits for threads to synchronize
    -   Understand the relationship between the standby function and the kernel object

**_Thread Waiting Functions_**

└─6. [WaitThread](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/06_WaitThread/WaitThread.cpp)

└─7. [MyThread2](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/07_MyThread2/MyThread2.cpp)

└─8. [WaitMultiThreads](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/08_WaitMultiThreads/WaitMultiThreads.cpp)

<br/>

-   **Synchronization objects for data protection**
    -   Describe Mutex and Semaphore from the perspective of protection of shared resources

**_Mutex_**

_Mutually exclusive access_

└─9. [MutexTest](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/09_MutexTest/MutexTest.cpp)

└─10. [MutexNoti](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/10_MutexNoti/MutexNoti.cpp)

**_Semaphore_**

_Mutexes provide exclusive access to a shared resource, while semaphores control access to a pool of resources._

└─11. [SemaphoreTest](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/11_SemaphoreTest/SemaphoreTest.cpp)

└─12. [TPSemaphore](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/12_TPSemaphore/TPSemaphore.cpp) : Thread Pool Using Semaphore

└─13. [WQSemaphore](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/13_WQSemaphore/WQSemaphore.cpp) : Waitable Queue

<br/>

-   **Synchronization object for flow control**
    -   Description of synchronization-only objects for the purpose of controlling and notifying flows between threads.

**_Event_**

└─14. [ExitWithEvent](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/14_ExitWithEvent/ExitWithEvent.cpp)

└─15. [MyThread3](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/15_MyThread3/MyThread3.cpp)

└─16. [EventTest](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/16_EventTest/EventTest.cpp)

└─17. [MultiSyncWaits](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/17_MultiSyncWaits/MultiSyncWaits.cpp)

**_Using Events as Signaling Mechanisms_**

└─18. [EventNotify](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/18_EventNotify/EventNotify.cpp)

└─19. [WQNotify](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/19_WQNotify/WQNotify.cpp)

└─20. [EventNotify2](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/20_EventNotify2/EventNotify2.cpp)

**_Using Events for RW Lock_**

└─21. [EvtRWLock](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/21_EvtRWLock/EvtRWLock.cpp)

└─22. [EvtRWLock2](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/22_EvtRWLock2/EvtRWLock2.cpp)

**_Waitable Timer_**

└─23. [WaitableTimerTest](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/23_WaitableTimerTest/WaitableTimerTest.cpp)

└─24. [WaitableTimerTest2](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/24_WaitableTimerTest2/WaitableTimerTest2.cpp)

└─25. [ManualWTTest](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/25_ManualWTTest/ManualWTTest.cpp)

└─26. [WakeSleepSystem](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/26_WakeSleepSystem/WakeSleepSystem.cpp)

└─27. [SetWaitableTimerEx](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/27_SetWaitableTimerEx/SetWaitableTimerEx.cpp)

<br/>

-   **Notification**
    -   Additional means and methods for synchronization in terms of notification, which can provide more flexibility in implementing synchronization, along with the mutex, semaphore, event, and waitable timer, all of which are dedicated synchronization objects.

**_Integration of Thread and Message Queue_**

└─28. [MsgNotify](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/28_MsgNotify/MsgNotify.cpp)

└─29. [MsgQueue](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/29_MsgQueue/MsgQueue.cpp)

└─30. [MsgWaitMulti](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/30_MsgWaitMulti/MsgWaitMulti.cpp)

**_Combining with Callback Functions_**

└─31. [WaitPool](https://github.com/gunh0/Windows_System_Programming/blob/master/WindowsSystemProgramming/31_WaitPool/WaitPool.cpp)

**_.NET BackgroundWorker_**

└─32. [BackgroundWorker](https://github.com/gunh0/windows-system-programming/tree/master/WindowsSystemProgramming/32_BackgroundWorkerr)

<br/>

<br/>

---

### 1-3. User Mode Synchronization

**Windows provides various synchronization kernel objects that operate in kernel mode, as well as synchronization objects and APIs that operate in user mode.**

**In user mode, a combination of spin lock and kernel-mode switching or a separate method is used for synchronization.**

**Therefore, one of the advantages of user-mode synchronization is its speed.**

**By minimizing the switch to kernel mode, thread context switching can also be reduced, making it faster than synchronization using kernel objects.**

<br/>

-   **Critical Section**

    -   A critical section is a block of code that must be executed atomically, which means that only one thread can execute it at a time. The purpose of a critical section is to prevent race conditions, which occur when two or more threads access shared resources simultaneously and interfere with each other's operations.

    -   In Windows, a critical section is a user-mode synchronization object that provides a lightweight and efficient way of protecting shared resources. It works by using a spin lock mechanism, which means that a thread repeatedly checks if the critical section is available until it can acquire it.

    -   To use a critical section, a thread must first call the InitializeCriticalSection function to create it. Then, the thread can enter the critical section by calling the EnterCriticalSection function and leave it by calling the LeaveCriticalSection function. If a thread tries to enter a critical section that is already locked by another thread, it will be blocked until the critical section becomes available.

    -   Critical sections are a useful synchronization mechanism for protecting small sections of code that access shared resources frequently. However, they are not suitable for protecting resources that are accessed infrequently or for long periods of time, as they can cause excessive spinning and waste CPU cycles. In these cases, more heavyweight synchronization mechanisms, such as mutexes or semaphores, should be used instead.

**_Using Critical Section_**

└─33. [LockAccount](https://github.com/gunh0/Windows_System_Programming/tree/master/WinSysProg_Csharp/33_LockAccount)

└─34. [CriticalSectionAccount](https://github.com/gunh0/windows-system-programming/tree/master/34_CriticalSectionAccount/CriticalSectionAccount.cpp)

└─35. [CriticalSectionTest](https://github.com/gunh0/windows-system-programming/tree/master/35_CriticalSectionTest/CriticalSectionTest.cpp)

└─36. [MyCSLock](https://github.com/gunh0/windows-system-programming/tree/master/36_MyCSLock/MyCSLock.cpp)
