# TCAS-II Simulator

> *Work in progress, this project is currently under active development. Some features might be unstable.*

A soft real-time, multithreaded Traffic Collision Avoidance System (TCAS-II) simulation in C using POSIX threads.

This project simulates airspace traffic, evaluates collision threats (Traffic Advisories & Resolution Advisories), and displays the environment on a terminal-based radar screen.

## Features

* **Multithreaded Architecture:** Independent threads for radar tracking, threat logic calculation, and display rendering.
* **Thread-Safe Parallelism:** Custom storage implementation utilizing mutex locks to prevent race conditions during state updates.
* **Real-Time Radar Display:** Terminal UI built with the `ncursesw` library to visualize aircraft positions and threat levels.
* **Metric System Physics:** All internal calculations rely on standard metric units (meters, m/s).
* **Extensive Documentation:** Fully documented with Doxygen, utilizing custom CSS themes and call/caller sequence graphs.

## Project Structure

```text
tcas-ii-sim/
├── src/                                  # Source files (.c) containing the implementations
├── include/                              # Header files (.h) with public APIs and definitions
├── build/                                # Compiled object files and the final executable
├── docs/                                 # Generated Doxygen HTML documentation
├── LICENSE                               # MIT license
├── Makefile                              # Automated build rules
├── Doxyfile                              # Doxygen configuration file
├── doxygen-awesome.css                   # Doxygen extra theme file
├── doxygen-awesome-sidebar-only.css      # Doxygen extra theme file
└── custom-style.css                      # Doxygen extra theme file
```

## Dependencies

To build and run this project, ensure you have the following installed on your Linux environment:

* **GCC** (GNU Compiler Collection)
* **Make**
* **NCurses** (Wide character support: `libncurses-dev`)
* **Doxygen & Graphviz** (For generating documentation and graphs)
* **Mscgen** (For Message Sequence Charts)

## Build and Run

The project uses a standard Makefile for compilation. The build process automatically handles dependency tracking and object file isolation.

1. **Compile the project:**
```bash
   make
```
2. **Run the simulation:**
```bash
   ./build/tcas-ii-sim
```
3. **Clean the build environment:**
```bash
   make clean
```

## Documentation

This project is documented using Doxygen with the `doxygen-awesome-css` dark theme.

To generate the latest HTML documentation:

```bash
   doxygen Doxyfile
```

Once generated, open `docs/html/index.html` in your preferred web browser to explore the API references, structs, enums, and sequence graphs.

## Acknowledgments

[IVAO Documentation Library, Traffic collision avoidance system - TCAS](https://wiki.ivao.aero/en/home/training/documentation/Traffic_collision_avoidance_system-TCAS)

## Author

Yusuf Efe Aktaş

## License

This project is licensed under the MIT License.