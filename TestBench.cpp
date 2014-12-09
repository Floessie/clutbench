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

#include "TestBench.hpp"

#include "ClutMethod.hpp"
#include "Timer.hpp"
#include "Exception.hpp"

TestBench::TestBench(const Image& _input_image, const Image& _clut_image) :
	input_image(_input_image),
	clut_image(_clut_image),
	clut_level(0)
{
	if (clut_image.getWidth() == clut_image.getHeight()) {
		unsigned int level = 1;
		while (level * level * level < clut_image.getWidth()) {
			++level;
		}
		if (level * level * level == clut_image.getWidth()) {
			clut_level = level;
		}
	}
	if (!clut_level) {
		throw Exception("CLUT image has wrong dimensions.", __FILE__, __LINE__);
	}

	output_image.clearAndInitialize(input_image.getWidth(), input_image.getHeight());
}

Timer TestBench::run(ClutMethod* clut_method, unsigned int cycles)
{
	clut_method->setClut(clut_image, clut_level);

	Timer timer;
	for (unsigned int cycle = 0; cycle < cycles; ++cycle) {
		for (unsigned int y = 0; y < input_image.getHeight(); ++y) {
			for (unsigned int x = 0; x < input_image.getWidth(); ++x) {
				float rgb[4] __attribute__((aligned(16)));
				rgb[0] = input_image.getR(x, y);
				rgb[1] = input_image.getG(x, y);
				rgb[2] = input_image.getB(x, y);
				clut_method->convert(rgb);
				output_image.setR(x, y, rgb[0]);
				output_image.setG(x, y, rgb[1]);
				output_image.setB(x, y, rgb[2]);
			}
		}
	}
	timer.stop();

	return timer;
}

const Image& TestBench::getOutputImage() const
{
	return output_image;
}
