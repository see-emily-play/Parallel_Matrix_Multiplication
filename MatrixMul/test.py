import numpy as np

if __name__ == '__main__':
    with open('./A.txt', 'r') as file:
        A = file.readlines()
    A = [[int(n) for n in x.split()] for x in A]

    with open('./B.txt', 'r') as file:
        B = file.readlines()
    B = [[int(n) for n in x.split()] for x in B]

    with open('./C.txt', 'r') as file:
        C = file.readlines()
    C = [[int(n) for n in x.split()] for x in C]

    res = np.dot(A,B)
    print('Matrix multiplication is correct' if np.array_equal(C, res) else 'Matrix multiplication is incorrect')
