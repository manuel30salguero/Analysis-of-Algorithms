# Final results and numerical traceability

## Matrix experiments

`full.csv` has 966 rows: five families, seven dimensions, seven seeds for random families and one instance for deterministic families, two methods and three repetitions. `small.csv` has 405 rows: sizes 2–10, five seeds, three methods, three repetitions. `rect.csv` has 210 rows: five shapes, seven seeds, two methods, three repetitions.

Shared columns: `family,rows,cols,seed,algorithm,rep,inner,time_us,cost,optimum,absolute_gap,scans,expansions,workspace_bytes,fingerprint`. `time_us` is batch elapsed time divided by `inner`; no generation, CSV, certificate checking or counting is inside timing. Scans/expansions are recorded only for Hungarian and measured separately. Fingerprints are unsigned 64-bit strings; do not parse through floating point.

Aggregation takes the median repetition for each instance, then the median over seeds. Random-family IQRs describe instance medians; deterministic-family IQRs describe three timing repetitions. Fits use dimensions 64–1024 and are descriptive. Relative gaps are used only for positive benchmark optima.

`memory.csv` has ten fresh-process observations. `input_bytes` is cost storage; `workspace_bytes` sums owned vector element capacities, excluding input, headers, allocator metadata and sorting stack; `peak_rss_kib` is Linux process high-water RSS. It includes more than algorithm workspace. See `MEMORY_RERUN_NOTE.md` for the complete rerun after a missing initial output record.

## Verification

`final_validation/oracle.csv`: 2,946 core instances, dimensions, fingerprint, Hungarian/exhaustive/Greedy costs and certificate status. `optional_oracle.csv`: 3,962 dummy-augmented problems, order, fingerprint, exact costs and certificate status. Logs record 19,281 core and 7,962 association assertions, Make/CMake builds and address/undefined-behavior sanitizers with leak detection disabled.

## KITTI raw records

- `kitti_inventory.csv`: 21 sequences, frame spans, pairs, Car boxes, frames with Car, active pairs, maximum population and augmented order.
- `kitti_pairs.csv`: 159,740 records. Key `(sequence,frame,metric,config,method)`; `frame` is the current frame and previous is `frame-1`. Includes parameters, box counts, eligible continuations, gate-admissible continuations, predicted/correct/incorrect links, integer objective, common input fingerprint and certificate status. Certificate status is for the shared exact optimum, including on the Greedy row.
- `kitti_timing.csv`: 95,844 baseline batch records, key `(sequence,frame,metric,method,repeat)`. `batch` is the number of solves and `time_us` is per solve. Includes allocation, geometry checks and cost construction; excludes parsing, sorting annotations, scoring, certificates and writing.
- `kitti_objects.csv`: 108,964 baseline records, one per current Car and method/cost. Contains geometry-order detection index, occlusion/truncation code, height, population, and eligible/predicted/correct flags. It contains no ground-truth IDs. A wrong incoming association has predicted=1 and correct=0. Eligible denotes presence of the same hidden ID in the previous frame and is computed after assignment.

## KITTI summaries

`kitti_aggregate.csv`, `kitti_per_sequence.csv`, `kitti_sensitivity.csv` sum counts then divide: precision=correct/predicted, recall=correct/eligible. The recall denominator includes true continuations excluded by gates. NaN denotes a zero denominator. `objective_normalized` divides the integer objective by one million, including unmatched penalties. Compare objectives only within one metric/configuration. Timing takes each pair's median, then the median/IQR across 6,489 active pairs.

`kitti_paired_comparison.csv` retains both methods' costs and correct counts for each identical problem. `objective_gap=Greedy-Hungarian`; `correct_gain=Hungarian-Greedy`. `kitti_counterexamples.csv` retains every baseline pair with positive objective gap and negative correct gain. These examples are selected transparently after evaluation for illustration, not used to set parameters.

`kitti_subgroups.csv` sums object flags by current occlusion, truncation, height (<40 or >=40 pixels), and maximum pair population (1–4, 5–8, 9+). These observational groups are not causal experiments. `kitti_summary.json` retains counts and main outcomes.

## Artifact mapping

| Evidence | Paper | Beamer |
|---|---|---|
| `full.csv`, `summary.csv` | Runtime table and figure; Greedy gap figure | Slide 9 |
| `full.csv` scan counts + closed forms | Complexity proof and inspection figure | Slides 6 and 8 |
| `small.csv`, oracle CSVs | Enumeration results and verification | Slide 7 |
| `rect.csv`, `rect_summary.csv` | Rectangular table | Slide 6 explanation |
| `memory.csv` | Workspace/process-memory figure | Slide 10 |
| `kitti_inventory.csv`, protocol | KITTI design | Slide 11 |
| `kitti_aggregate.csv`, `kitti_subgroups.csv` | KITTI table and error/miss-rate figure | Slide 12 |
| `kitti_per_sequence.csv` | Per-sequence recall difference figure | Speaker context |
| `kitti_sensitivity.csv`, counterexamples | Sensitivity and cost/identity distinction | Slide 12 |

`tools/analyze_results.py` generates matrix tables/figures and LaTeX macros. `tools/analyze_kitti.py` generates KITTI summaries/tables/figures. `tools/create_beamer_figures.py` generates slide-scale charts. Every statement comes from these outputs or an explicit derivation. The script prose is reviewed against those values; it is not automatically regenerated by analysis scripts.
