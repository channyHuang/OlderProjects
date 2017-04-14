#include "MathAction.h"
#include "math.h"
#include <vector>
#define ROTATE(A,i,j,k,l) g=A[i][j];h=A[k][l];A[i][j]=g-s*(h+g*tau);A[k][l]=h+s*(g-h*tau);
#define TINY 1.0e-200
using namespace std;


MathAction::MathAction(void)
{
}

MathAction::~MathAction(void)
{
}

void MathAction::CrossProduct(float x1, float y1 , float z1 , float x2 , float y2 , float z2 , float & x3 , float & y3 , float & z3)
{
	x3 = y1 * z2 - y2 * z1;
	y3 = - ( x1 * z2 - x2 * z1 );
	z3 = x1 * y2 - x2 * y1;
}


void MathAction::RotateVector(float x1, float y1 , float z1 , float x2 , float y2 , float z2 , float theta, float & x3 , float & y3 , float & z3)
{
	//float ReferenceVec[3] , normalvec[3];
	//float Xvec[3],Yvec[3];
	//
	//float  model,model1;

	//model=sqrt(x2*x2+y2*y2+z2*z2);

	////Normalize(x1,y1,z1);
	//Normalize(x2,y2,z2);
	//CrossProduct(x2,y2,z2,x1,y1,z1,normalvec[0],normalvec[1],normalvec[2]);
	//CrossProduct(normalvec[0],normalvec[1],normalvec[2],x2,y2,z2,
	//		ReferenceVec[0],ReferenceVec[1],ReferenceVec[2]);

	//model1=Normalize(ReferenceVec[0],ReferenceVec[1],ReferenceVec[2]);

	//
	//float XX,YY;

	//XX=DotProduct(x1,y1,z1,ReferenceVec[0],ReferenceVec[1],ReferenceVec[2]);
	//YY=DotProduct(x1,y1,z1,x2,y2,z2);

	//Xvec[0]=XX*x2;
	//Xvec[1]=XX*y2;
	//Xvec[2]=XX*z2;

	//Yvec[0]=YY*ReferenceVec[0];
	//Yvec[1]=YY*ReferenceVec[1];
	//Yvec[2]=YY*ReferenceVec[2];

	//x3=Xvec[0]*cos(theta)-Yvec[0]*sin(theta)+Yvec[0]*cos(theta)+Xvec[0]*sin(theta);
	//y3=Xvec[1]*cos(theta)-Yvec[1]*sin(theta)+Yvec[1]*cos(theta)+Xvec[1]*sin(theta);
	//z3=Xvec[2]*cos(theta)-Yvec[2]*sin(theta)+Yvec[2]*cos(theta)+Xvec[2]*sin(theta);



	//假设传进来的参数轴和向量是不垂直的

	float mod,staticvectormod;// 输入向量的模长；投影到轴上，不转动的部分的模长
	Normalize(x2,y2,z2);
	staticvectormod=DotProduct(x1,y1,z1,x2,y2,z2);
	x1-=staticvectormod*x2;
	y1-=staticvectormod*y2;
	z1-=staticvectormod*z2;

	mod=Normalize(x1,y1,z1);
	CrossProduct(x2,y2,z2,x1,y1,z1,x3,y3,z3);

	Normalize(x3,y3,z3);

	float XX,YY;
	float Xvec[3],Yvec[3];
	Xvec[0]=mod*cos(theta)*x1;
	Xvec[1]=mod*cos(theta)*y1;
	Xvec[2]=mod*cos(theta)*z1;

	Yvec[0]=mod*sin(theta)*x3;
	Yvec[1]=mod*sin(theta)*y3;
	Yvec[2]=mod*sin(theta)*z3;

	x3=Xvec[0]+Yvec[0]+staticvectormod*x2;
	y3=Xvec[1]+Yvec[1]+staticvectormod*y2;
	z3=Xvec[2]+Yvec[2]+staticvectormod*z2;












}
float MathAction::DotProduct(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float result;
	result=x1*x2+y1*y2+z1*z2;
	return result;
}

float MathAction::Normalize(float &x1, float &y1, float &z1) // 如果传入的是0向量，定义结果为（0，0，0），模为0。
{
	float result;
	result=sqrt(DotProduct(x1,y1,z1,x1,y1,z1));
	if(result != 0)
	{
		x1/=result;
		y1/=result;
		z1/=result;
	}

	return result;
}

float MathAction::BsplineBasis(int i, int k, float t)
{
	vector<float> tk;
	float Ni,Niplus1,N;

	if (k==1)
	{
		if(t<(i+1)&&t>i) 
			return 1;
		else
			return 0;

	}
	else
	{
		Ni=((t-i)*BsplineBasis(i,k-1,t))/(i+k-1-i);
		Niplus1=((i+k-t)*BsplineBasis(i+1,k-1,t))/(i+k-(i+1));
		N=Ni+Niplus1;
		return N;
	}

}



//template<class T>
bool MathAction::LUcmp(MatrixGao<float>& A, int n, int* indx, float * d) //modify the elements of A to get the result
{
	int i , imax, j,k;
	float big,dum,sum,temp;
	vector<float> vv;

	*d=1.0;

	for(i=0;i<n;++i)
	{
		big=0.0;
		for(j=0;j<n;++j)
		{
			if((temp=fabs(A[i][j]))>big)
				big=temp;
		}
		if(big==0)
		{
			return false;
		}
		vv.push_back(1.0/big);
	}

	for(j=0;j<n;++j)
	{
		for(i=0;i<j;++i)
		{
			sum=A[i][j];
			for(k=0;k<i;++k)
				sum -= A[i][k]*A[k][j];
			A[i][j]=sum;
		}
		big=0;
		for(i=j;i<n;++i)
		{
			sum=A[i][j];
			for(k=0;k<j;++k)
				sum-=A[i][k]*A[k][j];
			A[i][j]=sum;
			if(((dum=vv[i]*fabs(sum)) >big)||((dum=vv[i]*fabs(sum)) == big))
			{
				big=dum;
				imax=i;
			}
		}
		if(j!=imax)
		{
			for(k=0;k<n;++k)
			{
				dum=A[imax][k];
				A[imax][k]=A[j][k];
				A[j][k]=dum;
			}
			*d=-(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if(A[j][j]==0.0)A[j][j]=TINY;
		if(j!=n-1)
		{
			dum=1.0/(A[j][j]);
			for(i=j+1;i<n;++i)
				A[i][j] *= dum;
		}
	}
	vv.clear();
	return true;
}


//template <class T>
const vector<float> MathAction::MatrixMutiplyVector(MatrixGao<float> A, vector<float> x)
{
	vector<float> result;
	int i,j;
	float sum=0;
	if(A.sizeC!=x.size())
	{
		return result;
	}


	for(i=0;i<A.sizeR;++i)
	{
		sum=0;
		for(j=0;j<A.sizeC;++j)
		{
			sum+=A[i][j]*x[j];
		}
		result.push_back(sum);
	}
	return result;


}

//template <class T>
const vector<float> MathAction::VectorMutiplyMatrix(vector<float> x,MatrixGao<float> A)
{
	vector<float> result;
	int i,j;
	float sum=0;

	if(A.sizeR!=x.size())
	{
		return result;
	}


	for(j=0;j<A.sizeC;++j)
	{
		sum=0;
		for(i=0;i<A.sizeR;++i)
		{
			sum+=x[i]*A[i][j];

		}
		result.push_back(sum);
	}

	return result;

}
void MathAction::OrdinateConvert1to2(float p[3], float q[9])
{
	q[0]=p[0];
	q[1]=p[1];
	q[2]=p[2];
	q[3]=p[0]*p[0];
	q[4]=p[1]*p[1];
	q[5]=p[2]*p[2];
	q[6]=p[0]*p[1];
	q[7]=p[1]*p[2];
	q[8]=p[2]*p[0];

}

//template <class T>
void MathAction::LUcmpSolve(MatrixGao<float> A, float b[]) // this function will modify the matrix A;b will be return as the root .
{
	int i,ii=0,ip,j,flag=0;
	float sum;
	float d;
	int * indx = new int [A.sizeR];
	LUcmp(A,A.sizeR,indx,&d);
	for(i=0;i<A.sizeR;++i)
	{
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if(flag) // ii will be used to  store the first nonzero element of root  
		{
			for(j=ii;j<i;++j)
			{
				sum-=A[i][j]*b[j];
			}
		}
		else // only enter in once
		{
			if(sum) 
			{
				ii=i;
				flag=1;
			}
		}
		b[i]=sum;
	}

	for(i=A.sizeR-1;i>-1;--i)
	{
		sum=b[i];
		for(j=i+1;j<A.sizeR;++j)
			sum -= A[i][j]*b[j];
		b[i]=sum/A[i][i];

	}
}

void MathAction::LUcmpSolve_FromLUM(MatrixGao<float> A, int * indx, float b[]) 
{
	int i,ii=-1,ip,j,flag=0;
	float sum;
	float d;
	//int * indx = new int [A.sizeR];
	/*LUcmp(A,A.sizeR,indx,&d);*/
	for(i=0;i<A.sizeR;++i)
	{
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if(flag) // ii will be used to  store the first nonzero element of root  
		{
			for(j=ii;j<i;++j)
			{
				sum-=A[i][j]*b[j];
			}
		}
		else // only enter in once
		{
			if(sum) 
			{
				ii=i;
				flag=1;
			}
		}
		b[i]=sum;
	}

	for(i=A.sizeR-1;i>-1;--i)
	{
		sum=b[i];
		for(j=i+1;j<A.sizeR;++j)
			sum -= A[i][j]*b[j];
		b[i]=sum/A[i][i];

	}

}

//template <class T>
const MatrixGao<float> MathAction::MatrixInverse(MatrixGao<float>A)
{
	int* indx=new int [A.sizeR];
	float *col=new float [A.sizeR];
	int i,j;
	float d;
	MatrixGao<float> Y(A.sizeC,A.sizeR);


	LUcmp(A,A.sizeR,indx,&d);
	for(j=0;j<A.sizeR;++j)
	{
		for(i=0;i<A.sizeC;++i)col[i]=0.0;
		col[j]=1.0;
		LUcmpSolve_FromLUM(A,indx,col);
		for(i=0;i<A.sizeC;++i) Y[i][j]=col[i];
	}




	delete indx;
	delete col;
	return Y;
}




MatrixGao<float> MathAction::AddMatrix(MatrixGao<float> A, MatrixGao<float> B)
{
	if((A.sizeR!=B.sizeR)||(A.sizeC!=B.sizeC))
	{
		MatrixGao<float> D(0,0);
		return D;

	}

	int i,j,k;
	MatrixGao<float> C(A.sizeR,A.sizeC);
	for(i=0;i<A.sizeR;++i)
	{
		for(j=0;j<A.sizeC;++j)
		{	
			C[i][j]+=A[i][j]+B[i][j];
		}
	}

	return C;
}
MatrixGao<float> MathAction::MutiplyMatrix(MatrixGao<float> A, MatrixGao<float> B)
{


	if(A.sizeC!=B.sizeR)
	{
		MatrixGao<float> D(0,0);
		return D;

	}

	int i,j,k;
	MatrixGao<float> C(A.sizeR,B.sizeC);
	for(i=0;i<A.sizeR;++i)
	{
		for(j=0;j<B.sizeC;++j)
		{
			for(k=0;k<A.sizeC;++k)
				C[i][j]+=A[i][k]*B[k][j];
		}
	}

	return C;
}

float MathAction::Distance3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

vector<float> MathAction::LeastSquareSollve(MatrixGao<float> A,int n , float* b)// n is the number of the equation.
{
	MatrixGao<float>At,Coef;
	vector<float> bb,DuplicateOfb;
	int i;

	At=A;
	At.Transpose();

	Coef=MutiplyMatrix(At,A);

	for(i=0;i<n;++i)
	{
		DuplicateOfb.push_back(b[i]);

	}

	bb=MatrixMutiplyVector(At,DuplicateOfb);


	int size=bb.size();
	float *bbarray=new float [size];

	for(i = 0 ; i<size; ++i)
	{
		bbarray[i]=bb[i];

	}
	LUcmpSolve(Coef,bbarray);

	for(i = 0 ; i<size; ++i)
	{
		bb[i]=bbarray[i];

	}

	delete bbarray;
	return bb;

}

void MathAction::jacobiEigenSystem(MatrixGao<float> A, int n, float* d, MatrixGao<float>&  v, int* nrot)
{
	int j,iq,ip,i;
	float tresh,theta,tau,t,sm,s,h,g,c;
	vector<float> b, z;
	for(i=0;i<n;++i)
	{
		b.push_back(0);
		z.push_back(0);
	}

	v=MatrixGao<float>(n,n);
	for(ip=0;ip<n;++ip)
	{
		v[ip][ip]=1;
	}

	for(ip=0;ip<n;++ip)
	{
		b[ip]=d[ip]=A[ip][ip];
		z[ip]=0.0;
	}

	*nrot=0;

	for(i=0;i<50;++i)
	{
		sm=0.0;
		for(ip=0;ip<n-1;++ip)
		{
			for(iq=ip+1;iq<n;++iq)
			{
				sm+=fabs(A[ip][iq]);
			}
		}

		if(sm == 0.0)
		{
			z.clear();
			b.clear();
			return;
		}

		if(i<4)
		{
			tresh=0.2*sm/(n*n);
		}
		else
		{
			tresh=0.0;
		}

		for(ip=0;ip<n-1;++ip)
		{
			for(iq=ip+1;iq<n;++iq)
			{
				g=100.0*fabs(A[ip][iq]);

				if(i>4 && (float)(fabs(d[ip])+g)==(float)fabs(d[ip])&& (float)(fabs(d[iq])+g)==(float)fabs(d[iq]))
					A[ip][iq]=0.0;
				else
				{
					if(fabs(A[ip][iq])>tresh)
					{
						h=d[iq]-d[ip];
						if((float)(fabs(h)+g)==(float)(fabs(h)))
							t=(A[ip][iq])/h;
						else
						{
							theta = 0.5*h/(A[ip][iq]);
							t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
							if(theta<0.0) t=-t;
						}

						c=1.0/sqrt(1+t*t);
						s=t*c;
						tau=s/(1+c);
						h=t*A[ip][iq];
						z[ip] -= h;
						z[iq] += h;
						d[ip] -= h;
						d[iq] += h;
						A[ip][iq] = 0.0;

						for(j=0;j<ip;++j)
						{
							ROTATE(A,j,ip,j,iq);
						}

						for(j=ip+1; j<iq ;++j)
						{
							ROTATE(A,ip,j,j,iq);

						}

						for(j=iq+1;j<n;++j)
						{
							ROTATE(A,ip,j,iq,j);
						}

						for(j=0;j<n;++j)
						{
							ROTATE(v,j,ip,j,iq);
						}
						++(*nrot);
					}
				}
			}			
		}
		for(ip=0;ip<n;++ip)
		{
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.0;
		}

	}

}



//void MathAction::ComputeRigidParameter(vector<Point3D>& arrayL,vector<Point3D>& arrayR,float translate[3],float rotate[3][3])
//{
//	float eigenvalue;
//	float eigenvector[4];
//	size_t numpoints;
//	float scale;
//	Point3D translatepoint,averageL,averageR;
//
//	if(arrayL.size()!=arrayR.size())
//	{
//		AfxMessageBox(_T("\'左\'\'右\'文件顶点数目不相同，无法计算！"));
//		exit(1);
//	}
//	numpoints=arrayL.size();	
//	vector<Point3D> arrayL1(numpoints),arrayR1(numpoints);
//
//	InitData(arrayL,arrayR,arrayL1,arrayR1,averageL,averageR,numpoints);
//	scale=ComputeScale(arrayL1,arrayR1,numpoints);
//	eigenvalue=ComputeEigenvalue(arrayL1,arrayR1,numpoints,eigenvector);//Sxx,Sxy,Sxz,Syx,Syy,Syz,Szx,Szy,Szz
//	ComputeRotateMatrix(rotate,eigenvector);
//	translatepoint=ComputeTranslate(translate,scale,rotate,averageL,averageR);
//
//
//	/*MatrixGao<float> A;
//	A=&rotate[0][0];
//	LUcmpSolve(A,translate);*/
//	/*ShowResult(translate,rotate,scale);*/
//}

//void MathAction::InitData(vector<Point3D>& arrayL,vector<Point3D>& arrayR,vector<Point3D>& arrayL1,
//						  vector<Point3D>& arrayR1,Point3D& averageL,Point3D& averageR,size_t numpoints)
//{
//	Point3D tempL,tempR;
//	for(size_t i=0;i<numpoints;i++)
//	{
//		tempL=tempL+arrayL[i];
//		tempR=tempR+arrayR[i];
//	}
//	averageL=tempL/float(numpoints);
//	averageR=tempR/float(numpoints);
//	for(size_t i=0;i<numpoints;i++)
//	{
//		arrayL1[i]=arrayL[i]-averageL;
//		arrayR1[i]=arrayR[i]-averageR;
//	}
//}
//float MathAction::ComputeScale(vector<Point3D>& arrayL1,vector<Point3D>& arrayR1,size_t numpoints)
//{
//	float Sr=0,Sl=0;
//
//	for(size_t i=0;i<numpoints;i++)
//	{
//		Sr=Sr+arrayL1[i].ComputeDistance();
//		Sl=Sl+arrayR1[i].ComputeDistance();
//	}
//	return(sqrt(Sr/Sl));
//}
//float MathAction::ComputeEigenvalue(vector<Point3D>& arrayL1,vector<Point3D>& arrayR1,size_t numpoints,float eigenvector[4])
//{
//	int j,nrot;
//	float Sxx=0,Sxy=0,Sxz=0,Syx=0,Syy=0,Syz=0,Szx=0,Szy=0,Szz=0;
//	for(size_t i=0;i<numpoints;i++)
//	{
//		Sxx=Sxx+arrayL1[i].x*arrayR1[i].x;
//		Sxy=Sxy+arrayL1[i].x*arrayR1[i].y;
//		Sxz=Sxz+arrayL1[i].x*arrayR1[i].z;
//		Syx=Syx+arrayL1[i].y*arrayR1[i].x;
//		Syy=Syy+arrayL1[i].y*arrayR1[i].y;
//		Syz=Syz+arrayL1[i].y*arrayR1[i].z;
//		Szx=Szx+arrayL1[i].z*arrayR1[i].x;
//		Szy=Szy+arrayL1[i].z*arrayR1[i].y;
//		Szz=Szz+arrayL1[i].z*arrayR1[i].z;
//	}
//	const int NP=4;
//	const float c_d[NP*NP]=
//	{Sxx+Syy+Szz , Syz-Szy , Szx-Sxz , Sxy-Syx,
//	Syz-Szy , Sxx-Syy-Szz , Sxy+Syx , Szx+Sxz,
//	Szx-Sxz , Sxy+Syx , -Sxx+Syy-Szz , Syz+Szy,
//	Sxy-Syx , Szx+Sxz , Syz+Szy , -Sxx-Syy+Szz};
//
//	//vector<float> d(NP,0);
//	float* d = new float [NP];
//	MatrixGao<float> v(NP,NP),e(NP,NP);
//	e=c_d;
//
//	/*cout << "****** Finding Eigenvectors ******" << endl;*/
//	jacobiEigenSystem(e, e.sizeR, d, v, &nrot);
//
//	eigsrt(d,v,v.sizeR);
//
//
//	for(j=0;j<NP;j++)
//	{
//		eigenvector[j]=v[j][0];
//	}
//	return d[0];
//}
void MathAction::ComputeRotateMatrix(float rotate[3][3],float eigenvector[4])
{
	float q0,qx,qy,qz;

	q0=eigenvector[0];
	qx=eigenvector[1];
	qy=eigenvector[2];
	qz=eigenvector[3];
	float rotatematrix[3][3]={(q0*q0+qx*qx-qy*qy-qz*qz) , 2*(qx*qy-q0*qz) , 2*(qx*qz+q0*qy),
		2*(qy*qx+q0*qz) , (q0*q0-qx*qx+qy*qy-qz*qz) , 2*(qy*qz-q0*qx),
		2*(qz*qx-q0*qy) , 2*(qz*qy+q0*qx) , (q0*q0-qx*qx-qy*qy+qz*qz)};
	memcpy(rotate,rotatematrix,sizeof(float)*9);
}
//Point3D MathAction::ComputeTranslate(float translate[3],float scale,float rotate[3][3],
//									 Point3D& averageL,Point3D& averageR)
//{
//	Point3D temp,temptranslate;
//
//	temp=scale*(rotate*averageL);
//	temptranslate=averageR-temp;
//	translate[0]=temptranslate.x;
//	translate[1]=temptranslate.y;
//	translate[2]=temptranslate.z;
//	return temptranslate;
//}
//

void MathAction::eigsrt(float d[] , MatrixGao<float>v,int n)
{
	int k,j,i;
	float p;

	for (i=0;i<n;++i)
	{
		p=d[k=i];
		for(j=i+1;j<n;++j)
		{
			if(d[j]>=p)
				p=d[k=j];
		}
		if(k!=i)
		{
			d[k]=d[i];
			d[i]=p;
			for(j=0;j<n;++j)
			{
				p=v[j][i];
				v[j][i]=v[i][k];
				v[j][k]=p;
			}
		}
	}
}
float MathAction::Distance3D_PnttoFace(float A, float B, float C, float D, float x, float y, float z)
{

	float result=fabs(A*x+B*y+C*z+D)/sqrt(A*A+B*B+C*C);

	return result;
}
