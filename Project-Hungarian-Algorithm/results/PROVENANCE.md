# Measurement and release provenance

`environment.json` records the final serial, CPU-0-pinned C++ measurement run. `dataset_sha256.json` identifies the official archive and all 21 label files without redistributing them. `analysis_environment.json` identifies the Python environment.

`source_sha256.json` is the measurement runner's source snapshot; `measurement_source_sha256.json` preserves the same snapshot explicitly. Every delivered C++ source, header and test matches it. `final_source_sha256.json` covers final C++ sources and Python tools, including subsequent analysis/figure changes and memory-output hardening of the runner.

The initial run produced nine memory rows although all process exits succeeded. All ten measurements were repeated in fresh processes on the same CPU with the unchanged binary. The runner now captures, checks and flushes memory output, and analysis requires exactly ten records. See `MEMORY_RERUN_NOTE.md`. Matrix and KITTI timings were unaffected; delivered `memory.csv` contains only the complete rerun.

`full.csv`, `small.csv`, `rect.csv`, `memory.csv`, `kitti_pairs.csv`, `kitti_timing.csv`, `kitti_objects.csv` and `kitti_inventory.csv` are final raw evidence. Other CSVs and JSON summaries are derived analysis. `final_validation/` contains clean build/test/sanitizer logs and per-instance exhaustive oracle results. No previous synthetic vehicle-run CSV is part of the release.

The complete project's `SHA256SUMS.txt` hashes released files and component archives. It is a content-integrity manifest, not a claim that timings will be identical on another machine.
