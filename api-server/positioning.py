import numpy as np

def test_positioning():
	beacons = np.array([[1, 8], [10, 3], [2, 30]])
	tempA = beacons * -2
	A = np.ones((3, 3))
	A[:,:-1] = tempA
	pinvA = np.linalg.inv(A)

	actual_pos = np.array([12, 10])
	ranges_measured = np.array([np.linalg.norm(beacons[:1,:]-actual_pos),
								np.linalg.norm(beacons[1:2,:]-actual_pos),
								np.linalg.norm(beacons[2:,:]-actual_pos)])
	y = np.multiply(ranges_measured, ranges_measured) - np.multiply(beacons[:,:1], beacons[:,:1]).reshape(1, 3) - np.multiply(beacons[:,1:2], beacons[:,1:2]).reshape(3)
	x = np.mat(pinvA) * np.mat(y).reshape(3, 1)
	estimate = x[:2,:].reshape(2)
	error = np.linalg.norm(actual_pos-estimate)

# `beacons` should be input with a two-dimensional array
def setup_positioning(beacons):
	global beacon_locations 
        global pinvA
        global b1
        global b2
        beacon_locations = np.matrix(beacons)
	tempA = beacon_locations * -2
	A = np.ones((3, 3))
	A[:,:-1] = tempA
	pinvA = np.linalg.inv(A)
	b1 = beacon_locations[:,:1]
	b2 = beacon_locations[:,1:2]
	return

def iterate_position(range1, range2, range3):
	ranges_measured = np.array([range1, range2, range3])
	y = np.multiply(ranges_measured, ranges_measured) - np.multiply(b1, b1).reshape(1, 3) - np.multiply(b2, b2).reshape(3)
	x = np.mat(pinvA) * np.mat(y).reshape(3, 1)
	estimate = x[:2,:].reshape(2)
	return estimate
