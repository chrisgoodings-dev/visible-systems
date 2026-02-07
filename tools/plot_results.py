import argparse
import csv
import json
import os
from pathlib import Path

import matplotlib.pyplot as plt

def load_csv(path):
    rows = []
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        for row in r:
            rows.append(row)
    return rows

def load_json_folder(folder):
    rows = []
    for p in Path(folder).glob("*.json"):
        with open(p, "r", encoding="utf-8") as f:
            j = json.load(f)
        r = {
            "name": j["benchmark"],
            "mean_ns": j["result"]["mean_ns"],
            "stdev_ns": j["result"]["stdev_ns"],
            "qpc_seconds": j.get("counters", {}).get("qpc_seconds", None),
            "thread_cycles": j.get("counters", {}).get("thread_cycles", None),
            "working_set_bytes": j.get("counters", {}).get("working_set_bytes", None),
            "page_fault_count": j.get("counters", {}).get("page_fault_count", None),
        }
        rows.append(r)
    return rows

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", help="CSV file from stdout redirection")
    ap.add_argument("--jsondir", help="Folder of per-benchmark JSON files")
    ap.add_argument("--title", default="Visible Systems Memory Lab Results")
    args = ap.parse_args()

    if not args.csv and not args.jsondir:
        raise SystemExit("Provide --csv or --jsondir")

    if args.csv:
        rows = load_csv(args.csv)
        names = [r["name"] for r in rows]
        means = [float(r["mean_ns"]) / 1e6 for r in rows]  # ms
        errs  = [float(r["stdev_ns"]) / 1e6 for r in rows]
    else:
        rows = load_json_folder(args.jsondir)
        rows.sort(key=lambda r: r["name"])
        names = [r["name"] for r in rows]
        means = [float(r["mean_ns"]) / 1e6 for r in rows]
        errs  = [float(r["stdev_ns"]) / 1e6 for r in rows]

    plt.figure()
    plt.bar(names, means, yerr=errs)
    plt.xticks(rotation=25, ha="right")
    plt.ylabel("Mean time (ms)")
    plt.title(args.title)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
