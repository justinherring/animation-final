#pragma once
#ifndef Polygon_H
#define Polygon_H

#include <vector>
#include <memory>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class Shape;
class Program;
class MatrixStack;
class Particle;

class Polygon
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Polygon(double x0, double y0, double width, double height);
	Polygon(double x0, double y0, double width, double height, const std::shared_ptr<Shape> shape);
	virtual ~Polygon();
	void tare();
	void reset();
	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;

	bool collide(std::shared_ptr<Particle> v);
	bool collide(std::shared_ptr<Polygon> p);
	void step(double h, Eigen::Vector3d grav, Eigen::Vector3d movement);

	std::vector<std::shared_ptr<Particle> > vertices;

	double x0, y0, width, height;

private:
	const std::shared_ptr<Shape> sphere;
};

#endif
