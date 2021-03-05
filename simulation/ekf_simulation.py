import math

import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial.transform import Rotation as Rot

# Covariance for EKF simulation
Q = np.diag([
    0.1,  # variance of location on x-axis
    0.1,  # variance of location on z-axis
    0.1  # variance of velocity
]) ** 2  # predict state covariance
R = np.diag([0, 0.3, 0.3]) ** 2  # Observation x,y position covariance

#  Simulation parameter
INPUT_NOISE = np.diag([0.2]) ** 2
MEASUREMENT_NOISE = np.diag([0, 0.3, 0.3]) ** 2

DT = 0.1  # time tick [s]
SIM_TIME = 380.0  # simulation time [s]

show_animation = True
offset = 0.15
roller = 0.08


def motion_model(x, u): # x : state  u : feed rate
    F = np.array([[1.0, 0, 0],
                  [0, 1.0, 0],
                  [0, 0, 0]], dtype='float')

    B = np.array([[DT * u + offset * math.cos(x[2, 0]*DT/offset)],
                  [offset * math.sin(x[2, 0]*DT/offset) + roller * math.sin((DT*x[2, 0])/(9*offset))],
                  [u]], dtype='float')

    print("B[2,0]", B[2,0])
    x = F @ x + B

    return x


def observation_model(x):
    H = np.array([
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ])

    z = H @ x

    return z


def observation(xTrue, xd, u):
    xTrue = motion_model(xTrue, u)
    print("xTrue", xTrue)
    # add noise to gps x-y
    z = observation_model(xTrue) + MEASUREMENT_NOISE @ np.random.randn(3, 1)

    # add noise to input
    ud = u + INPUT_NOISE @ np.random.randn(1, 1)

    xd = motion_model(xd, ud)

    return xTrue, z, xd, ud


def jacob_f(x, u):
    """
    Jacobian of Motion Model
    motion model
    x_{t+1} = x_t+v*dt*offset*cos(v*dt/offset)
    z_{t+1} = z_t+offset*sin(v*dt/offset)+roller*sin(v*t/9/offset)
    v_{t+1} = v{t}
    so
    dx/dv = dt-v*sin(v*dt/offset)
    dz/dv = v*cos(v*dt/offset)+(v*roller/9/offset)*cos(v*t/9/offset)
    """
    v = x[2, 0]
    jF = np.array([
        [1.0, 0.0, -DT * v * math.sin(v*DT/offset)],
        [0.0, 1.0, v * math.cos(v*DT/offset) + (v*roller/9/offset) * math.cos(v*DT/9/offset)],
        [0.0, 0.0, 1.0]])

    return jF


def jacob_h():
    # Jacobian of Observation Model
    jH = np.array([
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ])

    return jH


def ekf_estimation(xEst, PEst, z, u):
    #  Predict
    xPred = motion_model(xEst, u)
    jF = jacob_f(xEst, u)
    PPred = jF @ PEst @ jF.T + Q

    #  Update
    jH = jacob_h()
    zPred = observation_model(xPred)
    y = z - zPred
    S = jH @ PPred @ jH.T + R
    K = PPred @ jH.T @ np.linalg.inv(S)
    xEst = xPred + K @ y
    PEst = (np.eye(len(xEst)) - K @ jH) @ PPred
    return xEst, PEst


def main():
    print(__file__ + " start!!")

    time = 0.0

    # State Vector [x z v]'
    xEst = np.zeros((3, 1))
    xTrue = np.zeros((3, 1))
    PEst = np.eye(3)

    xDR = np.zeros((3, 1))  # Dead reckoning

    # history
    hxEst = xEst
    hxTrue = xTrue
    hxDR = xTrue
    hz = np.zeros((3, 1))

    while SIM_TIME >= time:
        time += DT
        u = 0.833 # 0.833 mm/sec

        xTrue, z, xDR, ud = observation(xTrue, xDR, u)

        xEst, PEst = ekf_estimation(xEst, PEst, z, ud)

        # store data history
        hxEst = np.hstack((hxEst, xEst))
        hxDR = np.hstack((hxDR, xDR))
        hxTrue = np.hstack((hxTrue, xTrue))
        hz = np.hstack((hz, z))

        if show_animation:
            plt.cla()
            # for stopping simulation with the esc key.
            plt.gcf().canvas.mpl_connect('key_release_event',
                    lambda event: [exit(0) if event.key == 'escape' else None])
            plt.plot(hz[0, :], hz[1, :], ".g")
            plt.plot(hxTrue[0, :].flatten(),
                     hxTrue[1, :].flatten(), "-b")
            plt.plot(hxDR[0, :].flatten(),
                     hxDR[1, :].flatten(), "-k")
            plt.plot(hxEst[0, :].flatten(),
                     hxEst[1, :].flatten(), "-r")
            #plot_covariance_ellipse(xEst, PEst)
            plt.axis("equal")
            plt.grid(True)
            plt.pause(0.001)

if __name__ == '__main__':
    main()