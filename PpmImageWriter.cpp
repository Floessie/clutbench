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

#include <iostream>

#include "PpmImageWriter.hpp"
#include "Exception.hpp"
#include "Image.hpp"

PpmImageWriter::PpmImageWriter(bool _eight_bit) :
	eight_bit(_eight_bit)
{
}

void PpmImageWriter::save(const Image& image, std::ostream& stream)
{
	if (!stream) {
		throw Exception("Bad stream.", __FILE__, __LINE__);
	}
	stream << "P6\n";
	stream << "# Created by clutbench\n";
	stream << image.getWidth() << " " << image.getHeight() << '\n';
	if (eight_bit) {
		stream << "255\n";
	} else {
		stream << "65535\n";
	}
	for (unsigned int y = 0; y < image.getHeight(); ++y) {
		for (unsigned int x = 0; x < image.getWidth(); ++x) {
			for (unsigned int p = 0; p < 3; ++p) {
				unsigned short value = 0;
				switch (p) {
					case 0: {
						value = image.getR(x, y);
						break;
					}

					case 1: {
						value = image.getG(x, y);
						break;
					}

					case 2: {
						value = image.getB(x, y);
						break;
					}
				}

				if (eight_bit) {
					stream << static_cast<char>(value >> 8);
				} else {
					stream << static_cast<char>(value >> 8) << static_cast<char>(value & 0xFF);
				}

				if (!stream) {
					throw Exception("Bad stream.", __FILE__, __LINE__);
				}
			}
		}
	}
}
