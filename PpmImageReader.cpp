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

#include <istream>

#include "PpmImageReader.hpp"
#include "Exception.hpp"
#include "Image.hpp"

namespace
{

	bool skipPnmSpace(std::istream& stream)
	{
		enum {
			IN_SPACE,
			AFTER_NEWLINE,
			IN_COMMENT
		} state = IN_SPACE;
		char c;
		bool finished = false;

		while (!finished && stream.get(c)) {
			switch (c) {
				case ' ':
				case '\t':
				case '\r': {
					break;
				}

				case '\n': {
					switch (state) {
						case IN_SPACE: {
							state = AFTER_NEWLINE;
							break;
						}

						case IN_COMMENT: {
							state = IN_SPACE;
							break;
						}

						default: {
							break;
						}
					};
					break;
				}

				case '#': {
					switch (state) {
						case AFTER_NEWLINE: {
							state = IN_COMMENT;
							break;
						}

						default: {
							stream.unget();
							finished = true;
							break;
						}
					};
					break;
				}

				default: {
					switch (state) {
						case IN_COMMENT: {
							break;
						}

						default: {
							stream.unget();
							finished = true;
							break;
						}
					};
					break;
				}
			}
		}
		return stream.good();
	}

}

void PpmImageReader::load(std::istream& stream, Image& image)
{
	char c;

	if (
		!stream.get(c)
		|| c != 'P'
		|| !stream.get(c)
		|| c != '6'
	) {
		throw Exception("Image not a binary portable pixmap.", __FILE__, __LINE__);
	}

	skipPnmSpace(stream);

	unsigned long width;
	unsigned long height;
	unsigned long max_value;

	if (
		!(stream >> width)
		|| !skipPnmSpace(stream)
		|| !(stream >> height)
		|| !skipPnmSpace(stream)
		|| !(stream >> max_value)
		|| !skipPnmSpace(stream)
	) {
		throw Exception("Malformed PPM image header.", __FILE__, __LINE__);
	}

	image.clearAndInitialize(width, height);

	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			for (unsigned int p = 0; p < 3; ++p) {
				unsigned short value = 0;
				if (!stream.get(c)) {
					throw Exception("Corrupt PPM image body.", __FILE__, __LINE__);
				}
				value = static_cast<unsigned char>(c);
				if (max_value > 255) {
					value <<= 8;
					if (!stream.get(c)) {
						throw Exception("Corrupt PPM image body.", __FILE__, __LINE__);
					}
					value |= static_cast<unsigned char>(c);
				}
				const float normalized_value = static_cast<float>(value) * 65535.0f / static_cast<float>(max_value);
				switch (p) {
					case 0: {
						image.setR(x, y, normalized_value);
						break;
					}

					case 1: {
						image.setG(x, y, normalized_value);
						break;
					}

					case 2: {
						image.setB(x, y, normalized_value);
						break;
					}
				}
			}
		}
	}
}
