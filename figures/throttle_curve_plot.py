from matplotlib import pyplot as plt
import numpy as np

tq_list = []
x = np.arange(0, 103)
tqMax = 230
tqNegMax = 50
posStart = 5
posNegMax = 5
posNegZero = 25
posZero = 27
posHalf = 75
posMax = 98

for throttlePos in x:
	torque = 0
	if throttlePos >= posStart:
		if posNegMax > posStart:
			torque = - (((throttlePos-posStart) / (posNegMax-posStart)) * tqNegMax)
		if throttlePos >= posNegMax:
			if posNegZero > posNegMax:
				torque = - ( (1 - (throttlePos-posNegMax) / (posNegZero-posNegMax)) * tqNegMax)
			if throttlePos >= posNegZero:
				torque = 0
				if throttlePos >= posZero:
					if posHalf > posZero:
						torque = ((throttlePos-posZero) / (posHalf-posZero)) * tqMax/2
					if throttlePos >= posHalf:
						if posMax > posHalf:
							torque = tqMax/2 + ((throttlePos-posHalf) / (posMax-posHalf)) * tqMax/2
						if throttlePos >= posMax:
							torque = tqMax
	
	tq_list.append(torque)

plt.scatter(x, tq_list)
plt.savefig("throttle_curve.png")