#pragma once
#ifndef Building_H
#define Building_H

#include "Player.h"

class Building {
public:
	Building(double x0, double y0, double width, double height, double depth,
		const std::shared_ptr<Shape> cube, const std::shared_ptr<Shape> sphere,
		bool isForeground=false);
	~Building();

	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const;

	bool collide(Ray ray);

	std::shared_ptr<Polygon> boundingBox;

	bool isForeground() { return _isForeground; }
	Eigen::Vector3f color() { return _color; }
	void setColor(Eigen::Vector3f color) { _color = color; }

private:
	std::shared_ptr<Shape> cube;
	std::shared_ptr<Texture> texture;

	double x0, y0, width, height, depth;
	Eigen::Vector3f _color;

	bool _isForeground;
};

#endif