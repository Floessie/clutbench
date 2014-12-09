/*
 * This file is part of clutbench.
 *
 * Copyright (c) 2014 Flössie <floessie.mail@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <algorithm>

#include "OptimizedClutMethod.hpp"

namespace
{

	inline void posToXy(unsigned int pos, unsigned int width, unsigned int (&x)[2], unsigned int (&y)[2])
	{
		x[0] = pos % width;
		y[0] = pos / width;
		x[1] = (pos + 1) % width;
		y[1] = (pos + 1) / width;
	}

}

const char* OptimizedClutMethod::getDescription() const
{
	return "Optimized and cleaned up code";
}

const char* OptimizedClutMethod::getFilename() const
{
	return "optimized";
}

void OptimizedClutMethod::setClut(const Image& image, unsigned int level)
{
	clut_image = image;
	clut_level = level * level;
	flevel_minus_one = static_cast<float>(clut_level - 1) / 65535.0f;
	flevel_minus_two = static_cast<float>(clut_level - 2);
}

void OptimizedClutMethod::convert(float* rgb) const
{
	const unsigned int level = clut_level; // This is important

	const unsigned int red = std::min(flevel_minus_two, rgb[0] * flevel_minus_one);
	const unsigned int green = std::min(flevel_minus_two, rgb[1] * flevel_minus_one);
	const unsigned int blue = std::min(flevel_minus_two, rgb[2] * flevel_minus_one);

	const float r = rgb[0] * flevel_minus_one - red;
	const float g = rgb[1] * flevel_minus_one - green;
	const float b = rgb[2] * flevel_minus_one - blue;

	const unsigned int level_square = level * level;

	const unsigned int color = red + green * level + blue * level_square;

	unsigned int x[2];
	unsigned int y[2];
	posToXy(color, clut_image.getWidth(), x, y);

	float tmp1[4] __attribute__((aligned(16)));
	tmp1[0] = clut_image.getR(x[0], y[0]) * (1 - r) + clut_image.getR(x[1], y[1]) * r;
	tmp1[1] = clut_image.getG(x[0], y[0]) * (1 - r) + clut_image.getG(x[1], y[1]) * r;
	tmp1[2] = clut_image.getB(x[0], y[0]) * (1 - r) + clut_image.getB(x[1], y[1]) * r;

	posToXy(color + level, clut_image.getWidth(), x, y);

	float tmp2[4] __attribute__((aligned(16)));
	tmp2[0] = clut_image.getR(x[0], y[0]) * (1 - r) + clut_image.getR(x[1], y[1]) * r;
	tmp2[1] = clut_image.getG(x[0], y[0]) * (1 - r) + clut_image.getG(x[1], y[1]) * r;
	tmp2[2] = clut_image.getB(x[0], y[0]) * (1 - r) + clut_image.getB(x[1], y[1]) * r;

	float out[4] __attribute__((aligned(16)));
	out[0] = tmp1[0] * (1 - g) + tmp2[0] * g;
	out[1] = tmp1[1] * (1 - g) + tmp2[1] * g;
	out[2] = tmp1[2] * (1 - g) + tmp2[2] * g;

	posToXy(color + level_square, clut_image.getWidth(), x, y);

	tmp1[0] = clut_image.getR(x[0], y[0]) * (1 - r) + clut_image.getR(x[1], y[1]) * r;
	tmp1[1] = clut_image.getG(x[0], y[0]) * (1 - r) + clut_image.getG(x[1], y[1]) * r;
	tmp1[2] = clut_image.getB(x[0], y[0]) * (1 - r) + clut_image.getB(x[1], y[1]) * r;

	posToXy(color + level + level_square, clut_image.getWidth(), x, y);

	tmp2[0] = clut_image.getR(x[0], y[0]) * (1 - r) + clut_image.getR(x[1], y[1]) * r;
	tmp2[1] = clut_image.getG(x[0], y[0]) * (1 - r) + clut_image.getG(x[1], y[1]) * r;
	tmp2[2] = clut_image.getB(x[0], y[0]) * (1 - r) + clut_image.getB(x[1], y[1]) * r;

	tmp1[0] = tmp1[0] * (1 - g) + tmp2[0] * g;
	tmp1[1] = tmp1[1] * (1 - g) + tmp2[1] * g;
	tmp1[2] = tmp1[2] * (1 - g) + tmp2[2] * g;

	rgb[0] = out[0] * (1 - b) + tmp1[0] * b;
	rgb[1] = out[1] * (1 - b) + tmp1[1] * b;
	rgb[2] = out[2] * (1 - b) + tmp1[2] * b;
}
