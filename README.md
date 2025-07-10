# CacheLab Project — Rutgers CS211 Summer 2025

## Overview

This project has two parts:

- **Part A:** `csim.c` — a cache simulator for a configurable single-level cache.  
- **Part B:** `ctuner.c` — a cache tuner that finds the optimal cache configuration for a given trace and metric.

Both programs are written in C and tested against reference traces and autograders.

## Getting Started

### Prerequisites

- Linux environment (tested on Rutgers ilabs)  
- GCC compiler  
- Python 3 

## Objective

Build a cache simulator and tuner in C to understand cache memory behavior and optimization.

---

## Part A — Cache Simulator

- Supports arbitrary sets, associativity, and block sizes.  
- Parses Valgrind trace files (loads, stores, modifies).  
- Implements LRU eviction policy.  
- Supports verbose output.  
- Passed all provided and hidden test cases matching the reference simulator.

---

## Part B — Cache Tuner

- Exhaustive search over cache configs to optimize hit, miss, or eviction rates.  
- Parses simulator output to evaluate performance.  
- Finds smallest parameter config meeting target metric.  
- Passed all tests with correct outputs or proper no-solution messages.

---

## Tools & Environment

- Developed and tested on Rutgers ilabs Linux machines.  
- Used GCC, Make, and Python 3 for testing.

---
