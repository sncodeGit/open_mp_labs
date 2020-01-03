import subprocess
import matplotlib.pyplot as plt
import numpy as np

N1 = 100
M1 = 100
M2 = 100
iter_num = 3

threadsNum = [1,2,3,4,5,6,8,10,12,16,20,24,28,32]
Y1 = [0.] * len(threadsNum)
Y2 = [0.] * len(threadsNum)

j = 0
for i in threadsNum:
    for k in range(iter_num):
        proc = subprocess.Popen("Task4_light.exe", shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        args = str(N1) + ' ' + str(M1) + ' ' + str(M2) + ' 1 ' + str(i)
        proc.stdin.write(args.encode())
        proc.stdin.close()
        out = proc.stdout.readlines()
        out[0] = out[0].decode()
        out[1] = out[1].decode()
        Y1[j] += float(out[0])
        Y2[j] += float(out[1])
    j += 1

Y1 = np.array(Y1)
Y2 = np.array(Y2)

Y1 = Y1 / 1. / iter_num
Y2 = Y2 / 1. / iter_num

plt.plot(threadsNum, Y1, 'o-', label="Blocks")
plt.plot(threadsNum, Y2, 'o-', label="Tapes")

plt.xlabel('Количество нитей')
plt.ylabel('Время работы алгоритма (сек.)')
plt.title('Размеры матриц N1*M1 и M1*M2 (N1, M1, M2): ' + str(N1) + ', ' + str(M1) + ', ' + str(M2))
plt.legend()
plt.show()  
