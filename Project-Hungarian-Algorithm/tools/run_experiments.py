#!/usr/bin/env python3
"""Run C++ measurements serially; Python does not solve assignment instances."""
from pathlib import Path
import argparse, datetime, json, os, platform, subprocess

ROOT = Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--build',default='build')
parser.add_argument('--kitti-labels',required=True,help='Directory of local KITTI training label .txt files')
parser.add_argument('--out',default='results/reproduction')
parser.add_argument('--compiler',default='g++',help='Compiler used to build the executables')
parser.add_argument('--build-flags',default='Unrecorded: supply the actual build flags with --build-flags',help='Actual flags used for this build; do not infer them from the executable')
args=parser.parse_args()
build=(ROOT/args.build).resolve(); out=(ROOT/args.out).resolve();out.mkdir(parents=True,exist_ok=True)
affinity=sorted(os.sched_getaffinity(0)) if hasattr(os,'sched_getaffinity') else []
selected=affinity[0] if affinity else None
if selected is not None:os.sched_setaffinity(0,{selected})
metadata={'timestamp_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
 'system':platform.platform(),'compiler':subprocess.check_output([args.compiler,'--version'],text=True),
 'flags':args.build_flags,
 'cpu':subprocess.check_output(['lscpu'],text=True),'original_cpu_affinity':affinity,'pinned_cpu':selected,
 'kitti_timing':'Baseline only; steady_clock; pilot; target 200 us, cap 64; three batches per pair; median active-pair time; includes geometry cost construction and allocation, excludes annotation parsing/evaluation and certificates',
 'timing':'steady_clock; warmup and pilot; batches target 2 ms, cap 256; three repeats; allocation and input validation included; generation, result validation, counters and CSV excluded'}
(out/'environment.json').write_text(json.dumps(metadata,indent=2))
with (out/'execution.log').open('w') as log:
 for mode in ('full','small','rect'):
  cmd=[str(build/'lap_benchmark'),mode,str(out/(mode+'.csv'))]
  print('Running',mode,flush=True);log.write(' '.join(cmd)+'\n');log.flush()
  subprocess.run(cmd,stdout=log,stderr=log,check=True,cwd=ROOT)
 with (out/'memory.csv').open('w') as mem:
  mem.write('algorithm,n,input_bytes,workspace_bytes,peak_rss_kib,cost\n')
  mem.flush()
  for n in (128,256,512,1024,2048):
   for alg in ('hungarian','greedy'):
    line=subprocess.check_output([str(build/'lap_benchmark'),'memory',alg,str(n),'uniform'],stderr=log,text=True,cwd=ROOT)
    if len(line.strip().split(','))!=6: raise RuntimeError('Missing memory record')
    mem.write(line);mem.flush()
 print('Running KITTI',flush=True)
 subprocess.run([str(build/'lap_tracking_experiment'),str(Path(args.kitti_labels).resolve()),str(out)],stdout=log,stderr=log,check=True,cwd=ROOT)
import hashlib
sources={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for sub in ('src','include','tests','tools') for p in sorted((ROOT/sub).glob('*')) if p.is_file()}
(out/'source_sha256.json').write_text(json.dumps(sources,indent=2))
labels=Path(args.kitti_labels).resolve()
(out/'dataset_sha256.json').write_text(json.dumps({p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(labels.glob('*.txt'))},indent=2))
print('All C++ experiments completed:',out,flush=True)
