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

#include "SseClutMethod.hpp"

namespace
{

	inline void posToIndex(unsigned int pos, size_t (&index)[2])
	{
		index[0] = static_cast<size_t>(pos) * 4;
		index[1] = static_cast<size_t>(pos + 1) * 4;
	}

	inline __m128 getClutValue(const unsigned short* clut_image, size_t index)
	{
		return _mm_cvtpu16_ps(*reinterpret_cast<const __m64*>(clut_image + index));
	}

}

SseClutMethod::SseClutMethod() :
	clut_image(0)
{
}

SseClutMethod::~SseClutMethod()
{
	_mm_free(clut_image);
}

const char* SseClutMethod::getDescription() const
{
	return "Integer clut storage with SSE optimization";
}

const char* SseClutMethod::getFilename() const
{
	return "sse";
}

void SseClutMethod::setClut(const Image& image, unsigned int level)
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

void SseClutMethod::convert(float* rgb) const
{
	const unsigned int level = clut_level; // This is important

	const unsigned int red = std::min(flevel_minus_two, rgb[0] * flevel_minus_one);
	const unsigned int green = std::min(flevel_minus_two, rgb[1] * flevel_minus_one);
	const unsigned int blue = std::min(flevel_minus_two, rgb[2] * flevel_minus_one);

	const __m128 v_rgb = _mm_load_ps(rgb) *_mm_load_ps1(&flevel_minus_one) - _mm_set_ps(0.0f, blue, green, red);

	const unsigned int level_square = level * level;

	const unsigned int color = red + green * level + blue * level_square;

	size_t index[2];
	posToIndex(color, index);

	const __m128 v_r = _mm_shuffle_ps(v_rgb, v_rgb, 0x00);
	const __m128 v_one_minus_r = _mm_set_ps1(1.0f) - v_r;

	__m128 v_tmp1 = getClutValue(clut_image, index[0]) * v_one_minus_r + getClutValue(clut_image, index[1]) * v_r;

	posToIndex(color + level, index);

	__m128 v_tmp2 = getClutValue(clut_image, index[0]) * v_one_minus_r + getClutValue(clut_image, index[1]) * v_r;

	const __m128 v_g = _mm_shuffle_ps(v_rgb, v_rgb, 0x55);
	const __m128 v_one_minus_g = _mm_set_ps1(1.0f) - v_g;

	__m128 v_out = v_tmp1 * v_one_minus_g + v_tmp2 * v_g;

	posToIndex(color + level_square, index);

	v_tmp1 = getClutValue(clut_image, index[0]) * v_one_minus_r + getClutValue(clut_image, index[1]) * v_r;

	posToIndex(color + level + level_square, index);

	v_tmp2 = getClutValue(clut_image, index[0]) * v_one_minus_r + getClutValue(clut_image, index[1]) * v_r;

	v_tmp1 = v_tmp1 * v_one_minus_g + v_tmp2 * v_g;

	const __m128 v_b = _mm_shuffle_ps(v_rgb, v_rgb, 0xAA);
	const __m128 v_one_minus_b = _mm_set_ps1(1.0f) - v_b;

	_mm_store_ps(rgb, v_out * v_one_minus_b + v_tmp1 * v_b);
}
