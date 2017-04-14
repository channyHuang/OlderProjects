
//---------------------------------------------------------------------
#ifndef PLANE_HPP
#define PLANE_HPP

#include <Eigen/Eigen>

//! Three-dimensional plane, using double precision.
class Plane3d {
public:
	typedef Eigen::Vector3d Point;
	typedef Eigen::Vector3d Vector;

public:
	Plane3d() : normal_(0, 0, 1), dist_(0) { }
	Plane3d(const Vector &normal, double d) : normal_(normal.normalized()), dist_(d) { }
	Plane3d(const Vector &normal, const Point &x0) : normal_(normal.normalized()) , dist_(normal_.dot(x0)) { }

public:
	void setNormal(const Vector &normal) { normal_ = normal; normal_.normalize(); }
	void setDistance(double dist)        { dist_ = dist; }

	const Vector & normal() const { return normal_; }
	double distance()       const { return dist_; }
	Point x0()              const { return dist_*normal_; }

private:
	Vector normal_;
	double dist_;
};

inline bool operator==(const Plane3d &a, const Plane3d &b) {
	return (a.normal() - b.normal()).squaredNorm() + fabs(a.distance() - b.distance()) < 1e-10;
}

inline bool operator!=(const Plane3d &a, const Plane3d &b) {
	return !(a == b);
}

#endif // PLANE_HPP
