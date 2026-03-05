# kernel stack timing

1. use perf record (I have instrumented PG to enable/disable perf)

```bash
mkfifo /tmp/perf_ctl
sudo perf record -a -F 200 -e cpu-clock:k -g --kernel-callchains -P --control=fifo:/tmp/perf_ctl --delay=-1 -o perf.data
```

this will keep running, and we should run this (in a separate terminal) before any Citus/Postgres start executing queries/transactions

1. Start running with Citus as usual, then once it’s done, use Ctrl-C to stop perf, which should generate a `perf.data`
2. Dump meaningful data from it:

```bash
sudo perf script -i perf.data -F comm,period,event,ip,sym,dso > perf.txt
```

4. `python3 sum_net.py < perf.txt` will analyze it:

- `net_fraction=xxx` is what matters, i.e., record that in the spreadsheet, the actual numbers in ns will be longer than normal due to perf overhead