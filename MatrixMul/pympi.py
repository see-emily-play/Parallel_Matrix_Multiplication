from mpi4py import MPI
import numpy as np
import time

comm = MPI.COMM_WORLD
my_rank = comm.Get_rank()
n_processes = comm.Get_size()

n = 5040

if __name__ == '__main__':
    A = []
    B = []
    local_n = int(n / n_processes)
    X_part = np.empty((int(local_n), n), dtype='int')

    C = np.empty((n, n), dtype='int')

    if my_rank == 0:
        A = np.random.randint(low = 1, high = 10, size = (n, n))
		B = np.random.randint(low = 1, high = 10, size = (n, n))
        start = MPI.Wtime()

    comm.Scatterv(sendbuf = A, recvbuf=X_part, root = 0)
    B = comm.bcast(B, root=0)
	part_res = np.dot(X_part, B)
    comm.Gatherv(sendbuf=part_res, recvbuf=C, root=0)

    if my_rank == 0:
        end = MPI.Wtime()
		fR = open('./res.txt', 'a')
        fR.write("Elapsed time (transposed, " +str(n_processes) +" proc): " +str(round(end-start,6))+"\n")

        fA = open('./A', 'w')
        for i in range(len(A)):
            for j in range(len(A[i])):
                fA.write(str(A[i][j]) + ' ')
            fA.write('\n')
        fA.close()

        fB = open('./B', 'w')
        for i in range(len(B)):
            for j in range(len(B[i])):
                fB.write(str(B[i][j]) + ' ')
            fB.write('\n')
        fB.close()

        fC = open('./C', 'w')
        for i in range(len(result)):
            for j in range(len(result[i])):
                fC.write(str(result[i][j]) + ' ')
            fC.write('\n')
        fC.close()