# Hungarian Algorithm research project

Manuel Elías Salguero Fernández  
School of Mathematical and Computational Sciences, Yachay Tech University  
manuel.salguero@yachaytech.edu.ec  
ORCID: https://orcid.org/0009-0009-6747-7245

The central subject is exact linear assignment in **C++17**. Controlled synthetic cost matrices study correctness, complexity, structure, Greedy quality, and memory. Real KITTI training annotations provide a consecutive-frame vehicle-association case study. There is no detector or persistent tracker in that experiment.

The research questions are: what invariant and certificate establish exactness; how the loops produce rectangular and square bounds; how matrix structure changes work, runtime and memory relative to global edge-sorting Greedy; and whether geometric objective optimality implies correct vehicle identity. Retained pair-level counterexamples answer the last question negatively.

## Build and validate

Linux or WSL, GCC/Clang with C++17, Make or CMake 3.16+, and a standard C++ library are required. Memory measurements use Linux `getrusage`; the driver uses CPU affinity and `lscpu`.

```bash
make -j2
make test
mkdir -p results/validation
./build/test_assignment results/validation/oracle.csv
./build/test_tracking results/validation/optional_oracle.csv
./build/lap_main data/example_matrix.txt
```

The recorded Make build uses `-O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic`. Alternative build:

```bash
cmake -S . -B build_cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build_cmake -j2
ctest --test-dir build_cmake --output-on-failure
```

Both builds passed. CMake 4.4.4 and GCC 13.3.0 were used in the recorded validation. The tests use exceptions rather than assertions disabled by `NDEBUG`. Final validation: 2,946 core oracle instances / 19,281 checks; 3,962 optional oracle instances / 7,962 association checks. AddressSanitizer and UBSan passed with leak detection disabled. Logs and per-instance oracle CSVs are in `results/final_validation/`.

The worked matrix returns Hungarian cost 5, Greedy cost 6, zero-based columns `(1,0,2)`, row potentials `(3,2,2)`, column potentials `(0,-2,0)`, and a passing certificate. A custom input begins with `rows cols`, followed by row-major integer costs.

## Reproduce all measurements

Obtain the official KITTI tracking **training labels** and follow `data/README.md`. Do not add labels or the archive to Git. Install Python analysis dependencies from `requirements-analysis.txt`.

```bash
python3 tools/run_experiments.py \
  --kitti-labels data/kitti/training/label_02 \
  --out results/reproduction --compiler g++ \
  --build-flags='-O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic'
python3 tools/analyze_results.py --results results/reproduction
python3 tools/analyze_kitti.py --results results/reproduction
```

The runner executes C++ experiments serially and pins them to one available CPU. It records environment, flags, source hashes and dataset hashes. Supply the actual flags if using a different build. Matrix generation, exhaustive search, assignment and timing loops are C++; Python only orchestrates and analyzes results. Use a new output directory. Times vary across machines; deterministic fingerprints, objectives and inspection counts should reproduce.

Individual commands:

```bash
./build/lap_benchmark full results/new_full.csv
./build/lap_benchmark small results/new_small.csv
./build/lap_benchmark rect results/new_rect.csv
./build/lap_benchmark memory hungarian 2048 uniform
./build/lap_benchmark memory greedy 2048 uniform
./build/lap_tracking_experiment data/kitti/training/label_02 results/new_kitti
```

The repository focuses on the C++ implementation, validation, experiments, and retained results. The manuscript and presentation are hosted separately for course submission.

## Algorithms and numerical contract

Hungarian uses row-minimum initialization, minimum slacks, alternating paths, deterministic ties, and explicit dual certificates. The oriented time bound is O(r²c), with r no greater than c. Wide/square auxiliary storage is O(r+c); tall input adds an O(rc) transpose. Greedy sorts every edge by `(cost,row,column)`, costing O(rc log(rc)) time and O(rc) storage. Exhaustive search enumerates all injections without pruning and is restricted to dimensions at most ten.

Fixed-cardinality assignment returns exactly `min(rows,cols)` pairs. Optional association uses an explicit dummy matrix of order `previous+current`. Greedy sorts beneficial admissible real pairs, accepting only disjoint endpoints. Equal zero savings remains unmatched under Greedy's rule. Costs are signed 64-bit integers bounded by ±1,000,000,000; dimensions are bounded by 10,000. This numeric contract is not a practical performance guarantee. Geometry costs are quantized at scale 1,000,000.

## KITTI protocol and results

`docs/KITTI_PROTOCOL.json` freezes parameters before identity evaluation. The baseline uses center distance divided by 80 pixels, or one minus IoU; gates are distance at most 80 pixels or IoU at least 0.05. Each unmatched endpoint costs 0.6. The reader requires 17 fields and retains Car only, including all occlusion/truncation codes. Frame spans include empty positions. Geometry-only canonical ordering removes annotation-order identity information. IDs are read by the evaluator after both assignments. No ID, occlusion or truncation enters costs, gates, tie-breaking or motion.

All 21 training sequences yield 8,008 frames, 7,987 pairs, 27,300 Car boxes and 26,719 eligible continuations. Hungarian precision is 99.45% for distance and 99.58% for IoU; Greedy gives 98.32% and 99.09%. Lower Hungarian objective coexists with fewer correct identities on 12 distance pairs and three IoU pairs. The case study measures annotated-box association, not full tracking or an official leaderboard score. Full aggregate, per-sequence, subgroup, sensitivity and counterexample results are included.

## File map

| Path | Purpose |
|---|---|
| `include/`, `src/assignment.cpp` | C++ APIs, exact/Greedy/exhaustive solvers and matrix generation |
| `src/tracking.cpp` | Geometry costs, dummy construction and strict KITTI parser; small synthetic fixtures for tests only |
| `src/tracking_experiment.cpp` | Fixed consecutive-frame KITTI experiment, certificates and evaluation |
| `tests/` | Independent enumeration, edge cases, parser and ID invariance checks |
| `tools/` | Serial measurements, analysis and slide-figure generation |
| `results/` | Final raw CSVs, summaries, provenance and validation logs |
| `docs/RESULTS_SCHEMA.md` | Column meanings, aggregation and numerical traceability |
| `data/README.md` | Official dataset acquisition, hash and placement instructions |

The release contains 966 full-matrix, 405 small-matrix and 210 rectangular timing rows plus ten memory rows. KITTI contains 159,740 pair/configuration/method rows, 95,844 timing rows, and 108,964 current-object rows. All-equal inspections equal n(n+1)(2n+1)/6; planted inspections equal n². Uniform Greedy median excess at n=1024 is 265.64%; the adversarial ratio is 250.25 (24,925% excess).

