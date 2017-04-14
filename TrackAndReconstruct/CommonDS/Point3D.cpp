#include "Point3D.h"

float CPoint3D::h =  0.5859375;
float CPoint3D::half_h = 0.29296875;
CPoint3D::CPoint3D()
{
	for (int i=0;i<3;i++)
		m_vPos[i] = 0;
}

CPoint3D::~CPoint3D(void)
{
}
