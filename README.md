# Production Optimization in Automotive Manufacturing

This project focuses on designing and implementing optimization techniques to improve efficiency in automotive production lines. The proposed solutions explore methods such as backtracking, greedy, and metaheuristic strategies to minimize production constraint violations.

## Problem Overview

A sequence of cars must be scheduled along an assembly line. Each car may require certain improvements that are installed at specific stations. Due to capacity limitations, each station can only handle a limited number of installations within a sliding window of consecutive cars. Violations of these constraints generate penalties that must be minimized.

## Definitions and Notation

* `C`: Total number of cars to be produced
* `M`: Total number of possible improvements
* `K`: Number of car classes (each class defines a pattern of improvements)
* `n_e`: Size of the sliding window at station `e`
* `c_e`: Maximum number of cars within `n_e` that may require improvement `e`

## Constraint Violations and Penalties

If, within any window of `n_e` consecutive cars, more than `c_e` require the same improvement, a penalty is applied.

The penalty is computed as:

```
penalty = k − c_e
```



All violations across all stations are accumulated to compute the total penalty.
Incomplete windows at the beginning and end of the production sequence are also evaluated.

## Input Specification

The input file follows this structure:

1. Three strictly positive integers: `C M K`
2. One line with `M` strictly positive integers defining the values of `c_e`
3. One line with `M` strictly positive integers defining the values of `n_e`
4. `K` additional lines, each describing a car class:

   * Class identifier
   * Number of cars belonging to this class
   * A binary vector of length `M` indicating required improvements

## Program Execution

The program must be executed with two command-line arguments:

1. Path to the input file
2. Path to the output file

## Example

```
Input:              Output:
10 5 3              3 2.3
1 1 1 2 1           0 1 0 1 2 0 2 0 2 1
2 2 2 3 2
0 4 1 1 0 0 1
1 3 0 1 0 1 0
2 3 0 0 1 0 0
```

The output consists of the total penalty followed by the optimized production sequence.

## Authors

* Maria Risques Montalban
* Elena Gonzalez Rodriguez
