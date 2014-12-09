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

#include "OriginalClutMethod.hpp"

namespace
{

	inline void pos2xy(int pos, int imageSideSize, int &outX, int &outY)
	{
		outX = pos % imageSideSize;
		outY = pos / imageSideSize;
	}

}

const char* OriginalClutMethod::getDescription() const
{
	return "Original RawTherapee 4.2 (adapted implementation)";
}

const char* OriginalClutMethod::getFilename() const
{
	return "original";
}

void OriginalClutMethod::setClut(const Image& image, unsigned int level)
{
	clut_image = image;
	clut_level = level;
}

void OriginalClutMethod::convert(float* rgb) const
{
	rgb[0] /= 65535.0f;
	rgb[1] /= 65535.0f;
	rgb[2] /= 65535.0f;

	int color, red, green, blue, i, j;
	float tmp[6], r, g, b;
	int level = clut_level * clut_level;
	int imageSideSize = clut_image.getWidth();

	red = rgb[0] * (float)(level - 1);
	if(red > level - 2)
		red = (float)level - 2;
	if(red < 0)
		red = 0;

	green = rgb[1] * (float)(level - 1);
	if(green > level - 2)
		green = (float)level - 2;
	if(green < 0)
		green = 0;

	blue = rgb[2] * (float)(level - 1);
	if(blue > level - 2)
		blue = (float)level - 2;
	if(blue < 0)
		blue = 0;

	r = rgb[0] * (float)(level - 1) - red;
	g = rgb[1] * (float)(level - 1) - green;
	b = rgb[2] * (float)(level - 1) - blue;

	color = red + green * level + blue * level * level;

	i = color;
	j = color + 1;
	int xi, yi, xj, yj;
	pos2xy(i, imageSideSize, xi, yi);
	pos2xy(j, imageSideSize, xj, yj);

	tmp[0] = clut_image.getR(xi, yi) * (1 - r) + clut_image.getR(xj, yj) * r;
	tmp[1] = clut_image.getG(xi, yi) * (1 - r) + clut_image.getG(xj, yj) * r;
	tmp[2] = clut_image.getB(xi, yi) * (1 - r) + clut_image.getB(xj, yj) * r;

	i = color + level;
	j = color + level + 1;
	pos2xy(i, imageSideSize, xi, yi);
	pos2xy(j, imageSideSize, xj, yj);

	tmp[3] = clut_image.getR(xi, yi) * (1 - r) + clut_image.getR(xj, yj) * r;
	tmp[4] = clut_image.getG(xi, yi) * (1 - r) + clut_image.getG(xj, yj) * r;
	tmp[5] = clut_image.getB(xi, yi) * (1 - r) + clut_image.getB(xj, yj) * r;

	rgb[0] = tmp[0] * (1 - g) + tmp[3] * g;
	rgb[1] = tmp[1] * (1 - g) + tmp[4] * g;
	rgb[2] = tmp[2] * (1 - g) + tmp[5] * g;

	i = color + level * level;
	j = color + level * level + 1;
	pos2xy(i, imageSideSize, xi, yi);
	pos2xy(j, imageSideSize, xj, yj);

	tmp[0] = clut_image.getR(xi, yi) * (1 - r) + clut_image.getR(xj, yj) * r;
	tmp[1] = clut_image.getG(xi, yi) * (1 - r) + clut_image.getG(xj, yj) * r;
	tmp[2] = clut_image.getB(xi, yi) * (1 - r) + clut_image.getB(xj, yj) * r;

	i = color + level + level * level;
	j = color + level + level * level + 1;
	pos2xy(i, imageSideSize, xi, yi);
	pos2xy(j, imageSideSize, xj, yj);

	tmp[3] = clut_image.getR(xi, yi) * (1 - r) + clut_image.getR(xj, yj) * r;
	tmp[4] = clut_image.getG(xi, yi) * (1 - r) + clut_image.getG(xj, yj) * r;
	tmp[5] = clut_image.getB(xi, yi) * (1 - r) + clut_image.getB(xj, yj) * r;

	tmp[0] = tmp[0] * (1 - g) + tmp[3] * g;
	tmp[1] = tmp[1] * (1 - g) + tmp[4] * g;
	tmp[2] = tmp[2] * (1 - g) + tmp[5] * g;

	rgb[0] = rgb[0] * (1 - b) + tmp[0] * b;
	rgb[1] = rgb[1] * (1 - b) + tmp[1] * b;
	rgb[2] = rgb[2] * (1 - b) + tmp[2] * b;
}
