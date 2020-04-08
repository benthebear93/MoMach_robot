
from matplotlib import pyplot as plt
from matplotlib import animation
from matplotlib import pyplot as plt
import time
import numpy as np

fig = plt.figure()
ax = plt.axes(xlim=(0, 400), ylim=(0, 400))

x = [10, 350, 350, -350, -350]
y = [-10, -350, 350, 350, -350]
xlog = []
ylog = []
# Major ticks every 20, minor ticks every 5
major_ticks = np.arange(0, 400, 20)
minor_ticks = np.arange(0, 400, 5)

ax.set_xticks(major_ticks)
ax.set_xticks(minor_ticks, minor=True)
ax.set_yticks(major_ticks)
ax.set_yticks(minor_ticks, minor=True)

# And a corresponding grid
ax.grid(which='both')

# Or if you want different settings for the grids:
#ax.grid(which='minor', alpha=0.2)
#ax.grid(which='major', alpha=0.5)

ax.plot(x, y, 'r--')
# plt.xlabel('X_axis')
# plt.title('Work_Space')

x_current = 0
y_current = 0
ax.scatter(x_current, y_current, marker='s', s=3, c='g')

plt.draw()
x_ch = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y_ch = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
while True:
    plt.clf()
    ax = plt.axes(xlim=(0, 400), ylim=(0, 400))
    major_ticks = np.arange(0, 400, 20)
    minor_ticks = np.arange(0, 400, 5)

    ax.set_xticks(major_ticks)
    ax.set_xticks(minor_ticks, minor=True)
    ax.set_yticks(major_ticks)
    ax.set_yticks(minor_ticks, minor=True)

    # And a corresponding grid
    ax.grid(which='both')

    # Or if you want different settings for the grids:
    ax.grid(which='minor', alpha=0.2)
    ax.grid(which='major', alpha=0.5)


    if len(x_ch) == 0:
        x_current = x_current
        y_current = y_current
    else:
        prev_x = x_ch.pop()
        prev_y = y_ch.pop()
        x_current = prev_x * 10
        y_current = prev_y * 10

    xlog.append(x_current)
    ylog.append(y_current)

    plt.grid()
    plt.xlabel('X_axis')
    plt.title('Work_Space')
    plt.scatter(xlog, ylog, marker='s', s=10, c='b')
    plt.scatter(x_current, y_current, marker='s', s=10, c='g')
    plt.draw()
    plt.pause(0.001)
