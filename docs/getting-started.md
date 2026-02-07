# Getting Started

Welcome to Visible Systems.

This guide will help you build and run the labs on your local machine.

---

## Prerequisites

- Windows 10 or later
- Visual Studio 2022 (MSVC)
- CMake 3.20 or newer
- Python 3 (optional, for plotting)

---

## Build Instructions

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
