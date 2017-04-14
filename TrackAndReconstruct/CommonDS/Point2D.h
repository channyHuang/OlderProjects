#pragma once
#include "MathAction.h"
#include "SpotMath.h"

using namespace std;

class CPoint2D
{
public:
	CPoint2D(void);
	CPoint2D(short a,short b){m_vPos[0]=a; m_vPos[1]=b;}
	~CPoint2D(void);
private:
	short m_vPos[2];
public:
	CPoint2D& operator=(const CPoint2D& rp)
	{
		for(int i=0;i<2;i++)
			m_vPos[i] = rp.m_vPos[i];
		return *this;
	}
	short getX(){return m_vPos[0];}
	short getY(){return m_vPos[1];}
	void setX(short input){m_vPos[0]=input;}
	void setY(short input){m_vPos[1]=input;}
};