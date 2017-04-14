#pragma once
#include "MathAction.h"
#include "SpotMath.h"
#include <cmath>
//const float h = 0.5859375;//300/512
//const float half_h = 0.29296875;//h/2

using namespace std;

class CPoint3D
{
public:
	CPoint3D(void);
	~CPoint3D(void);
	CPoint3D(float a,float b,float c){m_vPos[0]=a; m_vPos[1]=b; m_vPos[2]=c;}

protected:
	float m_vPos[3];
public:
	static float h;
	static float half_h;
	CPoint3D& operator=(const CPoint3D& rp)
	{
		for(int i=0;i<3;i++)
			m_vPos[i] = rp.m_vPos[i];
		return *this;
	}
	CPoint3D& operator+=(const CPoint3D& rhs)
	{
		for(int i=0;i<3;i++)
			m_vPos[i] += rhs.m_vPos[i];
		return *this;
	}
	CPoint3D& operator-=(const CPoint3D& rhs)
	{
		for(int i=0;i<3;i++)
			m_vPos[i] -= rhs.m_vPos[i];
		return *this;
	}
	CPoint3D& operator*=(const float d)
	{
		for(int i=0;i<3;i++)
			m_vPos[i] *= d;
		return *this;
	}
	CPoint3D& operator/=(const float d)
	{
		if (d!=0.0)
		{
			for(int i=0;i<3;i++)
				m_vPos[i] /= d;
		}
		return *this;
	}
	friend  float simpleDist(const CPoint3D& lhs,const CPoint3D& rhs)
	{
		return fabs(lhs.getX()-rhs.getX())+fabs(lhs.getY()-rhs.getY()) + fabs(lhs.getZ()-rhs.getZ());
	}
	friend  float RealDist(const CPoint3D& lhs,const CPoint3D& rhs)
	{
		float dx= lhs.getX()-rhs.getX();
		float dy= lhs.getY()-rhs.getY();
		float dz= lhs.getZ()-rhs.getZ();
		return sqrtf(dx*dx+dy*dy+dz*dz);
	}
	friend  float L2Dist(const CPoint3D& lhs,const CPoint3D& rhs)
	{
		return sqrtf(
			(lhs.getX()-rhs.getX())*(lhs.getX()-rhs.getX())+
			(lhs.getY()-rhs.getY())*(lhs.getY()-rhs.getY())+
			(lhs.getZ()-rhs.getZ())*(lhs.getZ()-rhs.getZ()));
	}
	static bool Equalfloat(float a,float b)
	{
		float ab = a-b;
		ab = ab>0?ab:-ab;
		if ( ab< half_h )
			return true;
		else
			return false;
	}
	friend bool operator==(const CPoint3D& lhs, const CPoint3D& rhs)
	{
		if ( Equalfloat(lhs.getX(),rhs.getX()) && Equalfloat(lhs.getY(),rhs.getY()) && Equalfloat(lhs.getZ(),rhs.getZ()) )
			return true;
		else
			return false;
	}
	friend bool operator< (const CPoint3D& lhs, const CPoint3D& rhs)
	{
		if (Equalfloat(lhs.getX(),rhs.getX()))
		{
			if (Equalfloat(lhs.getY(),rhs.getY()))
			{
				if (Equalfloat(lhs.getZ(),rhs.getZ()))
					return false;
				else if (lhs.getZ()>rhs.getZ())
					return false;
				else
					return true;
			}
			else if (lhs.getY()>rhs.getY())
				return false;
			else
				return true;
		}
		else if (lhs.getX()>rhs.getX())
			return false;
		else
			return true;
	}
	friend CPoint3D operator-(const CPoint3D& lhs, const CPoint3D& rhs)
	{
		CPoint3D res;
		res.setXYZ(lhs.getX() - rhs.getX(),
			lhs.getY() - rhs.getY(),lhs.getZ() - rhs.getZ());
		return res;
	}
	friend CPoint3D operator*(const float factor, const CPoint3D& rhs)
	{
		CPoint3D res;
		res.setXYZ(factor*rhs.getX(),factor*rhs.getY(),factor*rhs.getZ());
		return res;
	}
	CPoint3D& DevideNumbers(const float a, const float b, const float c)
	{
		if (a!=0 && b!=0 && c!=0)
		{
			m_vPos[0] /= a;
			m_vPos[1] /= b;
			m_vPos[2] /= c;
		}
		return *this;
	}
	CPoint3D& MultiplyNumbers(const float a, const float b, const float c)
	{
		m_vPos[0] *= a;
		m_vPos[1] *= b;
		m_vPos[2] *= c;
		return *this;
	}

	float getX() const{return m_vPos[0];}
	float getY() const{return m_vPos[1];}
	float getZ() const{return m_vPos[2];}

	void setX(const float input){m_vPos[0]=input;}
	void setY(const float input){m_vPos[1]=input;}
	void setZ(const float input){m_vPos[2]=input;}

	void setXYZ(const float x,const float y,const float z){
		m_vPos[0] = x;
		m_vPos[1] = y;
		m_vPos[2] = z;
	}
};


