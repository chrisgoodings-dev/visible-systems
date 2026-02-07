# Visible Systems

**Visible Systems** is an open-source C++ project for making performance, memory, and correctness *observable*.

Modern systems programming is hard to learn not because the concepts are inaccessible, but because their effects are hidden. Caches, memory layouts, data races, and undefined behavior all influence program behaviour—yet most teaching material abstracts them away.

Visible Systems exists to reverse that trade-off.

This project provides a collection of small, focused C++ laboratories that expose how real systems behave, supported by instrumentation, benchmarks, and visualisation tools. It is designed for learners, educators, and researchers who want to understand systems software without relying on folklore or guesswork.

---

## Goals

- Make low-level system behaviour visible and measurable
- Support reproducible performance and correctness experiments
- Provide teaching material that does not lie about real systems
- Bridge academic research and practical engineering

---

## Who This Is For

- Students learning systems programming or C++
- Educators teaching performance, memory, or concurrency
- Engineers who want deeper intuition about system behaviour
- Researchers studying performance, safety, or software pedagogy

---

## Project Structure

- **Labs** – focused experiments on memory, performance, and concurrency
- **Teaching materials** – guided exercises and student projects
- **Experiments** – reproducible research studies and datasets
- **Tools** – instrumentation, profiling, and visualisation support

Each lab is self-contained and includes:
- Learning objectives
- Experimental setup
- Expected observations
- Discussion prompts

---

## Core Design Principles

- No hidden abstractions
- No framework magic
- Observable behaviour over theoretical explanation
- Reproducibility over anecdotes

---

## Research & Citation

Visible Systems is designed to support empirical research. If you use this project in academic work, please cite the repository and associated publications (listed in `/docs/research-methodology.md`).

---

## Contributing

Contributions are welcome—from bug fixes to new labs, experiments, or teaching material. See `CONTRIBUTING.md` for guidelines.

---

## License

MIT License
