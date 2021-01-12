import numpy as np
import scipy.linalg
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

X = np.array([[24,13,38],[8,3,17],[21,6,40],[1,14,-9],[9,3,21],[7,1,14],[8,7,11],[10,16,3],[1,3,2],
    [15,2,30],[4,6,1],[12,10,18],[1,9,-4],[7,3,19],[5,1,13],[1,12,-6],[21,9,34],[8,8,7],
  [1,18,-18],[15,8,25],[16,10,29],[7,0,17],[14,2,31],[3,7,0],[5,6,7]])
pca = PCA(n_components=3)

pca.fit(X)
eig_vec = pca.components_

print(pca.explained_variance_ratio_)
# [0.90946569 0.08816839 0.00236591]
# Percentage of variance explain by last vector is less 0.2%

# This is the normal vector of minimum variance
normal = eig_vec[2, :]  # (a, b, c)
print("normal :", normal)
centroid = np.mean(X, axis=0)

# Every point (x, y, z) on the plane should satisfy a*x+b*y+c*z = d
# Taking centroid as a point on the plane
d = -centroid.dot(normal)

# Draw plane
xx, yy = np.meshgrid(np.arange(np.min(X[:, 0]), np.max(X[:, 0])), np.arange(np.min(X[:, 1]), np.max(X[:, 1])))

z = (-normal[0] * xx - normal[1] * yy - d) * 1. / normal[2]

# plot the surface
plt3d = plt.figure().gca(projection='3d')
plt3d.plot_surface(xx, yy, z)
plt3d.scatter(*(X.T))
plt.show()
'''
# some 3-dim points
mean = np.array([0.0, 0.0, 0.0])
cov = np.array([[1.0, -0.5, 0.8], [-0.5, 1.1, 0.0], [0.8, 0.0, 1.0]])
data = np.random.multivariate_normal(mean, cov, 50)

# regular grid covering the domain of the data
X, Y = np.meshgrid(np.arange(-3.0, 3.0, 0.5), np.arange(-3.0, 3.0, 0.5))
XX = X.flatten()
YY = Y.flatten()

order = 1  # 1: linear, 2: quadratic
if order == 1:
    # best-fit linear plane
    A = np.c_[data[:, 0], data[:, 1], np.ones(data.shape[0])]
    C, _, _, _ = scipy.linalg.lstsq(A, data[:, 2])  # coefficients

    # evaluate it on grid
    Z = C[0] * X + C[1] * Y + C[2]

    # or expressed using matrix/vector product
    # Z = np.dot(np.c_[XX, YY, np.ones(XX.shape)], C).reshape(X.shape)

elif order == 2:
    # best-fit quadratic curve
    A = np.c_[np.ones(data.shape[0]), data[:, :2], np.prod(data[:, :2], axis=1), data[:, :2] ** 2]
    C, _, _, _ = scipy.linalg.lstsq(A, data[:, 2])

    # evaluate it on a grid
    Z = np.dot(np.c_[np.ones(XX.shape), XX, YY, XX * YY, XX ** 2, YY ** 2], C).reshape(X.shape)

# plot points and fitted surface
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot_surface(X, Y, Z, rstride=1, cstride=1, alpha=0.2)
ax.scatter(data[:, 0], data[:, 1], data[:, 2], c='r', s=50)

plt.xlabel('X')
plt.ylabel('Y')
ax.set_zlabel('Z')
#ax.axis('equal')
#ax.axis('tight')
plt.show()'''