from pathlib import Path
import pandas as pd,numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import argparse
parser=argparse.ArgumentParser();parser.add_argument('--results',default='results');parser.add_argument('--output',default='docs/slide_assets');args=parser.parse_args()
root=Path(__file__).resolve().parents[1];r=root/args.results;out=root/args.output;out.mkdir(parents=True,exist_ok=True)
plt.rcParams.update({'font.family':'DejaVu Sans','font.size':12,'axes.titlesize':13,'axes.labelsize':12,'xtick.labelsize':10,'ytick.labelsize':11,'pdf.fonttype':42,'axes.spines.top':False,'axes.spines.right':False})
hcol='#11798A';gcol='#D8872C'
def save(fig,name):fig.savefig(out/(name+'.pdf'),bbox_inches='tight');fig.savefig(out/(name+'.png'),dpi=200,bbox_inches='tight');plt.close(fig)
s=pd.read_csv(r/'summary.csv')
fig,ax=plt.subplots(figsize=(7.5,3.1))
for family,color,label in [('equal','#8D3C74','All equal: cubic witness'),('uniform','#708090','Uniform'),('planted',hcol,'Planted: quadratic witness')]:
 z=s[(s.family==family)&(s.algorithm=='hungarian')];ax.loglog(z.rows,z.scans,'o-',lw=2,ms=5,color=color,label=label)
ax.set_xlabel('Matrix dimension n');ax.set_ylabel('Reduced-cost inspections');ax.legend(frameon=False,fontsize=10);ax.grid(alpha=.16);fig.tight_layout();save(fig,'structure')
fig,ax=plt.subplots(figsize=(7.5,3.1));families=['uniform','planted','near_ties','equal','adversarial'];labels=['Uniform','Planted diagonal','Near ties','All equal','Adversarial blocks']
for alg,offset,col in [('hungarian',-.18,hcol),('greedy',.18,gcol)]:
 z=s[(s.rows==1024)&(s.algorithm==alg)].set_index('family').loc[families]
 ax.barh(np.arange(5)+offset,z.median_us/1000,height=.33,color=col,label=alg.capitalize())
ax.set_yticks(range(5),labels);ax.invert_yaxis();ax.set_xscale('log');ax.set_xlim(1,1600);ax.set_xlabel('Median time per solve (ms), log scale');ax.legend(frameon=False,fontsize=10,loc='lower right');ax.grid(axis='x',alpha=.16);ax.set_axisbelow(True);fig.tight_layout();save(fig,'runtime_comparison')
fig,ax=plt.subplots(figsize=(5.6,3.1));mem=pd.read_csv(r/'memory.csv');z=mem[mem.n==2048].set_index('algorithm')
ax.bar(['Hungarian','Greedy'],[z.loc[a,'workspace_bytes']/1024 for a in ['hungarian','greedy']],color=[hcol,gcol],width=.5)
ax.set_yscale('log');ax.set_ylim(10,300000);ax.set_ylabel('Auxiliary storage (KiB)')
for i,a in enumerate(['hungarian','greedy']):v=z.loc[a,'workspace_bytes']/1024;ax.text(i,v*1.2,f'{v:,.1f}',ha='center',fontsize=12)
ax.grid(axis='y',alpha=.16);ax.set_axisbelow(True);fig.tight_layout();save(fig,'workspace')
k=pd.read_csv(r/'kitti_aggregate.csv');fig,ax=plt.subplots(figsize=(5.5,3.2))
for alg,offset,col in [('hungarian',-.18,hcol),('greedy',.18,gcol)]:
 z=k[k.method==alg].set_index('metric').loc[['distance','iou']]
 bars=ax.bar(np.arange(2)+offset,z.incorrect,width=.33,color=col,label=alg.capitalize())
 ax.bar_label(bars,padding=4,fontsize=12)
ax.set_xticks([0,1],['Center distance','IoU']);ax.set_ylim(0,560);ax.set_ylabel('Incorrect accepted links');ax.legend(frameon=False,fontsize=10);ax.grid(axis='y',alpha=.16);ax.set_axisbelow(True);fig.tight_layout();save(fig,'kitti_errors')
