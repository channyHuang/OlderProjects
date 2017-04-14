#pragma once

#include "Point3D.h"

class CPoint3D;



class SpotMath
{
private:
	SpotMath(void);

public:
	static bool IsIntersect( const CPoint3D& ps1, const CPoint3D& ps2,
		const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3, CPoint3D& sectPt);

	static bool IsIntersectAngle( const CPoint3D& ps1, const CPoint3D& ps2,
		const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3);

	static CPoint3D Cross(const CPoint3D& v1, const CPoint3D& v2);

	static CPoint3D Normalize(const CPoint3D& v);

	static float Length(const CPoint3D& v);

	static float Dot(const CPoint3D& v1, const CPoint3D& v2);

	static float Dot(const vector<float>& v1, const vector<float>& v2);

	static float Dot(const vector<int>& v1, const vector<int>& v2);

	static float DotSelf(const vector<int>& v);

	static CPoint3D AvgCPoint3D( vector<CPoint3D>& vs);

	static void Multiply(const float w, CPoint3D& v);

	static float DistSquare( const CPoint3D& pt1, const CPoint3D& pt2 );

	//(值为归一过的index)线段[PS1, PS2] 是否和正方形面片有交点。
	//面片在x=xFace的平面上，y最小为ymin，y最大为ymax，z最小为zmin，z最大为zmax
	static bool IsIntersectPatchX(const CPoint3D& ps1, const CPoint3D& ps2, const int xFace,
		const int ymin, const int ymax, const int zmin, const int zmax);

	//面片在y=yFace的平面上，x最小为xmin，x最大为xmax，z最小为zmin，z最大为zmax
	static bool IsIntersectPatchY(const CPoint3D& ps1, const CPoint3D& ps2, const int yFace,
		const int xmin, const int xmax, const int zmin, const int zmax);

	//面片在z=zFace的平面上，x最小为xmin，x最大为xmax，y最小为ymin，y最大为ymax
	static bool IsIntersectPatchZ(const CPoint3D& ps1, const CPoint3D& ps2, const int zFace,
		const int xmin, const int xmax, const int ymin, const int ymax);

	static void regularizePoints(const CPoint3D& point, CPoint3D& reguPoint);
};
