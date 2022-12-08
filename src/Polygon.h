#pragma once
#ifndef Polygon_H
#define Polygon_H

#include <vector>
#include <memory>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "Spring.h"
#include "Particle.h"

class Shape;
class Program;
class MatrixStack;
class Particle;

class Polygon
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Polygon(double x0, double y0, double width, double height, bool fixed=true);
	Polygon(double x0, double y0, double width, double height, const std::shared_ptr<Shape> shape, bool fixed=true);
	virtual ~Polygon();
	void tare();
	void reset();
	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;

	bool collide(std::shared_ptr<Particle> v);
	bool collide(std::shared_ptr<Polygon> p);
	void step(double h, Eigen::Vector3d grav, Eigen::Vector3d movement);

	std::vector<std::shared_ptr<Particle> > vertices;
	std::vector<std::shared_ptr<Spring> > springs;

	double x0, y0, width, height;

	std::shared_ptr<Spring> createSpring(const std::shared_ptr<Particle> p0, const std::shared_ptr<Particle> p1, double E)
	{
		auto s = std::make_shared<Spring>(p0, p1);
		s->E = E;
		Eigen::Vector3d x0 = p0->x;
		Eigen::Vector3d x1 = p1->x;
		Eigen::Vector3d dx = x1 - x0;
		s->L = dx.norm();
		return s;
	}

private:
	const std::shared_ptr<Shape> sphere;
	void createRectangle(double x0, double y0, double width, double height, const std::shared_ptr<Shape> shape, bool fixed=true);

	Eigen::VectorXd v;
	Eigen::VectorXd f;
	Eigen::MatrixXd M;
	Eigen::MatrixXd K;

	double x_original;
	double y_original;
};

#endif
