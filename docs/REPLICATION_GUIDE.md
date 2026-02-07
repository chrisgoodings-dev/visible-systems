# Replication Guide

This guide is intended to help reviewers and researchers reproduce the experiments reported using Visible Systems.

---

## Intended Audience

This guide assumes familiarity with:
- C++
- Basic performance benchmarking concepts
- Command-line tools on Windows

No prior experience with Visible Systems is required.

---

## Step 1: Environment Setup

Recommended environment:
- Windows 10 or later
- Visual Studio 2022 (MSVC)
- CMake 3.20+
- Release build configuration

Optional:
- Python 3 for plotting results

---

## Step 2: Build the Artifacts

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
