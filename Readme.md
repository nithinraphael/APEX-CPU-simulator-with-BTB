# APEX CPU Implementation with BTB Support

This project implements the APEX CPU specification, with Branch Target Buffer (BTB) support. The project uses `clang++` as the compiler and C++20 as the standard.

## NOTE

This project adopts Rust's approach to error handling, ensuring maximum safety. It also incorporates several other concepts from Rust for enhanced performance and reliability. \

To run the project, first change the input file in the Makefile (INPUT_FILE = 2t3.asm) to the desired input. Then, use the following command:

## Key Features

- **Modern C++**: Ensures efficient, maintainable code.
- **RAII Pattern**: Manages resource allocation and deallocation.
- **Smart Pointers**: Provides automatic, exception-safe memory management.
- **Optional**: Represents optional data safely.

## Prerequisites

Ensure you have the following installed on your system:

- clang++
- make

## Building the Project

To build the project, navigate to the project directory in your terminal and run the following command:

```bash
make all
```

This will create a build directory and compile the main.cpp file into an executable named apex-sim.

Running the Project
To run the project, use the following command:

```bash
make run
```

This will run the apex-sim executable with the input file 2t3.asm located in the input directory.

Cleaning the Project
To clean the project (remove the build files), use the following command:

```bash
make clean
```

Clean, Build, and Run
To clean the project, build it, and then run it, use the following command:

```bash
make cbr
```

Debugging the Project
To debug the project using lldb, use the following command:

```bash
make cbd
```
