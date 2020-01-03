import subprocess
import matplotlib.pyplot as plt
import numpy as np

N = 1200
M = 700
iter_num = 5
threadsNum = [1,2,3,4,5,6,8,10,12,16,20,24,28,32]
Y1 = [0.] * len(threadsNum)
Y2 = [0.] * len(threadsNum)
Y3 = [0.] * len(threadsNum)

j = 0
for i in threadsNum:
    for k in range(iter_num):
        proc = subprocess.Popen("Task3_light.exe", shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        args = str(N) + ' ' + str(M) + ' 1 ' + str(i)
        proc.stdin.write(args.encode())
        proc.stdin.close()
        out = proc.stdout.readlines()
        out[0] = out[0].decode()
        out[1] = out[1].decode()
        out[2] = out[2].decode()
        Y1[j] += float(out[0])
        Y2[j] += float(out[1])
        Y3[j] += float(out[2])
    j += 1

Y1 = np.array(Y1)
Y2 = np.array(Y2)
Y3 = np.array(Y3)

Y1 = Y1 / 1. / iter_num
Y2 = Y2 / 1. / iter_num
Y3 = Y3 / 1. / iter_num

plt.plot(threadsNum, Y1, 'o-', label="Rows")
plt.plot(threadsNum, Y2, 'o-', label="Columns")
plt.plot(threadsNum, Y3, 'o-', label="Blocks")

plt.xlabel('Количество нитей')
plt.ylabel('Время работы алгоритма (сек.)')
plt.title('Количество строк матрицы (N) = ' + str(N) + ', размер вектора (M) = ' + str(M))
plt.legend()
plt.show()
