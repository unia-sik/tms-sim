/*
 * This file is part of tms-sim.
 *
 * Copyright 2014 University of Augsburg
 *
 * tms-sim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tms-sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tms-sim.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * $Id: tlogger.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file tlogger.h
 * @brief Stream-based logging of tms-sim behaviour
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_TLOGGER_H
#define UTILS_TLOGGER_H 1

#include <mutex>
#include <iostream>
#include <cassert>
#include <sstream>
#include <ctime>
#include <ostream>

namespace tmssim {

  /*
    based on http://stackoverflow.com/a/4455454
  */

  namespace detail {
    template<class Ch, class Tr, class A>
      class no_output {
    private:
      struct null_buffer {
	template<class T>
	null_buffer &operator<<(const T &) {
	  return *this;
	}
      };
    public:
      typedef null_buffer stream_buffer;
      
    public:
      void operator()(const stream_buffer &) {
      }
    };
    
    template<class Ch, class Tr, class A>
      class output_to_clog {
    public:
      typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;
    public:
      void operator()(const stream_buffer &s) {
	static std::mutex mtx;
	std::lock_guard<std::mutex> lck(mtx);
	//std::clog << now() << ": " << s.str() << std::endl;
	std::cout << s.str() << std::endl;
      }
      
    private:
      static std::string now() {
	char buf[64];
	const time_t tm = time(0);  
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	return buf;
      }
    };
    
    template<template <class Ch, class Tr, class A> class OutputPolicy, class Ch = char, class Tr = std::char_traits<Ch>, class A = std::allocator<Ch> >
      class tlogger {
      typedef OutputPolicy<Ch, Tr, A> output_policy;
    public:
      ~tlogger() {
	output_policy()(m_SS);
      }
    public:
      template<class T>
      tlogger &operator<<(const T &x) {
	m_SS << x;
	return *this;
      }
    private:
      typename output_policy::stream_buffer m_SS;
    };
  } // NS detail


#define TLL_NONE  0
#define TLL_ERROR 1
#define TLL_WARN  2
#define TLL_INFO  3
#define TLL_DEBUG 4

#ifndef TLOGLEVEL
#define TLOGLEVEL TLL_WARN
#endif


#if TLOGLEVEL >= TLL_ERROR
  class tError : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tError : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_ERROR

#if TLOGLEVEL >= TLL_WARN
  class tWarn : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tWarn : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_WARN

#if TLOGLEVEL >= TLL_INFO
  class tInfo : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tInfo : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_INFO

#if TLOGLEVEL >= TLL_DEBUG
  class tDebug : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tDebug : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_DEBUG

} // NS tmssim

#endif // !UTILS_TLOGGER_H
