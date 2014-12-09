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

#pragma once

class Image
{
public:
	struct Difference {
		unsigned long long absolute;
		unsigned int max_r;
		unsigned int max_g;
		unsigned int max_b;
	};

	Image();
	~Image();

	Image(const Image& other);
	Image& operator =(const Image& other);

	void clearAndInitialize(unsigned int width, unsigned int height);

	unsigned int getWidth() const;
	unsigned int getHeight() const;

	float getR(unsigned int x, unsigned int y) const;
	float getG(unsigned int x, unsigned int y) const;
	float getB(unsigned int x, unsigned int y) const;

	void setR(unsigned int x, unsigned int y, float value);
	void setG(unsigned int x, unsigned int y, float value);
	void setB(unsigned int x, unsigned int y, float value);

	Difference compare(const Image& other) const;

private:
	unsigned int width;
	unsigned int height;

	float* red;
	float* green;
	float* blue;
};
