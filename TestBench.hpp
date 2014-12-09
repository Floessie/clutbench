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

#include "Image.hpp"

class ClutMethod;
class Timer;

class TestBench
{
public:
	TestBench(const Image& _input_image, const Image& _clut_image);

	Timer run(ClutMethod* clut_method, unsigned int cycles);

	const Image& getOutputImage() const;

private:
	const Image& input_image;
	const Image& clut_image;

	unsigned int clut_level;
	Image output_image;
};
