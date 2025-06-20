#!/usr/bin/env python3
import argparse
import urllib.request
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def download_text(url):
    """
    Download raw text from a URL (or file path) and return as a string.
    """
    with urllib.request.urlopen(url) as resp:
        return resp.read().decode('utf-8', 'replace')

def parse_manual(text):
    """
    Parse the toranj raw dump, skipping metadata and repeated headers,
    returning a DataFrame with columns:
      - Binary, Threads, ParamValue, AverageTime, Stat2
    """
    rows = []
    for line in text.splitlines():
        if not line or line.startswith("Thread Sequence:") or line.startswith("param values:") or line.startswith("Binary"):
            continue
        parts = [p.strip() for p in line.split(',')]
        if len(parts) != 5:
            continue
        rows.append(parts)
    df = pd.DataFrame(rows, columns=["Binary","Threads","ParamValue","AverageTime","Stat2"])
    df["Threads"]     = df["Threads"].astype(int)
    df["ParamValue"]  = df["ParamValue"].astype(int)
    df["AverageTime"] = df["AverageTime"].astype(float)
    df["Stat2"]       = df["Stat2"].astype(float)
    return df


def make_grouped_plots(pov, pnv, chapel, cpp, base, prefix):
    """
    Generate per-thread and per-element line plots for chapel vs cpp.
    """
    base_dir = f"{prefix}_{base}"
    # create folders
    for mode in ['old','new']:
        if mode == 'old' or pnv is not None:
            os.makedirs(f"{base_dir}/{mode}/threads", exist_ok=True)
            os.makedirs(f"{base_dir}/{mode}/elements", exist_ok=True)

    # per-thread
    for mode, piv in [('old', pov), ('new', pnv)]:
        if piv is None:
            continue
        for t in sorted(piv.index.get_level_values('Threads').unique()):
            df_t = piv.xs(t, level='Threads')  # index=ParamValue
            labels = [str(p) for p in df_t.index]
            series = {chapel: df_t[chapel].tolist(), cpp: df_t[cpp].tolist()}
            out = f"{base_dir}/{mode}/threads/T{t}.png"
            make_line(labels, series,
                      f"{base.upper()} {mode.upper()} - Threads={t}",
                      "AverageTime (s)", out, actual=True)

    # per-element
    for mode, piv in [('old', pov), ('new', pnv)]:
        if piv is None:
            continue
        for p in sorted(piv.index.get_level_values('ParamValue').unique()):
            df_p = piv.xs(p, level='ParamValue')  # index=Threads
            labels = [str(t) for t in df_p.index]
            series = {chapel: df_p[chapel].tolist(), cpp: df_p[cpp].tolist()}
            out = f"{base_dir}/{mode}/elements/P{p}.png"
            make_line(labels, series,
                      f"{base.upper()} {mode.upper()} - Param={p}",
                      "AverageTime (s)", out, actual=True)


def make_bar(labels, values, title, ylabel, outname):
    plt.figure(figsize=(12,5))
    plt.axhline(1.0 if 'Speedup' in ylabel else 0, color="black", linewidth=0.8)
    plt.bar(labels, values)
    plt.xticks(rotation=90, fontsize=6)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.tight_layout()
    plt.savefig(outname)
    print(f"Saved '{title}' chart as {outname}")

def make_grouped_bar(labels, series_dict, title, ylabel, outname):
    plt.figure(figsize=(12,5))
    x = np.arange(len(labels))
    n = len(series_dict)
    width = 0.8 / n
    for i,(name,vals) in enumerate(series_dict.items()):
        plt.bar(x + i*width, vals, width, label=name)
    plt.xticks(x + width*(n-1)/2, labels, rotation=90, fontsize=6)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.tight_layout()
    plt.savefig(outname)
    print(f"Saved '{title}' grouped bar chart as {outname}")

def make_line(labels, series_dict, title, ylabel, outname, actual=False):
    """
    Plot multiple series in series_dict ({label: values}) as a line chart.
    Use thinner lines and smaller markers for clarity.
    """
    dpi = 200 if actual else None
    plt.figure(figsize=(12,5), dpi=dpi)
    x = range(len(labels))
    for name, vals in series_dict.items():
        plt.plot(x, vals,
                 marker='o', markersize=4,
                 linewidth=1,
                 label=name)
    plt.xticks(x, labels, rotation=90, fontsize=6)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.tight_layout()
    plt.savefig(outname)
    print(f"Saved '{title}' line chart as {outname}")

if __name__=="__main__":
    p = argparse.ArgumentParser(description="Compare toranj results or variants")
    p.add_argument("old_url")
    p.add_argument("new_url", nargs='?')
    p.add_argument("-o","--output-prefix", default="comparison")
    p.add_argument("--ignore-chapel", action='store_true')
    p.add_argument("--pattern")
    p.add_argument("--compare-variant", metavar='BASE')
    p.add_argument("--raw-line", action='store_true')
    p.add_argument("--grouped-plots", action='store_true',
                   help="Generate per-thread and per-element grouped line plots")
    args = p.parse_args()

    txt_old = download_text(args.old_url)
    df_old = parse_manual(txt_old)
    if args.ignore_chapel:
        df_old = df_old[~df_old['Binary'].str.contains('chapel',case=False)]
    if args.pattern:
        df_old = df_old[df_old['Binary'].str.contains(args.pattern,regex=True)]

    def prep_variant(df, base, label):
        chapel = f"{base}_chapel"
        cpp_pat = f"cpp.*{base}$"
        dfv = df[(df['Binary']==chapel) | df['Binary'].str.contains(cpp_pat,regex=True)]
        if chapel not in dfv['Binary'].values:
            raise SystemExit(f"Missing {chapel} in {label}")
        cpps = [b for b in dfv['Binary'].unique() if b!=chapel]
        if not cpps:
            raise SystemExit(f"Missing C++ variant for '{base}' in {label}")
        cpp = cpps[0]
        pivot = dfv.pivot(index=['Threads','ParamValue'], columns='Binary', values='AverageTime')
        return pivot, chapel, cpp

    if args.grouped_plots:
        txt_new = download_text(args.new_url)
        base = args.compare_variant
        # OLD variant
        pov, _, cpp = prep_variant(df_old, base, 'old')
        chapel = f"{base}_chapel"
        df_new = parse_manual(txt_new)
        pnv, _, _ = prep_variant(df_new, base, 'new')
        make_grouped_plots(pov.sort_index(), pnv.sort_index(), chapel, cpp, base, args.output_prefix)
        exit(0)
        
    # variant mode
    if args.compare_variant:
        base = args.compare_variant
        # OLD variant
        pov, chapel, cpp = prep_variant(df_old, base, 'old')
        def mark_best(r):
            b = min(r[chapel], r[cpp])
            return pd.Series({
                chapel: f"{r[chapel]:.6f}{'*' if r[chapel]==b else ''}",
                cpp:     f"{r[cpp]:.6f}{'*' if r[cpp]==b else ''}" 
            })
        table_old = pov.apply(mark_best, axis=1)
        print(f"\n=== Variant OLD: {chapel} vs {cpp} ===")
        print(table_old.to_string())
        labels = [f"T{t}/P{p}" for t,p in pov.index]
        # normalized grouped bar
        norm_old = pov.div(pov.max(axis=1), axis=0)
        make_grouped_bar(labels,
                         {chapel: norm_old[chapel].tolist(), cpp: norm_old[cpp].tolist()},
                         f"{base} OLD: normalized times", "Normalized time",
                         f"{args.output_prefix}_{base}_norm_old_bar.png")
        # raw normalized line
        if args.raw_line:
            max_all = pov[[chapel, cpp]].values.max()
            norm1 = pov[[chapel, cpp]] / max_all
            make_line(labels,
                      {chapel: norm1[chapel].tolist(), cpp: norm1[cpp].tolist()},
                      f"{base} OLD raw normalized", "Normalized time",
                      f"{args.output_prefix}_{base}_raw_old_line.png")
        # actual times line (high-res)
        make_line(labels,
                  {chapel: pov[chapel].tolist(), cpp: pov[cpp].tolist()},
                  f"{base} OLD raw times", "Time (s)",
                  f"{args.output_prefix}_{base}_actual_old_line.png", actual=True)

        # NEW variant
        if args.new_url:
            txt_new = download_text(args.new_url)
            df_new = parse_manual(txt_new)
            if args.ignore_chapel:
                df_new = df_new[~df_new['Binary'].str.contains('chapel',case=False)]
            if args.pattern:
                df_new = df_new[df_new['Binary'].str.contains(args.pattern,regex=True)]
            pnv, _, _ = prep_variant(df_new, base, 'new')
            table_new = pnv.apply(mark_best, axis=1)
            print(f"\n=== Variant NEW: {chapel} vs {cpp} ===")
            print(table_new.to_string())
            labels2 = [f"T{t}/P{p}" for t,p in pnv.index]
            norm_new = pnv.div(pnv.max(axis=1), axis=0)
            make_grouped_bar(labels2,
                             {chapel: norm_new[chapel].tolist(), cpp: norm_new[cpp].tolist()},
                             f"{base} NEW: normalized times", "Normalized time",
                             f"{args.output_prefix}_{base}_norm_new_bar.png")
            if args.raw_line:
                max_all2 = pnv[[chapel, cpp]].values.max()
                norm2 = pnv[[chapel, cpp]] / max_all2
                make_line(labels2,
                          {chapel: norm2[chapel].tolist(), cpp: norm2[cpp].tolist()},
                          f"{base} NEW raw normalized", "Normalized time",
                          f"{args.output_prefix}_{base}_raw_new_line.png")
            # actual times line
            make_line(labels2,
                      {chapel: pnv[chapel].tolist(), cpp: pnv[cpp].tolist()},
                      f"{base} NEW raw times", "Time (s)",
                      f"{args.output_prefix}_{base}_actual_new_line.png", actual=True)
        # continue to old vs new

    # old vs new mode
    if not args.compare_variant:
        if not args.new_url:
            p.error('new_url required')
        txt_new = download_text(args.new_url)
        df_new = parse_manual(txt_new)
        if args.ignore_chapel:
            df_new = df_new[~df_new['Binary'].str.contains('chapel',case=False)]
        if args.pattern:
            df_new = df_new[df_new['Binary'].str.contains(args.pattern,regex=True)]
        m = pd.merge(df_old, df_new, on=['Binary','Threads','ParamValue'], suffixes=('_old','_new'))
        labels = [f"{b}/T{t}/P{p}" for b,t,p in zip(m['Binary'], m['Threads'], m['ParamValue'])]
        # actual old vs new line
        make_line(labels,
                  {'old': m['AverageTime_old'].tolist(), 'new': m['AverageTime_new'].tolist()},
                  'Raw AverageTime old vs new', 'Time (s)',
                  f"{args.output_prefix}_actual_oldnew_line.png", actual=True)
        # raw normalized
        if args.raw_line:
            max_all = m[['AverageTime_old','AverageTime_new']].values.max()
            norm_o = m['AverageTime_old'] / max_all
            norm_n = m['AverageTime_new'] / max_all
            make_line(labels,
                      {'old_norm': norm_o.tolist(), 'new_norm': norm_n.tolist()},
                      'Raw normalized AverageTime old vs new', 'Normalized time',
                      f"{args.output_prefix}_raw_oldnew_line.png")
        # speedup factor old vs new
        m['Speedup'] = m['AverageTime_old'] / m['AverageTime_new']
        make_bar(labels, m['Speedup'], 'Speedup old vs new', 'Speedup (old/new)',
                 f"{args.output_prefix}_speedup_bar.png")
        make_line(labels, {'speedup': m['Speedup'].tolist()}, 'Speedup old vs new',
                  'Speedup (old/new)', f"{args.output_prefix}_speedup_line.png")
