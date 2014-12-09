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

#include "Image.hpp"

namespace
{

	float get(const float* plane, unsigned int width, unsigned int height, unsigned int x, unsigned int y)
	{
		if (x < width && y < height) {
			const size_t index = static_cast<size_t>(width) * y + x;
			return plane[index];
		}
		return 0.0f;
	}

	void set(float* plane, unsigned int width, unsigned int height, unsigned int x, unsigned int y, float value)
	{
		if (x < width && y < height) {
			const size_t index = static_cast<size_t>(width) * y + x;
			plane[index] = std::max(0.0f, std::min(65535.0f, value));
		}
	}

}

Image::Image() :
	width(0),
	height(0),
	red(0),
	green(0),
	blue(0)
{
}

Image::~Image()
{
	_mm_free(red);
	_mm_free(green);
	_mm_free(blue);
}

Image::Image(const Image& other) :
	width(other.width),
	height(other.height)
{
	const size_t size = static_cast<size_t>(width) * height;

	red = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
	green = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
	blue = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));

	std::copy(other.red, other.red + size, red);
	std::copy(other.green, other.green + size, green);
	std::copy(other.blue, other.blue + size, blue);
}

Image& Image::operator =(const Image& other)
{
	if (this != &other) {
		_mm_free(red);
		_mm_free(green);
		_mm_free(blue);

		width = other.width;
		height = other.height;

		const size_t size = static_cast<size_t>(width) * height;

		red = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
		green = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
		blue = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));

		std::copy(other.red, other.red + size, red);
		std::copy(other.green, other.green + size, green);
		std::copy(other.blue, other.blue + size, blue);
	}
	return *this;
}

void Image::clearAndInitialize(unsigned int width, unsigned int height)
{
	_mm_free(red);
	_mm_free(green);
	_mm_free(blue);

	this->width = width;
	this->height = height;

	const size_t size = static_cast<size_t>(width) * height;

	red = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
	green = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));
	blue = reinterpret_cast<float*>(_mm_malloc(size * sizeof(float), 4 * sizeof(float)));

	std::fill(red, red + size, 0.0f);
	std::fill(green, green + size, 0.0f);
	std::fill(blue, blue + size, 0.0f);
}

unsigned int Image::getWidth() const
{
	return width;
}

unsigned int Image::getHeight() const
{
	return height;
}

float Image::getR(unsigned int x, unsigned int y) const
{
	return get(red, width, height, x, y);
}

float Image::getG(unsigned int x, unsigned int y) const
{
	return get(green, width, height, x, y);
}

float Image::getB(unsigned int x, unsigned int y) const
{
	return get(blue, width, height, x, y);
}

void Image::setR(unsigned int x, unsigned int y, float value)
{
	set(red, width, height, x, y, value);
}

void Image::setG(unsigned int x, unsigned int y, float value)
{
	set(green, width, height, x, y, value);
}

void Image::setB(unsigned int x, unsigned int y, float value)
{
	set(blue, width, height, x, y, value);
}

Image::Difference Image::compare(const Image& other) const
{
	Difference difference = {
		0,
		0,
		0,
		0
	};

	const unsigned int common_width = std::min(width, other.getWidth());
	const unsigned int common_height = std::min(height, other.getHeight());

	for (unsigned int y = 0; y < common_height; ++y) {
		for (unsigned int x = 0; x < common_width; ++x) {
			const int r_diff = getR(x, y) - other.getR(x, y);
			const int g_diff = getG(x, y) - other.getG(x, y);
			const int b_diff = getB(x, y) - other.getB(x, y);
			const unsigned int abs_r_diff = std::max(r_diff, -r_diff);
			const unsigned int abs_g_diff = std::max(g_diff, -g_diff);
			const unsigned int abs_b_diff = std::max(b_diff, -b_diff);

			difference.absolute += abs_r_diff + abs_g_diff + abs_b_diff;
			difference.max_r = std::max(difference.max_r, abs_r_diff);
			difference.max_g = std::max(difference.max_g, abs_g_diff);
			difference.max_b = std::max(difference.max_b, abs_b_diff);
		}
	}

	return difference;
}
