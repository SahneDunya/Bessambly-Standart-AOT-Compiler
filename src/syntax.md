# Bessambly Syntax
Bessambly is a hardware-independent, lowest-level programming language. While not based on any programming language, its low-level structure gives it a syntax similar to Assembly Language. Our goal is to provide developers with similar control over hardware by providing an abstracted representation of machine code, while offering portability across different architectures.

Bessambly source code files have the .bsm extension.

## Core Philosophy and Structure
Bessambly supports imperative programming. This means you work with a sequence of instructions that directly change the state of the program. The language lacks a module system and a standard library, making it extremely minimalist and lightweight.

## Control Flow
Control flow in Bessambly is achieved through the direct use of jump instructions and flags, unlike constructs like if, for, and while in traditional high-level languages. This approach offers an experience closer to Assembly Language and provides absolute control over program execution.

### jump: Enables unconditional or conditional jumps to a specific labeled point in the program.
### flag: Binary status variables used to indicate the results of operations (e.g., comparison results, arithmetic overflows). Jump instructions are generally determined by the state of these flags.
Example (Conceptual):
```
CMP R1, R2 ; Compare R1 and R2, set the flags
JEQ EQUAL_LABEL ; If equal, jump to EQUAL_LABEL

; If not equal, the code here executes

EQUAL_LABEL:
; If equal, the code here executes
```
## System Calls and Interaction
Bessambly uses system calls for situations that require interaction with the outside world, such as I/O operations, device access, networking, IPC (Interprocess Communication), dynamic memory management, and accessing system services.

These system calls may, in some cases, require code that is dependent on the underlying hardware or operating system. However, alternative approaches have been presented to maintain Bessambly's hardware independence goal:

### Boot Calls: Provide access to specific functions directly at the system startup level.
### Virtual Machine Calls (Hypercalls, VM Exits, etc.): Bessambly can use its own Virtual Machine (BVM) calls, as well as calls from other popular virtual machines such as WebAssembly (WASM), JVM, and .NET. This allows Bessambly code to be dependent on a specific virtual machine rather than the hardware itself. Virtual machine calls are the preferred way to ensure Bessambly's hardware independence, but in this case, the code becomes dependent on the virtual machine in which it is running.
Syntactically, system calls are typically triggered by a special instruction or call mechanism and operate by loading the required parameters into designated "registers" (or similar temporary storage areas).

Example (Conceptual System Call):
```

MOV R0, FILE_HANDLE_ID ; Load the file handle into R0
MOV R1, BUFFER_ADDR ; Load the data buffer address into R1
MOV R2, LENGTH ; Load the length to be read into R2
SYSCALL READ_FILE ; Execute the file read system call
```
## Relationship Between Bessambly and Assembly
### Assembly is a direct representation of machine code and is tightly tied to a specific CPU architecture. Each CPU architecture has a different assembly language.

### Bessambly, on the other hand, is designed as a hardware-abstracted version of assembly language. Bessambly abstracts concepts such as hardware-specific registers or addressing modes through a general virtual architecture, providing programmers with a general virtual architecture. This allows Bessambly code to run on different hardware platforms, either through the Bessambly Virtual Machine (BVM) or directly by compiling.

## Development Tools
Bessambly comes with the following tools to support the development process:

### Bessambly AOT (Ahead-of-Time) Compiler: This compiler directly translates Bessambly code into machine code for a specific target platform. This compiled code runs directly on real hardware without requiring the Bessambly Virtual Machine (BVM). It is ideal for performance-critical applications and system-level programming.
### Bessambly JIT (Just-In-Time) Compiler: Instead of directly translating Bessambly code into machine code, it compiles it into intermediate code. There are two main intermediate code formats supported by the JIT compiler:
#### WebAssembly (.wasm): This is an industry-standard intermediate format for portability across the web platform and other virtual machines.
#### VirtualBessambly (.vbsm): This is an optimized intermediate format designed specifically for Bessambly's native virtual machine, the BVM. The JIT compiler is beneficial for dynamic adaptability and rapid development cycles.
#### Bessambly Virtual Machine (BVM): A virtual execution environment that runs Bessambly code (especially .vbsm intermediate code) in a hardware-independent manner. It provides portability, a security layer, and dynamic optimization capabilities.

Bessambly's syntax aims to provide hardware independence, a fundamental requirement of modern software, while offering the power and control of low-level programming.
