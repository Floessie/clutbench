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

#include <xmmintrin.h>

#include "IntegerClutMethod.hpp"

namespace
{

	inline void posToIndex(unsigned int pos, size_t (&index)[2])
	{
		index[0] = static_cast<size_t>(pos) * 4;
		index[1] = static_cast<size_t>(pos + 1) * 4;
	}

}

IntegerClutMethod::IntegerClutMethod() :
	clut_image(0)
{
}

IntegerClutMethod::~IntegerClutMethod()
{
	_mm_free(clut_image);
}

const char* IntegerClutMethod::getDescription() const
{
	return "4 * 16b integer clut storage (8B per pixel instead of 12B)";
}

const char* IntegerClutMethod::getFilename() const
{
	return "integer";
}

void IntegerClutMethod::setClut(const Image& image, unsigned int level)
{
	_mm_free(clut_image);
	const size_t size = image.getWidth() * image.getHeight();
	clut_image = reinterpret_cast<unsigned short*>(_mm_malloc(size * 4 * sizeof(unsigned short), 4 * sizeof(unsigned short)));
	size_t index = 0;
	for (unsigned int y = 0; y < image.getHeight(); ++y) {
		for (unsigned int x = 0; x < image.getWidth(); ++x) {
			clut_image[index] = image.getR(x, y);
			++index;
			clut_image[index] = image.getG(x, y);
			++index;
			clut_image[index] = image.getB(x, y);
			index += 2;
		}
	}

	clut_level = level * level;
	flevel_minus_one = static_cast<float>(clut_level - 1) / 65535.0f;
	flevel_minus_two = static_cast<float>(clut_level - 2);
}

void IntegerClutMethod::convert(float* rgb) const
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

	size_t index[2];
	posToIndex(color, index);

	float tmp1[4] __attribute__((aligned(16)));
	tmp1[0] = clut_image[index[0]] * (1 - r) + clut_image[index[1]] * r;
	tmp1[1] = clut_image[index[0] + 1] * (1 - r) + clut_image[index[1] + 1] * r;
	tmp1[2] = clut_image[index[0] + 2] * (1 - r) + clut_image[index[1] + 2] * r;

	posToIndex(color + level, index);

	float tmp2[4] __attribute__((aligned(16)));
	tmp2[0] = clut_image[index[0]] * (1 - r) + clut_image[index[1]] * r;
	tmp2[1] = clut_image[index[0] + 1] * (1 - r) + clut_image[index[1] + 1] * r;
	tmp2[2] = clut_image[index[0] + 2] * (1 - r) + clut_image[index[1] + 2] * r;

	float out[4] __attribute__((aligned(16)));
	out[0] = tmp1[0] * (1 - g) + tmp2[0] * g;
	out[1] = tmp1[1] * (1 - g) + tmp2[1] * g;
	out[2] = tmp1[2] * (1 - g) + tmp2[2] * g;

	posToIndex(color + level_square, index);

	tmp1[0] = clut_image[index[0]] * (1 - r) + clut_image[index[1]] * r;
	tmp1[1] = clut_image[index[0] + 1] * (1 - r) + clut_image[index[1] + 1] * r;
	tmp1[2] = clut_image[index[0] + 2] * (1 - r) + clut_image[index[1] + 2] * r;

	posToIndex(color + level + level_square, index);

	tmp2[0] = clut_image[index[0]] * (1 - r) + clut_image[index[1]] * r;
	tmp2[1] = clut_image[index[0] + 1] * (1 - r) + clut_image[index[1] + 1] * r;
	tmp2[2] = clut_image[index[0] + 2] * (1 - r) + clut_image[index[1] + 2] * r;

	tmp1[0] = tmp1[0] * (1 - g) + tmp2[0] * g;
	tmp1[1] = tmp1[1] * (1 - g) + tmp2[1] * g;
	tmp1[2] = tmp1[2] * (1 - g) + tmp2[2] * g;

	rgb[0] = out[0] * (1 - b) + tmp1[0] * b;
	rgb[1] = out[1] * (1 - b) + tmp1[1] * b;
	rgb[2] = out[2] * (1 - b) + tmp1[2] * b;
}
