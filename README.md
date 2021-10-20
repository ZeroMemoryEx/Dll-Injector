# Dll-Injector
DLL injection is a method of executing arbitrary code in the address space of a separate live process.

# TECHNICAL DETAILS

* Process Access Rights
* Open process 
* GetProcAddress-LoadLibraryA (used to load a DLL into the process memory) 
* VirtualAllocEx (used to allocate space from the target process virtual memory)
* WriteProcessMemory (used to write the path of the DLL file into the allocated memory)
* CreateRemoteThread (used to creates a thread in the virtual memory area of a process)

# DEMO

![Alt Text](https://raw.githubusercontent.com/ZeroM3m0ry/Dll-Injector/master/demo.gif)

