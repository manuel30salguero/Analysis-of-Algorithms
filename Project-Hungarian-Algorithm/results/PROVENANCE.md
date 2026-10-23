# Results provenance

`environment.json` records the machine, compiler, flags, CPU information, and timing procedure used for the final C++ measurements. `analysis_environment.json` records the Python versions used for post-processing.

`dataset_sha256.json` records the SHA-256 hash of each of the 21 KITTI tracking training label files used in the case study. The original archive and extracted labels are included under `data/kitti/` in this repository.

`final_source_sha256.json` records hashes of the final C++ sources, headers, tests, and Python experiment/analysis tools contained in this repository.

The main raw measurement files are:

- `full.csv`
- `small.csv`
- `rect.csv`
- `memory.csv`
- `kitti_pairs.csv`
- `kitti_timing.csv`
- `kitti_objects.csv`
- `kitti_inventory.csv`

Other CSV and JSON files in `results/` are derived summaries used in the analysis.

`final_validation/` contains the build, test, sanitizer, and exhaustive-oracle evidence for the final implementation.

Timing values can vary across machines. Deterministic costs, fingerprints, certificate checks, and inspection counts are the stronger reproducibility checks for the algorithmic results.
