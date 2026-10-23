# KITTI tracking training labels

Acquire the official **Object Tracking** training label archive through [KITTI](https://www.cvlibs.net/datasets/kitti/eval_tracking.php), following its account and download terms. This release used the user-supplied archive `data_tracking_label_2.zip`. No test set, image archive or leaderboard submission was used.

Archive SHA-256:

```text
6ad1fa0125e3cddcbe4d182e41cd12a854a11b0c733b26c706595f6db5c254e0
```

Place the extracted 21 files at `data/kitti/training/label_02/0000.txt` through `0020.txt`, or pass another local label-directory path. `results/dataset_sha256.json` records each file hash. Official annotations and the archive are excluded from all delivered packages and ignored by Git.

Each observed row has 17 whitespace-separated fields: frame, track ID, type, truncation code, occlusion code, alpha, left, top, right, bottom, height, width, length, x, y, z, rotation. The final seven 3D fields and alpha are parsed but never used in association. The training parser rejects extra fields, including prediction scores. Blank lines are skipped.

The principal filter is exactly `Car`. All other types are excluded. Selected IDs must be nonnegative and unique within a frame; box coordinates must imply finite centers and positive finite dimensions. Truncation codes 0–2 and occlusion codes 0–3 are retained, never used in solving. Duplicate geometry is rejected rather than resolved by an ID-dependent order. None of the supplied Car rows is rejected.

Frames run from zero through each file's maximum observed frame index across all classes; missing indices become empty frames. All consecutive pairs are evaluated. Sequence 0017 has no Car boxes and remains in the inventory with undefined precision/recall. Annotated boxes provide geometry; IDs only score completed assignments. This is not independent detector evaluation or official KITTI tracking evaluation.

`docs/KITTI_PROTOCOL.json` contains the frozen baseline and sensitivity settings. `results/kitti_inventory.csv` gives sequence counts. Full results are derived summaries and evaluation records; they do not redistribute the official annotation files.

# KITTI Tracking Training Labels

This repository includes the KITTI tracking training labels used in the vehicle-association case study.

Included archive:

- `data/kitti/data_tracking_label_2.zip`
- SHA-256: `6ad1fa0125e3cddcbe4d182e41cd12a854a11b0c733b26c706595f6db5c254e0`

Extracted labels used by the experiments:

- `data/kitti/training/label_02/0000.txt`
- ...
- `data/kitti/training/label_02/0020.txt`

The project filters the annotations to class `Car` for the reported KITTI association experiments.

The original dataset source is the KITTI Vision Benchmark Suite. The labels are included here solely to reproduce the academic experiments reported in this course project.
