# KITTI tracking training labels

This project uses the **KITTI Object Tracking training labels** for the vehicle-association case study.

Official source: https://www.cvlibs.net/datasets/kitti/eval_tracking.php

The exact archive used in the experiments is included at:

```text
data/kitti/data_tracking_label_2.zip
```

Archive SHA-256:

```text
6ad1fa0125e3cddcbe4d182e41cd12a854a11b0c733b26c706595f6db5c254e0
```

The extracted training labels are stored at:

```text
data/kitti/training/label_02/0000.txt
...
data/kitti/training/label_02/0020.txt
```

`results/dataset_sha256.json` records the SHA-256 hash of each label file.

## Use in this project

- Only class `Car` is used.
- Bounding-box geometry is used to build the assignment problem.
- Track IDs are not used to construct costs or choose assignments.
- IDs, occlusion, and truncation metadata are used only after assignment for evaluation and subgroup analysis.
- Missing frame indices are represented as empty frames.
- All consecutive frame pairs in the 21 training sequences are evaluated.

The final dataset inventory contains 27,300 `Car` boxes across 21 training sequences and 7,987 consecutive frame pairs.

This is an annotated-box association experiment, not detector evaluation, full multi-object tracking, or an official KITTI leaderboard submission.

The frozen experimental settings are in `docs/KITTI_PROTOCOL.json`.
