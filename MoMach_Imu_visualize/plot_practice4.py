import numpy as np
import matplotlib.pyplot as plt

fig, ax = plt.subplots()
ax.axis([0, 100, 0, 1])

y = np.random.rand(100)
lines = ax.plot(y)

fig.canvas.manager.show()

i=0
while 1:
    i=i+1
    y = np.random.rand(100)
    lines[0].set_ydata(y)
    fig.canvas.draw()
    fig.canvas.flush_events()