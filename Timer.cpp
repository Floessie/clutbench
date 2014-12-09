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

#include "Timer.hpp"

namespace
{

	unsigned long long diff(const timespec& t1, const timespec& t2)
	{
		static const long long nsecs_per_sec = 1000000000LL;

		timespec _t2 = t2;
		if (t1.tv_nsec < t2.tv_nsec) {
			const time_t sec = (t2.tv_nsec - t1.tv_nsec) / nsecs_per_sec + 1;
			_t2.tv_nsec -= nsecs_per_sec * sec;
			_t2.tv_sec += sec;
		}
		if (t1.tv_nsec - t2.tv_nsec > nsecs_per_sec) {
			const time_t sec = (t1.tv_nsec - t2.tv_nsec) / nsecs_per_sec;
			_t2.tv_nsec += nsecs_per_sec * sec;
			_t2.tv_sec -= sec;
		}

		return (t1.tv_sec - _t2.tv_sec) * nsecs_per_sec + (t1.tv_nsec - _t2.tv_nsec);
	}

}

Timer::Timer()
{
	start();
}

void Timer::start()
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
}

void Timer::stop()
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop_time);
}

unsigned long long Timer::getMSecs() const
{
	return diff(stop_time, start_time) / 1000000;
}

unsigned long long Timer::getUSecs() const
{
	return diff(stop_time, start_time) / 1000;
}

unsigned long long Timer::getNSecs() const
{
	return diff(stop_time, start_time);
}
