#pragma once
#ifndef Scene_H
#define Scene_H

#include <vector>
#include <memory>
#include <string>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class Particle;
class MatrixStack;
class Program;
class Shape;
class Player;

class Scene
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Scene();
	virtual ~Scene();
	
	void load(const std::string &RESOURCE_DIR);
	void init();
	void tare();
	void reset();
	void step(bool keys[256]);
	
	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;
	void drawLines(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;
	
	double getTime() const { return t; }

	std::shared_ptr<Player> getPlayer();
	
private:
	double t;
	double h;
	Eigen::Vector3d grav;

	std::shared_ptr<Shape> sphere, cube;
	std::shared_ptr<Player> player;
	
	std::vector< std::shared_ptr<Particle> > spheres;
};

#endif
