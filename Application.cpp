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
#include <fstream>
#include <sstream>

#include "Application.hpp"

#include "Image.hpp"
#include "Exception.hpp"
#include "PpmImageReader.hpp"
#include "PpmImageWriter.hpp"
#include "TestBench.hpp"
#include "Timer.hpp"

#include "OriginalClutMethod.hpp"
#include "OptimizedClutMethod.hpp"
#include "IntegerClutMethod.hpp"
#include "SseClutMethod.hpp"

namespace
{

	std::vector<ClutMethod*> createClutMethods()
	{
		std::vector<ClutMethod*> clut_methods;
		clut_methods.push_back(new OriginalClutMethod);
		clut_methods.push_back(new OptimizedClutMethod);
		clut_methods.push_back(new IntegerClutMethod);
		clut_methods.push_back(new SseClutMethod);
		return clut_methods;
	}

	void destroyClutMethods(const std::vector<ClutMethod*>& clut_methods)
	{
		for (std::vector<ClutMethod*>::const_iterator clut_methods_it = clut_methods.begin(); clut_methods_it != clut_methods.end(); ++clut_methods_it) {
			delete *clut_methods_it;
		}
	}

	unsigned int getNumber(const std::string& string)
	{
		unsigned int res = 0;
		std::istringstream(string) >> res;
		return res;
	}

	void runBenchmark(const std::vector<std::string>& args)
	{
		std::ifstream input_file(args[1].c_str());
		Image input_image;
		PpmImageReader().load(input_file, input_image);
		std::ifstream clut_file(args[2].c_str());
		Image clut_image;
		PpmImageReader().load(clut_file, clut_image);

		unsigned int cycles = 10;
		if (args.size() > 4) {
			cycles = getNumber(args[4]);
		}

		const std::vector<ClutMethod*> clut_methods = createClutMethods();

		try {
			TestBench test_bench(input_image, clut_image);
			Image reference_image;
			float reference_time_ms = 0.0f;

			for (std::vector<ClutMethod*>::const_iterator clut_methods_it = clut_methods.begin(); clut_methods_it != clut_methods.end(); ++clut_methods_it) {
				if (clut_methods_it != clut_methods.begin()) {
					std::cout << std::endl;
				}
				ClutMethod* const clut_method = *clut_methods_it;

				std::cout << "Method:     " << clut_method->getDescription() << std::endl;
				const Timer timer = test_bench.run(clut_method, cycles);
				std::cout << "Time:       " << timer.getMSecs() << "ms" << std::endl;

				if (clut_methods_it == clut_methods.begin()) {
					reference_image = test_bench.getOutputImage();
					reference_time_ms = timer.getMSecs();
				} else {
					const float speedup = reference_time_ms / static_cast<float>(std::max(1ull, timer.getMSecs()));
					const float faster = 100.0f * reference_time_ms / static_cast<float>(std::max(1ull, timer.getMSecs())) - 100.0f;
					std::cout << "Speedup:    " << speedup << " (" << faster << "% faster)" << std::endl;

					const Image::Difference difference = reference_image.compare(test_bench.getOutputImage());
					std::cout
						<< "Difference: "
						<< difference.absolute
						<< " (Rmax "
						<< difference.max_r
						<< ", Gmax "
						<< difference.max_g
						<< ", Bmax "
						<< difference.max_b
						<< ')'
						<< std::endl;
				}

				std::ofstream output_file((args[3] + '_' + clut_method->getFilename() + ".ppm").c_str());
				PpmImageWriter().save(test_bench.getOutputImage(), output_file);
			}
		}
		catch (...) {
			destroyClutMethods(clut_methods);
			throw;
		}

		destroyClutMethods(clut_methods);
	}

}

class Application::Implementation
{
public:
	Implementation();
	~Implementation();

	int execute(const std::vector<std::string>& args);

private:
};

Application::Application() :
	implementation(new Implementation)
{
}

Application::~Application()
{
	delete implementation;
}

int Application::execute(const std::vector<std::string>& args)
{
	return implementation->execute(args);
}

Application::Implementation::Implementation()
{
}

Application::Implementation::~Implementation()
{
}

int Application::Implementation::execute(const std::vector<std::string>& args)
{
	if (args.size() < 4) {
		std::cerr << "Usage:" << args[0] << " INPUT CLUT OUTPUT_PREFIX [CYCLES]" << std::endl;
		return 1;
	}

	try {
		runBenchmark(args);
	}
	catch (const Exception& exception)
	{
		std::cerr << exception.getFile() << ':' << exception.getLine() << ": " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}
