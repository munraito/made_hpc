run example:
```bash
mpirun -n 4 python wolfram_ca.py 110 1000 1000 --periodic --savefig
```

testing runtimes with multiple processes on cluster:
```bash
for i in {2..6}; do sbatch --output nodes_$i.txt -n $i submit.sh; done

tail -n +1 *.txt >res_times.txt
```

saved images are in [visualize_results](/visualize_results)
