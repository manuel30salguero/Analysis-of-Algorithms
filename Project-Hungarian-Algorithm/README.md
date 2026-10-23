# Hungarian Algorithm Project

**Manuel Elías Salguero Fernández**  
Analysis of Algorithms  
Yachay Tech University

This project studies the **Hungarian Algorithm** for solving the linear assignment problem.

The main objective is to understand how the algorithm works, analyze its correctness and complexity, and compare its behavior with a Greedy approach and exhaustive search.

The implementation was developed in **C++17**.

## Main topics

The project includes:

- Hungarian Algorithm implementation
- Greedy assignment
- Exhaustive search for small instances
- Square and rectangular cost matrices
- Correctness tests
- Runtime and memory experiments
- Synthetic cost-matrix experiments
- Vehicle association using the KITTI tracking dataset

## Project structure

```text
Project-Hungarian-Algorithm/
│
├── include/        C++ header files
├── src/            Main C++ implementation
├── tests/          Tests and verification
├── tools/          Python scripts for running and analyzing experiments
├── results/        Experimental results and generated figures
├── data/           Example matrix and KITTI tracking labels
├── docs/           Additional experiment documentation
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Build

Using Make:

```bash
make -j2
make test
```

Using CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

## Example

The small example used in the report is

```text
4 1 3
2 0 5
3 2 2
```

The Hungarian Algorithm returns the assignment

```text
1 -> 2
2 -> 1
3 -> 3
```

with total cost

```text
5
```

while the Greedy method obtains a total cost of

```text
6
```

## Experiments

The experiments analyze different types of cost matrices, including:

- uniform random matrices
- planted diagonal matrices
- near-tie matrices
- equal-cost matrices
- adversarial matrices

The project also compares runtime, number of inspected costs, memory usage, and assignment quality.

## KITTI case study

The KITTI tracking training annotations are used as a real-data case study.

For the experiments:

- only objects of class `Car` are used;
- bounding-box geometry is used to construct the cost matrix;
- vehicle IDs are used only after assignment for evaluation;
- center distance and IoU are tested as association costs.

The dataset used by the experiments is located in:

```text
data/kitti/training/label_02/
```

and contains sequences:

```text
0000.txt
...
0020.txt
```

The original downloaded archive is also included in:

```text
data/kitti/data_tracking_label_2.zip
```

The KITTI experiment is used to study data association only. It is not a complete vehicle-tracking system and it is not an official KITTI leaderboard submission.

## Reproducing the experiments

The main experiment script can be executed with:

```bash
python3 tools/run_experiments.py \
  --kitti-labels data/kitti/training/label_02 \
  --out results/reproduction
```

Results can then be analyzed with:

```bash
python3 tools/analyze_results.py --results results/reproduction
python3 tools/analyze_kitti.py --results results/reproduction
```

## Main conclusion

The Hungarian Algorithm guarantees the minimum total cost for the assignment problem defined by the cost matrix.

However, the KITTI experiments also show an important limitation:

> A lower assignment cost does not necessarily imply a better semantic vehicle association.

Therefore, the quality of the final result depends not only on the optimization algorithm, but also on how the assignment cost is defined.

