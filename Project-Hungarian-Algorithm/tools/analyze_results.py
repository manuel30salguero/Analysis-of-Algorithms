#!/usr/bin/env python3
"""Aggregate measured C++ records and create report figures and tables."""
from pathlib import Path
import argparse,json
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser();parser.add_argument('--results',default='results');parser.add_argument('--paper',default='docs/report_assets')
args=parser.parse_args();result=(root/args.results).resolve();paper=(root/args.paper).resolve()
figdir=paper/'figures';tabdir=paper/'tables';figdir.mkdir(parents=True,exist_ok=True);tabdir.mkdir(parents=True,exist_ok=True)
plt.rcParams.update({'font.family':'DejaVu Serif','font.size':8,'axes.titlesize':9,'axes.labelsize':8,'legend.fontsize':7,'xtick.labelsize':7,'ytick.labelsize':7,'pdf.fonttype':42,'ps.fonttype':42,'axes.spines.top':False,'axes.spines.right':False})
families=['uniform','planted','near_ties','equal','adversarial']
names={'uniform':'Uniform','planted':'Planted diagonal','near_ties':'Near ties','equal':'All equal','adversarial':'Adversarial blocks'}
colors=dict(zip(families,['#176b91','#178064','#d38a16','#9a477c','#9a472c']))
def save(fig,name):
 fig.savefig(figdir/(name+'.pdf'),bbox_inches='tight')
 fig.savefig(figdir/(name+'.png'),dpi=240,bbox_inches='tight');plt.close(fig)
def summarize(path):
 d=pd.read_csv(path)
 keys=['family','rows','cols','seed','algorithm']
 s=d.groupby(keys,as_index=False).agg(time_us=('time_us','median'),cost=('cost','first'),optimum=('optimum','first'),scans=('scans','first'),expansions=('expansions','first'))
 s['gap_pct']=100*(s.cost-s.optimum)/s.optimum
 return d,s
raw,s=summarize(result/'full.csv');s.to_csv(result/'per_instance_summary.csv',index=False)
agg=s.groupby(['family','rows','algorithm'],as_index=False).agg(median_us=('time_us','median'),q1_us=('time_us',lambda x:x.quantile(.25)),q3_us=('time_us',lambda x:x.quantile(.75)),gap_pct=('gap_pct','median'),scans=('scans','median'))
# For deterministic matrices, timing spread is across repetitions of one matrix.
for f in ['equal','adversarial']:
 for n in raw.rows.unique():
  for alg in ['hungarian','greedy']:
   x=raw[(raw.family==f)&(raw.rows==n)&(raw.algorithm==alg)].time_us
   sel=(agg.family==f)&(agg.rows==n)&(agg.algorithm==alg)
   agg.loc[sel,'q1_us']=x.quantile(.25);agg.loc[sel,'q3_us']=x.quantile(.75)
agg.to_csv(result/'summary.csv',index=False)

fig,axs=plt.subplots(1,2,figsize=(7.0,2.65),sharey=True)
for ax,alg,title in zip(axs,['hungarian','greedy'],['Hungarian','Global edge-sorting Greedy']):
 for f in families:
  z=agg[(agg.family==f)&(agg.algorithm==alg)]
  ax.loglog(z.rows,z.median_us/1000,'o-',ms=3,lw=1.25,color=colors[f],label=names[f])
  ax.fill_between(z.rows,z.q1_us/1000,z.q3_us/1000,color=colors[f],alpha=.13)
 ax.set_title(title);ax.set_xlabel('Matrix dimension n');ax.grid(alpha=.18,which='major')
axs[0].set_ylabel('Median time per solve (ms)');axs[1].legend(loc='upper left',frameon=False)
fig.tight_layout();save(fig,'runtime')

fig,ax=plt.subplots(figsize=(3.4,2.6))
for f in families:
 z=agg[(agg.family==f)&(agg.algorithm=='hungarian')]
 ax.loglog(z.rows,z.scans,'o-',ms=3,lw=1.2,color=colors[f],label=names[f])
ax.set_xlabel('Matrix dimension n');ax.set_ylabel('Reduced-cost inspections');ax.grid(alpha=.18)
ax.legend(frameon=False,loc='upper left');fig.tight_layout();save(fig,'operations')

fig,ax=plt.subplots(figsize=(3.4,2.5))
for f in ['uniform','near_ties','adversarial']:
 z=agg[(agg.family==f)&(agg.algorithm=='greedy')]
 ax.plot(z.rows,z.gap_pct,'o-',ms=3,lw=1.2,color=colors[f],label=names[f])
ax.set_xscale('log',base=2);ax.set_yscale('symlog',linthresh=.01)
ax.set_ylim(0,60000)
ax.set_xlabel('Matrix dimension n');ax.set_ylabel('Median Greedy excess cost (%)');ax.grid(alpha=.18)
ax.legend(frameon=False);fig.tight_layout();save(fig,'gaps')

_,small=summarize(result/'small.csv');smallagg=small.groupby(['rows','algorithm'],as_index=False).time_us.median()
smallagg.to_csv(result/'small_summary.csv',index=False)
fig,ax=plt.subplots(figsize=(3.4,2.5))
for alg,color in [('hungarian','#176b91'),('greedy','#d38a16'),('exhaustive','#9a477c')]:
 z=smallagg[smallagg.algorithm==alg];ax.semilogy(z.rows,z.time_us,'o-',ms=3,color=color,label=alg.capitalize())
ax.set_xlabel('Matrix dimension n');ax.set_ylabel('Median time per solve (microseconds)');ax.grid(alpha=.18);ax.legend(frameon=False)
fig.tight_layout();save(fig,'exhaustive')

mem=pd.read_csv(result/'memory.csv')
assert len(mem)==10 and mem.groupby('algorithm').size().eq(5).all()
fig,axs=plt.subplots(1,2,figsize=(7.0,2.55))
for alg,col in [('hungarian','#176b91'),('greedy','#d38a16')]:
 z=mem[mem.algorithm==alg]
 axs[0].loglog(z.n,z.workspace_bytes/1024,'o-',ms=3,color=col,label=alg.capitalize())
 axs[1].plot(z.n,z.peak_rss_kib/1024,'o-',ms=3,color=col,label=alg.capitalize())
axs[0].set_ylabel('Owned vector capacities (KiB)');axs[0].set_title('Workspace excluding the input')
axs[1].set_ylabel('Process peak resident size (MiB)');axs[1].set_title('Observed process high-water mark')
for ax in axs:ax.set_xlabel('Matrix dimension n');ax.grid(alpha=.18);ax.legend(frameon=False)
fig.tight_layout();save(fig,'memory')

_,rect=summarize(result/'rect.csv');rectagg=rect.groupby(['rows','cols','algorithm'],as_index=False).time_us.median();rectagg.to_csv(result/'rect_summary.csv',index=False)
# A fitted slope describes these finite measurements, not asymptotic complexity.
fits=[]
for f in families:
 for alg in ['hungarian','greedy']:
  z=agg[(agg.family==f)&(agg.algorithm==alg)&(agg.rows>=64)]
  b,a=np.polyfit(np.log(z.rows),np.log(z.median_us),1)
  pred=a+b*np.log(z.rows);y=np.log(z.median_us)
  r2=1-float(((y-pred)**2).sum()/((y-y.mean())**2).sum())
  fits.append({'family':f,'algorithm':alg,'slope':b,'r_squared':r2,'n_min':64,'n_max':1024})
fits=pd.DataFrame(fits);fits.to_csv(result/'scaling_fits.csv',index=False)

def tex_table(file,header,rows,cols):
 text='\\begin{tabular}{'+cols+'}\n\\toprule\n'+header+' \\\\\n\\midrule\n'
 text+=''.join(' & '.join(map(str,row))+' \\\\\n' for row in rows)
 text+='\\bottomrule\n\\end{tabular}\n';(tabdir/file).write_text(text)
def cell(f,alg,n=1024):return agg[(agg.family==f)&(agg.algorithm==alg)&(agg.rows==n)].iloc[0]
rows=[]
for f in families:
 h,g=cell(f,'hungarian'),cell(f,'greedy')
 rows.append([names[f],f'{h.median_us/1000:.3f}',f'{g.median_us/1000:.3f}',f'{g.gap_pct:.3f}'])
tex_table('runtime.tex','Family & Hungarian (ms) & Greedy (ms) & Gap (\\%)',rows,'lrrr')
rows=[]
for f in families:
 h,g=[fits[(fits.family==f)&(fits.algorithm==a)].iloc[0] for a in ['hungarian','greedy']]
 rows.append([names[f],f'{h.slope:.2f}',f'{g.slope:.2f}',f'{h.r_squared:.3f}'])
tex_table('scaling.tex','Family & H slope & G slope & H $R^2$',rows,'lrrr')
rows=[]
for (r,c),z in rectagg.groupby(['rows','cols']):
 rows.append([f'${r}\\times {c}$',f'{z[z.algorithm=="hungarian"].time_us.iloc[0]/1000:.3f}',f'{z[z.algorithm=="greedy"].time_us.iloc[0]/1000:.3f}'])
tex_table('rectangular.tex','Shape & Hungarian (ms) & Greedy (ms)',rows,'lrr')
macros={
'UniformHungarianMs':f'{cell("uniform","hungarian").median_us/1000:.3f}',
'UniformGreedyMs':f'{cell("uniform","greedy").median_us/1000:.3f}',
'UniformGap':f'{cell("uniform","greedy").gap_pct:.2f}',
'PlantedHungarianMs':f'{cell("planted","hungarian").median_us/1000:.3f}',
'PlantedGreedyMs':f'{cell("planted","greedy").median_us/1000:.3f}',
'EqualHungarianMs':f'{cell("equal","hungarian").median_us/1000:.3f}',
'TiesHungarianMs':f'{cell("near_ties","hungarian").median_us/1000:.3f}',
'GreedyAdversarialGap':f'{cell("adversarial","greedy").gap_pct:,.0f}',
'UniformSlope':f'{fits[(fits.family=="uniform")&(fits.algorithm=="hungarian")].slope.iloc[0]:.2f}',
'EqualSlope':f'{fits[(fits.family=="equal")&(fits.algorithm=="hungarian")].slope.iloc[0]:.2f}',
'PlantedSlope':f'{fits[(fits.family=="planted")&(fits.algorithm=="hungarian")].slope.iloc[0]:.2f}',
'BruteTenUs':f'{smallagg[(smallagg.rows==10)&(smallagg.algorithm=="exhaustive")].time_us.iloc[0]:,.0f}',
'HungarianTenUs':f'{smallagg[(smallagg.rows==10)&(smallagg.algorithm=="hungarian")].time_us.iloc[0]:.2f}' }
(paper/'results_macros.tex').write_text('% Generated from measured C++ CSV files.\n'+''.join('\\newcommand{\\'+k+'}{'+v+'}\n' for k,v in macros.items()))
summary={'macros':macros,'runtime':agg.to_dict(orient='records'),'memory':mem.to_dict(orient='records'),'fits':fits.to_dict(orient='records'),'record_counts':{'full':len(raw),'small':len(pd.read_csv(result/'small.csv')),'rect':len(pd.read_csv(result/'rect.csv'))}}
(result/'summary.json').write_text(json.dumps(summary,indent=2))
# Cross-check the known closed-form cost and operation counts against raw records.
eq=raw[(raw.family=='equal')&(raw.algorithm=='hungarian')]
assert (eq.scans==eq.rows*(eq.rows+1)*(2*eq.rows+1)//6).all()
assert (raw[raw.algorithm=='hungarian'].absolute_gap==0).all()
assert (raw.absolute_gap>=0).all()

print(json.dumps({'macros':macros,'records':summary['record_counts']},indent=2))
