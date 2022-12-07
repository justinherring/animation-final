#pragma once
#ifndef  Ray_H
#define Ray_H

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class Ray {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Ray(Eigen::Vector3d x, Eigen::Vector3d d) : x(x), d(d) {}
	
	Eigen::Vector3d x; // initial position
	Eigen::Vector3d d; // direction
};

#endif 
