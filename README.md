# URecoverState

A program that captures the state of a process and let you revert back to it

<h1>Open source projects i used (and modified to my need are)</h1>

https://github.com/TheCruZ/kdmapper

https://github.com/rajkumar-rangaraj/PDB-Downloader/releases/tag/v1


<h1>TODO:</h1>

<ul>

<li>Make waiting functions(Sleep, WaitForSingleObjects...) resume execution not from the start.</li>

</ul>

<h1>Conclusions</h1>

<h2>Gs and Fs registers in x86-64 land</h2>
The gs and fs registers in x86-64 architecture are the only segment registers, witch mean they represent an offset
from some address, not the actual address. the gs register in windows contains the address of the current thread TEB struct. If you want to modify one of this registers you can use the writegsbase_u64 function witch is supported from user space. The problem with this function is that a context switch break's it, revert the value of gsbase to the address of the thread TEB struct from ETHREAD->TEB

<h2>RtlCloneUserProcess (The fork of windows)</h2>
The RtlCloneUserProcess functions clones the address space of the current process to a new created process, then it creates a new thread and start execution in the newly created process from the point it stoped in the old process.
The problem with this function is that it clones almost all of the old process address space, but for some unknown reason it done not clone some of the memory mapped files pages, so this pages were not allocated in the new process. 
One of this pages was the page that contained the gSharedInfo struct

<h2>Handles in the windows kernel</h2>
A process handle table is represented in the kernel as a _HANDLE_TABLE struct, this table contains _HANDLE_TABLE_ENTRY structs. if you want to find and address of an entry. if you want that a handle will point to another object you need to modify the entry to point to the old kernel object. Every handle table entry has a pointer to the next free handle table entry in the time of allocating the entry. So if you want to return an entry to the free list you can search from the last allocated entry of the table to the last free entry of the table
and point it to your entry.
    
    //Example:
    while(true){
        currentEntry = currentEntry->NextFreeHandleEntry; 
        if (currentEntry->NextFreeHandleEntry == nullptr) 
        {
            currentEntry->NextFreeHandleEntry = freeEntry
        } 
    }

<h2>Pid's and Tid's in the windows kernel</h2>
When creating a new process or thread the windows kernel get the new tid or pid from a _HANDLE_TABLE
struct in the kernel called PspCidTable. if you want to change a process or a thread tid you need to look up the new tid or pid entry in the table (ExLookupHandleTableEntry) and modify it's content to point to the old ETHREAD or EPROCESS object 

<h2>Pdb files and not exported functions and vars from the windows kernel</h2>
Some of the functions and structs i mentioned in hear are not exported from the windows kernel, so from hear i had two solutions the first solution  is to patter search every one of the addresses in the ntoskrnl.exe memory region at run time. The second solution is to extract the base address of the functions and symbols from windows pdb files.
I implemented the second solution into my project with not a lot of work.

