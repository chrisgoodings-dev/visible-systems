# Artifact Evaluation

Visible Systems provides research and teaching artifacts intended to support replication, validation, and educational reuse.

This document describes the scope and expectations of the available artifacts, following ACM Artifact Evaluation principles.

---

## Artifact Scope

The artifacts include:

- C++ source code for experimental labs
- Benchmarking and instrumentation infrastructure
- Scripts for data collection and analysis
- Documentation supporting educational and research use

Artifacts are designed to be **self-contained**, **buildable**, and **inspectable**.

---

## Claims Supported by the Artifacts

The artifacts support claims related to:

- Observable effects of memory layout and access patterns
- Performance differences measurable through experimentation
- The feasibility of teaching systems concepts through observable behavior

They are not intended to claim absolute performance numbers across hardware.

---

## Build & Execution

Artifacts are known to build and run on:

- Windows 10+ with MSVC (Visual Studio 2022)
- CMake 3.20 or newer

Build instructions are provided in `docs/getting-started.md`.

---

## Reproducibility Expectations

Due to hardware and operating system variability:

- Absolute timings may vary
- Relative trends and qualitative outcomes should be consistent

Reviewers are encouraged to focus on **directional effects**, not raw numbers.

---

## Limitations

- Hardware performance counters may be limited on some platforms
- CI results are for build validation, not performance benchmarking

These limitations are documented explicitly to avoid misleading conclusions.

---

## Contact

For artifact-related questions, reviewers are encouraged to open an issue or contact the project maintainer.
