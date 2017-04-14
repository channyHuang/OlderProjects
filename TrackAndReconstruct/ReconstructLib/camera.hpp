
#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QObject>

#include <Eigen/Eigen>
using namespace Eigen;
#include <array>

#include "plane.hpp"
#include "ray.hpp"

typedef Eigen::Matrix<double, 3, 4> ProjMat;
typedef std::array<double, 5> LensDistortions;
typedef Eigen::Vector3d Response;
typedef std::vector<Response> Responses;

class Camera  {

public:
	Camera() {};
	Camera(QString id, QString name = QString());
	
public:
	QString id() const { return id_; }

	QString name() const { return name_; }
	void setName(QString name) {
		if(name_ != name) {
			name_ = name;
		}
	}
	
	const ProjMat & P() const { return P_; }
	const Eigen::Vector3d & t() const { return t_; }
	const Eigen::Vector3d & C() const { return C_; }
	const Eigen::Matrix3d & R() const { return R_; }
	const Eigen::Matrix3d & K() const { return K_; }

	const Eigen::Matrix3d & Kinv() const { return Kinv_; }
	const Eigen::Matrix3d & Rinv() const { return Rinv_; }
	
	void setP(const ProjMat &P);
	void sett(const Eigen::Vector3d &t);
	void setC(const Eigen::Vector3d &C);
	void setR(const Eigen::Matrix3d &R);
	void setK(const Eigen::Matrix3d &K);
	void set(const Eigen::Matrix3d &K, const Eigen::Matrix3d &R, const Eigen::Vector3d &t);

	const LensDistortions & lensDistortion() const { return lensDistortion_; }
	const Responses & response() const { return response_; }
	const Plane3d & plane() const { return plane_; }
	double refractiveIndex() const { return refractiveIndex_; }

	void setLensDistortion(const LensDistortions &distortion);
	void setResponse(const Responses &response);
	void setPlane(const Plane3d &plane);
	void setRefractiveIndex(double n);

	bool isRefractive() const { return isRefractive_; }
	bool isDistorted() const { return isDistorted_; }

public:

	bool project(Eigen::Vector3d &p) const;

	//! @see bool project(Eigen::Vector3d &)
	bool project(const Eigen::Vector3d &p, double &x, double &y) const {
		Eigen::Vector3d pp = p;
		bool ret = project(pp);
		x = pp[0];
		y = pp[1];
		return ret;
	}

	Ray3d unproject(const Eigen::Vector3d &p) const;
	
	//! @see bool unproject(Eigen::Vector3d &)
	Ray3d unproject(double x, double y) const {
		return unproject(Eigen::Vector3d(x, y, 1.0));
	}

	const Ray3d & principleRay() const { return principleRay_; }
	
	//! Take a 3D point (not vector) from global space to the camera's local space
	Eigen::Vector3d fromGlobalToLocal(const Eigen::Vector3d &p) const;

	//! Take a 3D point (not vector) from the camera's local space to global space
	Eigen::Vector3d fromLocalToGlobal(const Eigen::Vector3d &p) const;
	
	//! Take a 3D plane from global space to the camera's local space
	Plane3d fromGlobalToLocal(const Plane3d &p) const;

	//! Take a 3D plane from the camera's local space to global space
	Plane3d fromLocalToGlobal(const Plane3d &p) const;

	//! Take a 3D ray from global space to the camera's local space
	Ray3d fromGlobalToLocal(const Ray3d &p) const;

	//! Take a 3D ray from the camera's local space to global space
	Ray3d fromLocalToGlobal(const Ray3d &p) const;
	
private:
	//! Updates the projection matrix based on K, R, and t
	void updateProjection();

	//! Decomposes P into K, R, and t
	void updateOthers();
	
	//! Update the principle ray based on the current calibration
	void updatePrincipleRay();
	
private:
	QString id_;
	QString name_;
	
	ProjMat P_;         //! 3x4 projection matrix
	Eigen::Vector3d t_; //! translation vector
	Eigen::Vector3d C_; //! optical center [C = transpose(R) * -t]
	Eigen::Matrix3d R_; //! rotation matrix
	Eigen::Matrix3d K_; //! intrinsic matrix

	Eigen::Matrix3d Rinv_; //! inverse of rotation matrix, cached for efficiency
	Eigen::Matrix3d Kinv_; //! inverse of intrinsic matrix, cached for efficiency

	LensDistortions lensDistortion_;
	Responses response_;

	Plane3d plane_;           //! refractive plane separating camera from medium
	double refractiveIndex_;  //! refractive ratio of other medium and medium camera is in

	bool isRefractive_;  //! whether or not this view is behind a refractive interface
	bool isDistorted_;   //! whether or not there is a nonzero distortion coefficient
	Ray3d principleRay_; //! cached ray going through the camera's principle point
	
};

#endif // CAMERA_H
