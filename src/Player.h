#pragma once
#ifndef Player_H
#define Player_H

#include "Web.h"
#include "Texture.h"
#include "Polygon.h"

class Player {
public:
	Player(double x0, double y0, double width, double height, 
		const std::shared_ptr<Shape> cube, const std::shared_ptr<Shape> sphere);
	~Player();

	void move(Eigen::Vector2d direction);
	Ray shootWeb(Eigen::Vector2d position);
	void removeWeb();

	void reset();
	void step(double h, Eigen::Vector3d grav, bool keys[256]);

	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;
	void drawBoundingBox(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;
	void drawWebs() const;

	Eigen::Vector3d position() const;

private:
	std::shared_ptr<Shape> cube;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Polygon> boundingBox;

	double x0, y0, width, height;
	Ray web;
	std::shared_ptr<Particle> webTarget;
	std::shared_ptr<Particle> center;

	void createWeb(std::shared_ptr<Particle> target);
	void removeBBWeb(std::shared_ptr<Particle> oldTarget);
};

#endif