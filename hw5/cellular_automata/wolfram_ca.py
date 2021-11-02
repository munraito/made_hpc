from mpi4py import MPI
import numpy as np
import matplotlib.pyplot as plt
import argparse


np.random.seed(1337)

# MPI variables 
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
proc_cnt = comm.Get_size()

# options from command line
parser = argparse.ArgumentParser()
parser.add_argument("rule", type=int)
parser.add_argument("size", type=int)
parser.add_argument("steps", type=int)
parser.add_argument("--periodic", action="store_true")
parser.add_argument("--savefig", action="store_true")
args = parser.parse_args()

RULE = args.rule
SEQ_SIZE = args.size
STEPS = args.steps
if args.periodic:
    PERIODIC = True
#    if rank == 0:
#        print('periodic is on')
else:
    PERIODIC = False
#    if rank == 0:
#        print('periodic is off')

# variables necessary for step() function
rule_b = np.array([int(_) for _ in np.binary_repr(RULE, 8)],
                  dtype=np.int8)
u = np.array([[4], [2], [1]])


def step(x, rule_b):
    #single step of an elementary cellular automaton
    y = np.vstack((np.roll(x, 1), x, np.roll(x, -1))).astype(np.int8)
    # LCR pattern for numbers between 0 and 7 
    z = np.sum(y * u, axis=0).astype(np.int8)
    res = rule_b[7 - z]
    # print(rank, 'before:', res)
    if not PERIODIC:
        if rank == 0:
            # fix first cell
            res[0] = x[0]
        elif rank == proc_cnt - 1:
            # fix last cell
            res[-1] = x[-1]
    # print(rank, 'after:', res)
    return res


def send_ghost(arr):
    # send and receive ghost cells
    # leftmost
    if rank == 0:
        comm.send(arr[-1], dest=rank + 1)
        if PERIODIC:
            comm.send(arr[0], dest=proc_cnt - 1)
            arr = np.insert(arr, 0, comm.recv(source=proc_cnt - 1))
        arr = np.append(arr, comm.recv(source=rank + 1))
    # rightmost
    elif rank == proc_cnt - 1:
        if PERIODIC:
            comm.send(arr[-1], dest=0)
            arr = np.append(arr, comm.recv(source=0))
        comm.send(arr[0], dest=rank - 1)
        arr = np.insert(arr, 0, comm.recv(source=rank - 1))
    # middle
    else:
        comm.send(arr[0], dest=rank - 1)
        comm.send(arr[-1], dest=rank + 1)
        arr = np.append(arr, comm.recv(source=rank + 1))
        arr = np.insert(arr, 0, comm.recv(source=rank - 1))
    return arr


def slice_ghost(arr):
    # leave only significant cells for resulting array
    if PERIODIC:
        # cut from both sides
        arr = arr[1:-1]
    else:
        if rank == 0:
            # cut last ghost cell
            arr = arr[:-1]
        elif rank == proc_cnt - 1:
            # cut first ghost cell
            arr = arr[1:]
        else:
            arr = arr[1:-1]        
    return arr


# define size for each array
k, m = divmod(SEQ_SIZE, proc_cnt)
start = rank * k + min(rank, m)
end = (rank + 1) * k + min(rank + 1, m)
proc_size = end - start
# print(f"{rank} size: {proc_size}")
# initialize with random binary
arr = np.random.randint(2, size=proc_size)
# print('init:', rank, arr)
# np.full(shape=proc_size, fill_value=rank)
# np.random.randint(2, size=proc_size)

start_time = MPI.Wtime()
history = np.zeros((STEPS, proc_size), dtype=np.int8)
history[0, :] = arr
# gather_all(arr, 0) 

# main loop 
for i in range(STEPS - 1):
    arr = send_ghost(arr)
    # print(rank, arr)
    arr = step(arr, rule_b)
    arr = slice_ghost(arr)
    history[i + 1, :] = arr

#gather all into one array
gathered = comm.gather(history, root=0)

if rank == 0:
    res = np.concatenate([a for a in gathered], axis=1)
    if args.savefig:
        plt.figure(figsize=(10, 10))
        plt.axis('off')
        plt.title(f"rule {str(RULE)}")
        plt.imshow(res, cmap=plt.cm.binary);
        plt.savefig(f"visualize_results/rule_{str(RULE)}_{str(SEQ_SIZE)}x{str(STEPS)}.png")
    print(rank, 'time elapsed:', MPI.Wtime() - start_time)

MPI.Finalize()
