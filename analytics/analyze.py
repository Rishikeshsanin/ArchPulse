#!/usr/bin/env python3
from pathlib import Path
import argparse
import matplotlib.pyplot as plt
import pandas as pd

def chart(x,y,xlabel,ylabel,title,out):
    fig,ax=plt.subplots(figsize=(8,5));ax.plot(x,y,marker='o');ax.set(xlabel=xlabel,ylabel=ylabel,title=title);ax.grid(True,alpha=.25);fig.tight_layout();fig.savefig(out,dpi=160);plt.close(fig)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('directory',nargs='?',default='results',type=Path);d=ap.parse_args().directory;d.mkdir(parents=True,exist_ok=True)
    p=d/'benchmark.csv'
    if p.exists():
        df=pd.read_csv(p);chart(df.threads,df.runtime_ms,'Threads','Runtime (ms)','ArchPulse: Runtime Scaling',d/'runtime_scaling.png');chart(df.threads,df.speedup,'Threads','Speedup (x)','ArchPulse: Parallel Speedup',d/'parallel_speedup.png')
    p=d/'cache.csv'
    if p.exists():
        df=pd.read_csv(p);chart(range(1,len(df)+1),df.hit_rate,'Run','Hit Rate (%)','ArchPulse: Cache Hit Rate',d/'cache_hit_rate.png')
    p=d/'pipeline.csv'
    if p.exists():
        df=pd.read_csv(p);chart(range(1,len(df)+1),df.cpi,'Run','CPI','ArchPulse: Pipeline CPI',d/'pipeline_cpi.png')
    print(f'Analysis complete: {d.resolve()}')
if __name__=='__main__':main()
