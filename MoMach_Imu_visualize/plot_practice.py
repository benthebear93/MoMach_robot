import random
import matplotlib.animation as animation
import matplotlib.pyplot as plt
from itertools import count
import numpy as np
from collections import deque

sensor_val = deque([3, 7, -3, 4, 6, 9, -5, 13, 4, 6, 9, -5, 13, 9, -5, 13, -5, 13, 4, 6, 10])
y_val = []
x_val = []
index = count()

def animate(i):
    x_val.append(next(index))
    y_val.append(sensor_val.popleft())
    plt.cla()
    plt.plot(x_val, y_val, label='sensor val')
    plt.legend(loc='upper left')
    plt.tight_layout()

ani = animation.FuncAnimation(plt.gcf(), animate, interval=1)
ani.save('mymovie.mp4')
plt.tight_layout()
plt.show()
