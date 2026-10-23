#!/usr/bin/env python3
"""Aggregate C++ KITTI outputs. No assignment computation in Python."""
from pathlib import Path
import argparse,json
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
p=argparse.ArgumentParser();p.add_argument('--results',default='results');p.add_argument('--paper',default='docs/report_assets');args=p.parse_args()
root=Path(__file__).resolve().parents[1];r=root/args.results;paper=root/args.paper
figs=paper/'figures';tabs=paper/'tables';figs.mkdir(parents=True,exist_ok=True);tabs.mkdir(parents=True,exist_ok=True)
k=pd.read_csv(r/'kitti_pairs.csv',dtype={'sequence':str,'fingerprint':str});b=k[k.config=='baseline'].copy()
t=pd.read_csv(r/'kitti_timing.csv',dtype={'sequence':str});o=pd.read_csv(r/'kitti_objects.csv',dtype={'sequence':str});inv=pd.read_csv(r/'kitti_inventory.csv',dtype={'sequence':str})
assert (k.correct<=k.eligible).all() and (k.correct<=k.predicted).all() and (k.incorrect==k.predicted-k.correct).all() and (k.certificate_ok==1).all()
keys=['sequence','frame','metric','config'];paired=k.pivot(index=keys,columns='method',values=['objective','correct','predicted','fingerprint'])
assert (paired[('fingerprint','hungarian')]==paired[('fingerprint','greedy')]).all()
assert (paired[('objective','hungarian')].astype(int)<=paired[('objective','greedy')].astype(int)).all()
pair_summary=pd.DataFrame(index=paired.index)
for variable in ['objective','correct','predicted']:
 pair_summary[variable+'_h']=paired[(variable,'hungarian')].astype(int);pair_summary[variable+'_g']=paired[(variable,'greedy')].astype(int)
pair_summary['objective_gap']=pair_summary.objective_g-pair_summary.objective_h;pair_summary['correct_gain']=pair_summary.correct_h-pair_summary.correct_g
pair_summary.reset_index().to_csv(r/'kitti_paired_comparison.csv',index=False)

def aggregate(d,keys):
 out=d.groupby(keys,as_index=False).agg(pairs=('frame','size'),active_pairs=('previous',lambda x:0),eligible=('eligible','sum'),gate_eligible=('gate_eligible','sum'),predicted=('predicted','sum'),correct=('correct','sum'),incorrect=('incorrect','sum'),objective=('objective','sum'))
 active=d.assign(active=(d.previous+d.current)>0).groupby(keys).active.sum().reset_index(name='active_pairs')
 out=out.drop(columns='active_pairs').merge(active,on=keys)
 out['precision_pct']=100*out.correct/out.predicted.replace(0,np.nan);out['recall_pct']=100*out.correct/out.eligible.replace(0,np.nan)
 out['objective_normalized']=out.objective/1e6
 return out
agg=aggregate(b,['metric','method']);seq=aggregate(b,['sequence','metric','method']);sens=aggregate(k,['metric','config','method'])
tm=t.groupby(['sequence','frame','metric','method'],as_index=False).agg(time_us=('time_us','median'),previous=('previous','first'),current=('current','first'))
active=tm[tm.previous+tm.current>0]
ta=active.groupby(['metric','method'],as_index=False).agg(median_us=('time_us','median'),q1_us=('time_us',lambda x:x.quantile(.25)),q3_us=('time_us',lambda x:x.quantile(.75)))
agg=agg.merge(ta,on=['metric','method']);seq=seq.merge(active.groupby(['sequence','metric','method'],as_index=False).time_us.median().rename(columns={'time_us':'median_us'}),how='left')
agg.to_csv(r/'kitti_aggregate.csv',index=False);seq.to_csv(r/'kitti_per_sequence.csv',index=False);sens.to_csv(r/'kitti_sensitivity.csv',index=False);tm.to_csv(r/'kitti_time_per_pair.csv',index=False)
# Correct continuations stratified by properties of the current box. Every predicted incoming link belongs to exactly one current box.
o['size_group']=np.where(o.height<40,'height <40 px','height >=40 px');o['crowding_group']=pd.cut(o.crowding,bins=[0,4,8,np.inf],labels=['1-4 cars','5-8 cars','9+ cars'])
sub=[]
for col in ['occlusion','truncation','size_group','crowding_group']:
 z=o.groupby([col,'metric','method'],observed=True,as_index=False)[['eligible','predicted','correct']].sum().rename(columns={col:'group'});z['property']=col
 z['precision_pct']=100*z.correct/z.predicted.replace(0,np.nan);z['recall_pct']=100*z.correct/z.eligible.replace(0,np.nan);sub.append(z)
sub=pd.concat(sub,ignore_index=True);sub.to_csv(r/'kitti_subgroups.csv',index=False)
# Verify object-level evaluation aggregates exactly reproduce pair records.
assert o.groupby(['metric','method'])[['eligible','predicted','correct']].sum().equals(b.groupby(['metric','method'])[['eligible','predicted','correct']].sum())
win=[]
for metric,z in pair_summary.reset_index().query("config == 'baseline'").groupby('metric'):
 win.append({'metric':metric,'pairs':len(z),'strict_objective_win':int((z.objective_gap>0).sum()),'equal_objective':int((z.objective_gap==0).sum()),'h_more_correct':int((z.correct_gain>0).sum()),'equal_correct':int((z.correct_gain==0).sum()),'g_more_correct':int((z.correct_gain<0).sum()),'lower_cost_fewer_correct':int(((z.objective_gap>0)&(z.correct_gain<0)).sum()),'max_objective_gap':int(z.objective_gap.max())})
win=pd.DataFrame(win);win.to_csv(r/'kitti_comparison_summary.csv',index=False)
examples=pair_summary.reset_index().query("config == 'baseline' and objective_gap > 0 and correct_gain < 0").sort_values(['metric','sequence','frame']);examples.to_csv(r/'kitti_counterexamples.csv',index=False)
plt.rcParams.update({'font.family':'DejaVu Serif','font.size':8,'axes.titlesize':9,'axes.labelsize':8,'legend.fontsize':7,'xtick.labelsize':7,'ytick.labelsize':7,'pdf.fonttype':42,'axes.spines.top':False,'axes.spines.right':False})
colors={'hungarian':'#176b91','greedy':'#d38a16'}
def save(fig,name):
 fig.savefig(figs/(name+'.pdf'),bbox_inches='tight');fig.savefig(figs/(name+'.png'),dpi=240,bbox_inches='tight');plt.close(fig)
fig,axs=plt.subplots(1,2,figsize=(7.0,2.55))
for i,metric in enumerate(['distance','iou']):
 z=agg[agg.metric==metric].set_index('method')
 for j,method in enumerate(['hungarian','greedy']):axs[0].bar(i+(j-.5)*.34,z.loc[method,'incorrect'],width=.32,color=colors[method],label=method.capitalize() if i==0 else None)
axs[0].set_xticks([0,1],['Center distance','IoU']);axs[0].set_ylabel('Incorrect real associations');axs[0].legend(frameon=False);axs[0].set_ylim(bottom=0)
for metric,marker in [('distance','o'),('iou','s')]:
 for method in ['hungarian','greedy']:
  z=sub[(sub.property=='occlusion')&(sub.metric==metric)&(sub.method==method)].copy();z['group']=z['group'].astype(int);z=z.sort_values('group')
  axs[1].plot(z['group'],100-z.recall_pct,marker,color=colors[method],ls='-' if metric=='distance' else '--',label=('H' if method=='hungarian' else 'G')+' / '+metric)
axs[1].set_xticks([0,1,2,3],['0','1','2','3']);axs[1].set_ylim(0,6);axs[1].set_xlabel('Current annotation occlusion code');axs[1].set_ylabel('Continuation miss rate (%)');axs[1].legend(frameon=False,ncol=2,loc='upper left')
for ax in axs:ax.grid(axis='y',alpha=.2);ax.set_axisbelow(True)
fig.tight_layout();save(fig,'kitti_association')
fig,ax=plt.subplots(figsize=(7,2.3));x=np.arange(len(inv));
for metric,off,col in [('distance',-.18,'#176b91'),('iou',.18,'#9a477c')]:
 z=seq[seq.metric==metric].pivot(index='sequence',columns='method',values='recall_pct').reindex(inv.sequence)
 ax.bar(x+off,z.hungarian-z.greedy,width=.34,label=metric.capitalize(),color=col)
ax.axhline(0,color='#333333',lw=.7);ax.set_xticks(x,inv.sequence,rotation=45);ax.set_ylabel('Recall difference H - G (pp)');ax.set_xlabel('KITTI training sequence (0017 has no Car boxes)');ax.legend(frameon=False);ax.grid(axis='y',alpha=.2);fig.tight_layout();save(fig,'kitti_sequences')
# Sensitivity plot: all prespecified configurations, no best-case selection.
fig,axs=plt.subplots(1,2,figsize=(7,2.55),sharey=True)
order=['gate_low','baseline','gate_high','penalty_low','penalty_high']
for ax,metric in zip(axs,['distance','iou']):
 for method in ['hungarian','greedy']:
  z=sens[(sens.metric==metric)&(sens.method==method)].set_index('config').loc[order]
  ax.plot(range(5),z.recall_pct,'o-',color=colors[method],label=method.capitalize())
 ax.set_xticks(range(5),['gate low','base','gate high','pen. 0.4','pen. 0.8'],rotation=20,ha='right');ax.set_title(metric.capitalize());ax.set_ylim(0,105);ax.grid(alpha=.2)
axs[0].set_ylabel('Continuation recall (%)');axs[1].legend(frameon=False);fig.tight_layout();save(fig,'kitti_sensitivity')

def tex(file,header,rows,cols):
 (tabs/file).write_text('\\begin{tabular}{'+cols+'}\n\\toprule\n'+header+' \\\\\n\\midrule\n'+''.join(' & '.join(row)+' \\\\\n' for row in rows)+'\\bottomrule\n\\end{tabular}\n')
rows=[]
for row in agg.itertuples():rows.append(['Distance' if row.metric=='distance' else 'IoU','H' if row.method=='hungarian' else 'G',f'{row.predicted:,}',f'{row.correct:,}',f'{row.incorrect:,}',f'{row.precision_pct:.2f}',f'{row.recall_pct:.2f}',f'{row.objective_normalized:.2f}',f'{row.median_us:.2f}'])
tex('kitti.tex','Cost & Method & Predicted & Correct & Incorrect & Prec. (\\%) & Recall (\\%) & Objective/$S$ & Time ($\\mu$s)',rows,'llrrrrrrr')
summary={'inventory':inv.to_dict('records'),'totals':{c:int(inv[c].sum()) for c in ['frames','pairs','car_boxes','car_frames','active_pairs']},'max_cars':int(inv.max_cars.max()),'max_augmented_order':int(inv.max_augmented_order.max()),'aggregate':agg.to_dict('records'),'paired':win.to_dict('records'),'sensitivity':sens.to_dict('records'),'first_counterexamples':examples.groupby('metric').head(1).to_dict('records'),'raw_counts':{'pairs':len(k),'timing':len(t),'objects':len(o)},'total_certificates':len(k)//2}
(r/'kitti_summary.json').write_text(json.dumps(summary,indent=2));print(json.dumps({k:v for k,v in summary.items() if k not in ['inventory','sensitivity']},indent=2))
