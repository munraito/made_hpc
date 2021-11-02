from mpi4py import MPI
import string
import random

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
to_visit = list(list(range(1, size)))
name = str(rank) + '_' + ''.join(random.choices(string.ascii_lowercase, k=5))

if rank == 0:
    visited = [rank]
    new_dest = random.choice(to_visit)
    msg = name
    comm.ssend((msg, visited), dest=new_dest)
else:
    msg, visited = comm.recv()
    msg += ' ' + name
    visited.append(rank)
    to_visit = [r for r in to_visit if r not in visited]
    if to_visit:
        new_dest = random.choice(to_visit)
        comm.ssend((msg, visited), dest=new_dest)
    else:
        print(msg)
    
MPI.Finalize()
