# Hungarian Algorithm Project

**Manuel Elías Salguero Fernández**  
Analysis of Algorithms — Yachay Tech University

This project studies the **Hungarian Algorithm** for the linear assignment problem. The implementation is written in **C++17** and is analyzed in terms of correctness, complexity, runtime, memory, and solution quality.

The project also compares Hungarian with a global edge-sorting Greedy method and exhaustive search on small instances. A KITTI vehicle-association case study is included as an application of the assignment problem.

## What is included

- Hungarian Algorithm implementation in C++17
- Greedy assignment baseline
- Exhaustive search for small instances
- Square and rectangular assignment
- Correctness and certificate checks
- Runtime, operation-count, and memory experiments
- Synthetic matrix families
- KITTI tracking training labels used in the case study
- CSV results and validation logs

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

The recorded experiments used C++17 with:

```text
-O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic
```

## Small example

The matrix in `data/example_matrix.txt` is

```text
3 3
4 1 3
2 0 5
3 2 2
```

The Hungarian Algorithm returns total cost **5**, while the Greedy baseline returns **6**.

Run it with:

```bash
./build/lap_main data/example_matrix.txt
```

## Experiments

The synthetic experiments use five matrix families:

- uniform random
- planted diagonal
- near ties
- all equal
- adversarial blocks

The project measures runtime, reduced-cost inspections, solution gaps, rectangular behavior, and memory usage.

Python is used only to automate repeated runs, read the CSV outputs, summarize results, and generate figures. The assignment algorithms, matrix generation, validation, and timed solver code are implemented in C++.

To reproduce the experiments:

```bash
python3 tools/run_experiments.py \
  --kitti-labels data/kitti/training/label_02 \
  --out results/reproduction \
  --compiler g++ \
  --build-flags='-O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic'

python3 tools/analyze_results.py --results results/reproduction
python3 tools/analyze_kitti.py --results results/reproduction
```

Python analysis dependencies are listed in `requirements-analysis.txt`.

## KITTI case study

The repository includes the KITTI Object Tracking **training labels** used in the project:

```text
data/kitti/training/label_02/0000.txt
...
data/kitti/training/label_02/0020.txt
```

The original downloaded archive is also included at:

```text
data/kitti/data_tracking_label_2.zip
```

Only objects of class `Car` are used. Bounding-box geometry constructs the assignment costs; track IDs are inspected only after Hungarian and Greedy produce their assignments so that identity correctness can be evaluated.

The experiment is a consecutive-frame association study using annotated boxes. It is **not** a complete tracking system and is not an official KITTI leaderboard submission.

The main KITTI result is that Hungarian gives better aggregate association accuracy for the declared costs, while some individual frame pairs show that a lower geometric assignment cost does not necessarily imply a better semantic identity match.

See `data/README.md` and `docs/KITTI_PROTOCOL.json` for the dataset and experiment details.

## Project structure

| Path | Purpose |
|---|---|
| `include/` | C++ headers |
| `src/` | Hungarian, Greedy, exhaustive search, benchmarks, and KITTI experiment |
| `tests/` | Correctness and edge-case tests |
| `tools/` | Experiment automation and result analysis |
| `results/` | Raw/derived CSVs, environment metadata, and validation logs |
| `data/` | Example matrix and KITTI labels used in the case study |
| `docs/` | KITTI protocol and results schema |

## Main conclusion

The Hungarian Algorithm guarantees a minimum-cost assignment for the supplied cost matrix. The experiments also show that this guarantee concerns the **numerical objective**: if the cost model does not fully represent semantic identity, minimum assignment cost alone does not guarantee the best real-world association.
