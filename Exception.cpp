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

#include "Exception.hpp"

Exception::Exception(const std::string& _what_string, const char* _file, unsigned long _line) :
	what_string(_what_string),
	file(_file),
	line(_line)
{
}

Exception::~Exception() throw()
{
}

const char* Exception::what() const throw()
{
	return what_string.c_str();
}

const char* Exception::getFile() const
{
	return file;
}

unsigned long Exception::getLine() const
{
	return line;
}
