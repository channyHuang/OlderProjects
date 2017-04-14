#include "SpotMath.h"
#include <math.h>
#include "Point3D.h"

SpotMath::SpotMath(void)
{
}

bool SpotMath::IsIntersectPatchX(const CPoint3D& ps1, const CPoint3D& ps2, const int xFace, 
								 const int ymin, const int ymax, const int zmin, const int zmax)
{
	//(值为归一过的index)直线＜PS1, PS2＞ 是否和正方形面片有交点。
	//面片在x=xFace的平面上，y最小为ymin，y最大为ymax，z最小为zmin，z最大为zmax

	//ps1 ps2是否为同一点的判断，在外保证．由于速度考虑不再比较是否相同．
	float xDis1 = ps1.getX()-xFace;
	float xDis2 = ps2.getX()-xFace;

	if ( xDis1== 0 || xDis2 == 0)//在xFace上
		return true;
	else// 原来程序是线段求交，判断if ((xDis1<0 && xDis2>0)|| (xDis1>0 && xDis2<0))//在xFace的不同边．．目前直线求交，不再判断符号．
	{
		float tmp = xDis1/xDis2;

		if (tmp==1)
			return false;

		float intersectY = (ps1.getY() - ps2.getY()*tmp)/(1.0-tmp);

		int _ymax,_ymin;
		if (ymax>ymin)
		{
			_ymax = ymax;
			_ymin = ymin;
		}
		else
		{
			_ymax = ymin;
			_ymin = ymax;
		}

		if (intersectY>_ymax || intersectY<_ymin)
			return false;

		float intersectZ = (ps1.getZ() - ps2.getZ()*tmp)/(1.0-tmp);

		int _zmax,_zmin;
		if (zmax>zmin)
		{
			_zmax = zmax;
			_zmin = zmin;
		}
		else
		{
			_zmax = zmin;
			_zmin = zmax;
		}

		if (intersectZ>_zmax || intersectZ<_zmin)
			return false;
		else
			return true;
	}
}

bool SpotMath::IsIntersectPatchY(const CPoint3D& ps1, const CPoint3D& ps2, const int yFace,
								 const int xmin, const int xmax, const int zmin, const int zmax)
{
	//(值为归一过的index)直线PS1, PS2 是否和正方形面片有交点。
	//面片在y=yFace的平面上，x最小为xmin，x最大为xmax，z最小为zmin，z最大为zmax

	float yDis1 = ps1.getY()-yFace;
	float yDis2 = ps2.getY()-yFace;

	if ( yDis1== 0 || yDis2 == 0)//在xFace上
		return true;
	else// if ((yDis1<0 && yDis2>0) || (yDis1>0 && yDis2<0))//在yFace的不同边
	{
		float tmp = yDis1/yDis2;

		if (tmp==1)
			return false;

		float intersectX = (ps1.getX() - ps2.getX()*tmp)/(1.0-tmp);

		int _xmax,_xmin;
		if (xmax>xmin)
		{
			_xmax = xmax;
			_xmin = xmin;
		}
		else
		{
			_xmax = xmin;
			_xmin = xmax;
		}

		if ( intersectX>_xmax || intersectX<_xmin)
			return false;

		float intersectZ = (ps1.getZ() - ps2.getZ()*tmp)/(1.0-tmp);

		int _zmax,_zmin;
		if (zmax>zmin)
		{
			_zmax = zmax;
			_zmin = zmin;
		}
		else
		{
			_zmax = zmin;
			_zmin = zmax;
		}

		if (intersectZ>_zmax || intersectZ<_zmin)
			return false;
		else
			return true;
	}
	//else//在xFace面的同一边，无交点。
	//	return false;
}

bool SpotMath::IsIntersectPatchZ(const CPoint3D& ps1, const CPoint3D& ps2, const int zFace,
								 const int xmin, const int xmax, const int ymin, const int ymax)
{
	//(值为归一过的index)直线PS1, PS2 是否和正方形面片有交点。
	//面片在z=zFace的平面上，x最小为xmin，x最大为xmax，y最小为ymin，y最大为ymax

	float zDis1 = ps1.getZ()-zFace;
	float zDis2 = ps2.getZ()-zFace;

	if ( zDis1== 0 || zDis2 == 0)//在xFace上
		return true;
	else// if ((zDis1<0 && zDis2>0) || (zDis1>0 && zDis2<0))//在zFace的不同边
	{
		float tmp = zDis1/zDis2;

		if (tmp==1)
			return false;

		float intersectX = (ps1.getX() - ps2.getX()*tmp)/(1.0-tmp);

		int _xmax,_xmin;
		if (xmax>xmin)
		{
			_xmax = xmax;
			_xmin = xmin;
		}
		else
		{
			_xmax = xmin;
			_xmin = xmax;
		}

		if ( intersectX>_xmax || intersectX<_xmin)
			return false;

		float intersectY = (ps1.getY() - ps2.getY()*tmp)/(1.0-tmp);

		int _ymax,_ymin;
		if (ymax>ymin)
		{
			_ymax = ymax;
			_ymin = ymin;
		}
		else
		{
			_ymax = ymin;
			_ymin = ymax;
		}

		if (intersectY>_ymax || intersectY<_ymin)
			return false;
		else
			return true;
	}
}

bool SpotMath::IsIntersect( const CPoint3D& ps1, const CPoint3D& ps2,
						   const CPoint3D& pt1, const CPoint3D& pt2, 
						   const CPoint3D& pt3 ,CPoint3D& sectPt)
{
	CPoint3D v0(ps2.getX() - ps1.getX(), 
		ps2.getY() - ps1.getY(), ps2.getZ() - ps1.getZ());

	CPoint3D ns = Normalize(v0);

	float t0;
	if( ns.getX() )
	{
		t0 = (ps2.getX() - ps1.getX()) / ns.getX();
	}
	else if( ns.getY() )
	{
		t0 = (ps2.getY() - ps1.getY()) / ns.getY();
	}
	else
	{
		t0 = (ps2.getZ() - ps1.getZ()) / ns.getZ();
	}

	CPoint3D v1(pt2.getX() - pt1.getX(), 
		pt2.getY() - pt1.getY(), pt2.getZ() - pt1.getZ());
	CPoint3D v2(pt3.getX() - pt2.getX(), 
		pt3.getY() - pt2.getY(), pt3.getZ() - pt2.getZ());
	CPoint3D v3(pt1.getX() - pt3.getX(), 
		pt1.getY() - pt3.getY(), pt1.getZ() - pt3.getZ());

	CPoint3D nt = Normalize( Cross(v1,v2) );

	float dn = Dot(nt,ns);

	if( dn == 0.0 )
	{
		return false;
	}

	float ti = ( Dot(nt, pt1) - Dot(nt, ps1) ) / dn;

	if( t0 * ti < 0 || fabs(t0) < fabs(ti) )
	{
		return false;
	}

	CPoint3D pi( ps1.getX() + ns.getX() * ti, 
		ps1.getY() + ns.getY() * ti, ps1.getZ() + ns.getZ() * ti );

	CPoint3D v01(pi.getX() - pt1.getX(), 
		pi.getY() - pt1.getY(), pi.getZ() - pt1.getZ());
	CPoint3D v02(pi.getX() - pt2.getX(), 
		pi.getY() - pt2.getY(), pi.getZ() - pt2.getZ());
	CPoint3D v03(pi.getX() - pt3.getX(), 
		pi.getY() - pt3.getY(), pi.getZ() - pt3.getZ());

	CPoint3D n1 = Cross(v1,v01);
	CPoint3D n2 = Cross(v2,v02);
	CPoint3D n3 = Cross(v3,v03);

	//if( Dot(n1, n2) >= 0 && Dot(n2,n3) >= 0 ) //应该相交的没有相交
	if( ( Dot(n1, n2) > 0.0 && Dot(n2, n3) > 0.0 )
		|| ( Length(n1) < 0.000001 && Dot(n2, n3) >= -0.000001 ) 
		|| ( Length(n2) < 0.000001 && Dot(n3, n1) >= -0.000001 )
		|| ( Length(n3) < 0.000001 && Dot(n1, n2) >= -0.000001 ))
	{
		sectPt = pi;
		return true;
	}

	return false;
}

bool SpotMath::IsIntersectAngle( const CPoint3D& ps1, const CPoint3D& ps2,
								const CPoint3D& pt1, const CPoint3D& pt2, 
								const CPoint3D& pt3)
{
	CPoint3D v0(ps2.getX() - ps1.getX(), 
		ps2.getY() - ps1.getY(), ps2.getZ() - ps1.getZ());
	CPoint3D ns = Normalize(v0);

	CPoint3D vn1(pt1.getX() - ps1.getX(), 
		pt1.getY() - ps1.getY(), pt1.getZ() - ps1.getZ());
	CPoint3D vn2(pt2.getX() - ps1.getX(), 
		pt2.getY() - ps1.getY(), pt2.getZ() - ps1.getZ());
	CPoint3D vn3(pt3.getX() - ps1.getX(), 
		pt3.getY() - ps1.getY(), pt3.getZ() - ps1.getZ());

	vn1 = Normalize(vn1);
	vn2 = Normalize(vn2);
	vn3 = Normalize(vn3);

	float a0  = Dot(vn1,vn2);
	float tmp = Dot(vn2,vn3);
	a0 = a0<tmp?a0:tmp;
	tmp = Dot(vn3,vn1);
	a0 = a0<tmp?a0:tmp;

	float a1 = Dot(vn1,ns);
	if( (a1+0.000001) < a0 )
	{
		return false;
	}

	a1 = Dot(vn2,ns);
	if( (a1+0.000001) < a0 )
	{
		return false;
	}

	a1 = Dot(vn3,ns);
	if( (a1+0.000001) < a0 )
	{
		return false;
	}
	return true;
}
CPoint3D SpotMath::Cross(const CPoint3D& v1, const CPoint3D& v2)
{
	float x = v1.getY() * v2.getZ() - v1.getZ() * v2.getY();
	float y = v1.getZ() * v2.getX() - v1.getX() * v2.getZ();
	float z = v1.getX() * v2.getY() - v1.getY() * v2.getX();

	return CPoint3D(x,y,z);
}

CPoint3D SpotMath::Normalize(const CPoint3D& v)
{
	if(Length(v)==0)
		return v;
	float f = 1.0f / Length(v);

	return CPoint3D(v.getX() * f ,v.getY() * f, v.getZ() * f);
}

float SpotMath::Length(const CPoint3D& v)
{
	return sqrt(
		v.getX() * v.getX() + v.getY() * v.getY() + v.getZ() * v.getZ());  
}

float SpotMath::Dot(const CPoint3D& v1, const CPoint3D& v2)
{
	return v1.getX() * v2.getX() + 
		v1.getY() * v2.getY() +
		v1.getZ() * v2.getZ();
}
float SpotMath::Dot(const vector<float>& v1, const vector<float>& v2)
{
	if (v1.size()!=v2.size())
		return 0;
	else
	{
		float res=0.0;
		for (int i = 0; i!=v1.size(); i++)
		{
			res += v1.at(i)*v2.at(i);
		}
		return res;
	}
}
float SpotMath::Dot(const vector<int>& v1, const vector<int>& v2)
{
	if (v1.size()!=v2.size())
		return 0;
	else
	{
		float res=0;
		for (int i = 0; i!=v1.size(); i++)
		{
			res += v1.at(i)*v2.at(i);
		}
		return res;
	}
}
float SpotMath::DotSelf(const vector<int>& v)
{
	float res=0;
	for (int i = 0; i!=v.size(); i++)
	{
		res += v.at(i)*v.at(i);
	}
	return res;
}
CPoint3D SpotMath::AvgCPoint3D(vector<CPoint3D>& vs)
{
	float x,y,z;
	x=y=z=0.0;
	float loopCount =0.0 ;
	for (std::vector<CPoint3D>::iterator i = vs.begin(); i!=vs.end(); i++)
	{
		x+= (*i).getX();
		y+= (*i).getY();
		z+= (*i).getZ();
		loopCount+= 1.0;
	}
	return CPoint3D(x/loopCount,y/loopCount,z/loopCount);

}
void SpotMath::Multiply(const float w, CPoint3D& v)
{
	v.setX( w*v.getX());
	v.setY( w*v.getY());
	v.setZ( w*v.getZ());
}
float SpotMath::DistSquare( const CPoint3D& pt1, const CPoint3D& pt2 )
{
	float dx = pt1.getX()-pt2.getX();
	float dy = pt1.getY()-pt2.getY();
	float dz = pt1.getZ()-pt2.getZ();
	return (dx*dx + dy*dy + dz*dz);
}
void SpotMath::regularizePoints(const CPoint3D& point, CPoint3D& reguPoint)
{
	float tmp = point.getX()*point.getX() + point.getY()*point.getY() + point.getZ()*point.getZ();
	if (tmp)
	{
		float tmp2 = sqrt(tmp);
		reguPoint = point;
		reguPoint/=tmp2;
	}
}