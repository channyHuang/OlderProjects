#include "camera.hpp"

#include "linalg.hpp"

#include <gsl/gsl_poly.h>
#include <gsl/gsl_errno.h>
//! Check if a floating point value is 0.
template <typename T>
bool iszero(const T &x, const T eps = 1e-10) { return (x <= eps && x >= -eps); }

void gsl_error_handler(const char * /*reason*/, const char * /*file*/, int /*line*/, int /*gsl_errno*/) {
	// DO NOTHING
}
//! Find the roots of ax^4 + bx^3 + cx^2 + dx + e = 0.
void findRoots(
	double a, double b, double c, double d, double e,
	double &r1, double &r2, double &r3, double &r4)
{
	const double coeffs[] = { e, d, c, b, a };
	double roots[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	// TODO factor some of this stuff out so that we're not recreating it
	//      constantly. Perhaps a root finder class?
	gsl_set_error_handler(&gsl_error_handler);
	gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(5);
	gsl_poly_complex_solve(coeffs, 5, w, roots);
	gsl_poly_complex_workspace_free(w);

	r1 = (iszero(roots[1]) ? roots[0] : std::numeric_limits<double>::quiet_NaN());
	r2 = (iszero(roots[3]) ? roots[2] : std::numeric_limits<double>::quiet_NaN());
	r3 = (iszero(roots[5]) ? roots[4] : std::numeric_limits<double>::quiet_NaN());
	r4 = (iszero(roots[7]) ? roots[6] : std::numeric_limits<double>::quiet_NaN());
}

bool projectRefraction(Vector3d &p, const Plane3d &P, double n) {
	//
	const Vector3d proj = linalg::project(p, P.normal());
	const double y = (p - proj).y();
	const double z = proj.norm();
	const double r = (p - proj).norm();
	const double d = P.distance();
	const double rr = r*r, nn = n*n, dd = d*d;

	// Note that the below modifies p
	Vector3d dir = p - proj;
	dir.normalize();

	// Find roots of quartic
	double roots[4];
	findRoots(
		nn - 1,
		-2 * r*(nn - 1),
		rr*(nn - 1) + dd*nn - (z - d)*(z - d),
		-2 * dd*nn*r,
		dd*nn*rr,
		roots[0], roots[1], roots[2], roots[3]);

	// Find the root that makes sense
	for (int index = 0; index < 4; ++index) {
		if (!std::isnan(roots[index])) {
			const Vector3d pp = roots[index] * dir;
			const double py = pp.y();
			if (py > -1e-3 && y > -1e-3) {
				if (py < y + 1e-3) {
					p = pp + P.x0();
					return true;
				}
			}
			else if (py < 1e-3 && y < 1e-3) {
				if (y < py + 1e-3) {
					p = pp + P.x0();
					return true;
				}
			}
		}
	}

	return false;
}
//---------------------------------------------------------------------
// Gram-Schmidt orthonormalization.
//
void orthonormalize(Matrix3d &mat) {
	for (int i = 0; i < 3; ++i) {
		Vector3d accum = Vector3d::Zero();
		for (int j = 0; j < i; ++j) {
			// accum += projection(vectors[i], vectors[j])
			Vector3d vi = mat.col(i);
			Vector3d vj = mat.col(j);

			double scale = vi.dot(vj) / vj.squaredNorm();
			vj *= scale;
			accum += vj;
		}

		mat.col(i) -= accum;
		mat.col(i).normalize();
	}

	// Convert really tiny numbers to zero
	for (int i = 0; i < 3; ++i)
	for (int j = 0; j < 3; ++j)
	if (-1e-10 < mat(i, j) && mat(i, j) < 1e-10)
		mat(i, j) = 0.0;
}

//---------------------------------------------------------------------

Camera::Camera(QString id, QString name)
: id_(id)
, name_(name.isNull() ? "<no name>" : name)
, P_(ProjMat::Zero())
, t_(Vector3d::Zero())
, C_(Vector3d::Zero())
, R_(Matrix3d::Identity())
, K_(Matrix3d::Identity())
, Rinv_(Matrix3d::Identity())
, Kinv_(Matrix3d::Identity())
, refractiveIndex_(1.0)
, isRefractive_(false)
, isDistorted_(false)
{
	lensDistortion_.fill(0.0f);
	P_.topLeftCorner<3, 3>() = Matrix3d::Identity();
}

//---------------------------------------------------------------------


void Camera::setP(const ProjMat &P) {
	P_ = P;
	updateOthers();
}

void Camera::setR(const Eigen::Matrix3d &R) {
	R_ = R;
	orthonormalize(R_);
	Rinv_ = R_.transpose();
	updateProjection();
}

void Camera::sett(const Eigen::Vector3d &t) {
	t_ = t;
	C_ = Rinv_ * -t;
	updateProjection();
}

void Camera::setC(const Eigen::Vector3d &C) {
	C_ = C;
	t_ = R_ * -C;
	updateProjection();
}

void Camera::setK(const Eigen::Matrix3d &K) {
	K_ = K;
	Kinv_ = K_.inverse();
	updateProjection();
}

void Camera::set(const Eigen::Matrix3d &K, const Eigen::Matrix3d &R, const Eigen::Vector3d &t) {
	K_ = K;
	R_ = R;
	t_ = t;

	orthonormalize(R_);

	Kinv_ = K_.inverse();
	Rinv_ = R_.transpose();
	C_ = Rinv_ * -t;

	updateProjection();
}

//---------------------------------------------------------------------

void Camera::updateProjection() {
	P_.topLeftCorner<3, 3>() = R_;
	P_.col(3) = t_;
	P_ = K_ * P_;
	updatePrincipleRay();
}

void Camera::updateOthers() {
	P_ /= P_.row(2).head<3>().squaredNorm();

	// RQ Factorization
	Matrix3d M = P_.topLeftCorner<3, 3>();
	Matrix3d reverseRows;
	reverseRows << 0, 0, 1, 0, 1, 0, 1, 0, 0;

	const Eigen::Matrix3d qrMatrix = (reverseRows * M).transpose();
	const Eigen::HouseholderQR<Matrix3d> qrDecomp(qrMatrix);
	const Matrix3d matrixQ = qrDecomp.householderQ();
	const Matrix3d matrixR = qrDecomp.matrixQR().triangularView<Upper>();

	R_ = reverseRows * matrixQ.transpose();
	K_ = reverseRows * matrixR.transpose() * reverseRows;

	// Eigen doesn't require diagonal elements of R (in the QR
	// decomposition) to be positive, so correct for this since the
	// intrinsic matrix should have positive diagonal elements
	for (int axis = 2; axis >= 0; --axis) {
		if (K_(axis, axis) < 0) {
			K_(axis, axis) = -(K_(axis, axis));
			R_.row(axis) = -(R_.row(axis));
		}

		if (K_(axis, 2) < 0)
			K_(axis, 2) = -K_(axis, 2);
	}

	orthonormalize(R_);

	Kinv_ = K_.inverse();
	Rinv_ = R_.transpose();
	t_ = Kinv_ * P_.col(3);
	C_ = -Rinv_ * t_;

	updatePrincipleRay();
}

void Camera::updatePrincipleRay() {
	// XXX Does lens distortion play a role here...?
	const Eigen::Vector3d tcol = K_.col(2);
	const Eigen::Vector3d dir = Kinv_ * (tcol / tcol[2]);
	principleRay_.setSource(C_);
	principleRay_.setDirection(Rinv_ * dir.normalized());
}

//---------------------------------------------------------------------

void Camera::setLensDistortion(const LensDistortions &distortion) {
	if (distortion != lensDistortion_) {
		lensDistortion_ = distortion;
		isDistorted_ = !iszero(distortion[0])
			|| !iszero(distortion[1])
			|| !iszero(distortion[2])
			|| !iszero(distortion[3])
			|| !iszero(distortion[4]);
	}
}

//---------------------------------------------------------------------

void Camera::setResponse(const Responses &response) {
	if (response_ != response) {
		response_ = response;
	}
}

//---------------------------------------------------------------------

void Camera::setPlane(const Plane3d &plane) {
	if (plane_ != plane) {
		this->plane_ = plane;
		this->isRefractive_ = (!iszero(refractiveIndex_ - 1) && !iszero(plane_.distance()));
	}
}

//---------------------------------------------------------------------

void Camera::setRefractiveIndex(double n) {
	if (fabs(n - refractiveIndex_) > 1e-10) {
		refractiveIndex_ = n;
		isRefractive_ = (!iszero(refractiveIndex_ - 1) && !iszero(plane().distance()));
	}
}
//---------------------------------------------------------------------

Eigen::Vector3d Camera::fromGlobalToLocal(const Eigen::Vector3d &p) const {
	return (R_*p + t_);
}

Eigen::Vector3d Camera::fromLocalToGlobal(const Eigen::Vector3d &p) const {
	return Rinv_*(p - t_);
}

Plane3d Camera::fromGlobalToLocal(const Plane3d &p) const {
	Plane3d::Vector norm = R_*p.normal();
	return Plane3d(norm, p.distance());
}

Plane3d Camera::fromLocalToGlobal(const Plane3d &p) const {
	Plane3d::Vector norm = Rinv_*p.normal();
	return Plane3d(norm, p.distance());
}

Ray3d Camera::fromGlobalToLocal(const Ray3d &r) const {
	Ray3d::Vector direction = R_*r.direction();
	Ray3d::Point source = fromGlobalToLocal(r.source());
	return Ray3d(source, direction);
}

Ray3d Camera::fromLocalToGlobal(const Ray3d &r) const {
	Ray3d::Vector direction = Rinv_*r.direction();
	Ray3d::Point source = fromLocalToGlobal(r.source());
	return Ray3d(source, direction);
}

//---------------------------------------------------------------------

bool Camera::project(Eigen::Vector3d &p) const {
	// Place point in basis formed by this camera;
	Vector3d point = fromGlobalToLocal(p);

	// Refractive projection, if necessary
	if (isRefractive_) {
		if (!projectRefraction(point, plane_, refractiveIndex_)) {
			p[0] = p[1] = p[2] = std::numeric_limits<double>::quiet_NaN();
			return false;
		}
	}

	p = K_*point;
	p /= p.z();

	if (isDistorted_) {
		const double cx = K_(0, 2);
		const double cy = K_(1, 2);
		const double fx = K_(0, 0);
		const double fy = K_(1, 1);

		double &x = p[0];
		double &y = p[1];
		x = (x - cx) / fx;
		y = (y - cy) / fy;
		{
			// OpenCV model for distortion
			const LensDistortions &k = lensDistortion_;
			const double r2 = x*x + y*y;
			const double cdist = 1 + ((k[4] * r2 + k[1])*r2 + k[0])*r2;

			x = x*cdist + 2 * k[2] * x*y + k[3] * (r2 + 2 * x*x);
			y = y*cdist + k[2] * (r2 + 2 * y*y) + 2 * k[3] * x*y;
		}
		x = fx*x + cx;
		y = fy*y + cy;
	}
	
	return true;
}

//---------------------------------------------------------------------

Ray3d Camera::unproject(const Eigen::Vector3d &p) const {
	// Remove distortion, if any exists
	Eigen::Vector3d pp = p;
	if (isDistorted_) {
		// NOTE This code is based off of OpenCV's undistortPoints function
		const double cx = K_(0, 2);
		const double cy = K_(1, 2);
		const double ifx = 1.0 / K_(0, 0);
		const double ify = 1.0 / K_(1, 1);

		double &x = pp[0];
		double &y = pp[1];
		const double x0 = x = (x - cx)*ifx;
		const double y0 = y = (y - cy)*ify;

		const LensDistortions &k = lensDistortion_;
		for (int j = 0; j < 5; j++) {
			const double r2 = x*x + y*y;
			const double icdist = 1.0 / (1 + ((k[4] * r2 + k[1])*r2 + k[0])*r2);
			const double deltaX = 2 * k[2] * x*y + k[3] * (r2 + 2 * x*x);
			const double deltaY = k[2] * (r2 + 2 * y*y) + 2 * k[3] * x*y;
			x = (x0 - deltaX)*icdist;
			y = (y0 - deltaY)*icdist;
		}

		x /= ifx;  y /= ify;
		x += cx;   y += cy;
	}

	// Cast initial ray using inverse of camera's intrinsic matrix and
	// refract, if necessary
	Ray3d ray(Ray3d::Point::Zero(), Kinv_ * pp);
	if (isRefractive_)
		refract(ray, plane_, refractiveIndex_, ray);

	return fromLocalToGlobal(ray);
}

//---------------------------------------------------------------------
