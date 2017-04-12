
#use in drawing pictures
import sys

def timeCity():
	from mpl_toolkits.mplot3d import Axes3D
	import numpy as np
	import matplotlib.pyplot as plt

	f = open('GDP.txt','r');
	result = [];
	line = f.readline()
	while(line):
		line = line.strip('\n')
		line = line.split('\t')
		result.append(map(float,line));
		line = f.readline();
	result = np.array(result);

	x = np.linspace(0,23,24);
	y = result[:,2]

	X, Y = np.meshgrid(x,y)
	z = np.loadtxt('timeCity.txt').T

	fig = plt.figure();
	ax = fig.gca(projection = '3d');
	ax.plot_surface(X,Y,z,rstride = 8, cstride = 8, alpha = 0.5);
	cset = ax.contour(X, Y, z, zdir='z', offset=0)
	cset = ax.contour(X, Y, z, zdir='x', offset=0)
	cset = ax.contour(X, Y, z, zdir='y', offset=102000)
	ax.set_xlabel('Time(h)')
#	ax.set_xlim(0,25)
	ax.set_ylabel('GDP(avg)')
#	ax.set_ylim(22000, 10200)
	ax.set_zlabel('PV')
	
	plt.show();
	
def singleUVPV():
	import numpy as np
	import matplotlib.pyplot as plt
	
	data = np.loadtxt('singleTerm.txt')
	x = data[:,0];
	y = data[:,1];
	
	fig = plt.figure();
	plt.plot(x,y,'k',x,3*x,'k--');
	plt.xlabel('UV-->');
	plt.ylabel('PV-->');
	plt.legend(['Acture','PV=3UV'])
	plt.show();
	
def UVPV():
	import numpy as np
	import matplotlib.pyplot as plt
	import re
	
	f = open('terminal.txt');
	line = f.readline()
	result = [];
	while(line):
		line = line.strip('\n')
		result.append(line);
		line = f.readline();
	f.close();
	
	data = open('multiTerm.txt').read().split('\n\n'); #list
	fig = plt.figure();
	for i in range(0, 1):
		mydata = re.split('[ \n]',data[i]);
		mydata = map(int,mydata);
		newdata = np.array(mydata).reshape((len(mydata)/2,2));
		x = newdata[:,0];
		y = newdata[:,1];
		plt.plot(x,y)
	plt.plot(x,3*x,'k--');
	plt.legend(['SCH_E329','PV=3UV'],loc="lower right");
	plt.xlabel('UV-->');
	plt.ylabel('PV-->');
	plt.show();
	
if __name__ == "__main__":
	if (sys.argv[1] == '1'):
		timeCity();
	elif (sys.argv[1] == '2'):
		singleUVPV();
	else :
		UVPV();