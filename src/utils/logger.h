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
 * $Id: logger.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file logger.h
 * @brief Simple logging facility for logging of simulation
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 * This logger is intended to be used for logging the actual simulation.
 * It provides multiple output classes that can be de-/activated.
 */

#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <utils/nullstream.h>

#include <string>
#include <iostream>

#include <mutex>
#include <cassert>
#include <sstream>
#include <ctime>
#include <map>
#include <ostream>
#include <string>

#include <cstdint>

namespace tmssim {

  typedef uint32_t LogClass;
  
  
#define LOG_CLASS_TASK       0x00000001 ///!< show output of tasks
#define LOG_CLASS_JOB        0x00000002 ///!< show output of jobs
#define LOG_CLASS_SCHEDULER  0x00000004 ///!< show output of schedulers
#define LOG_CLASS_EXEC       0x00000008
#define LOG_CLASS_SIMULATION 0x00000010
#define LOG_CLASS_RESERVED05 0x00000020
#define LOG_CLASS_RESERVED06 0x00000040
#define LOG_CLASS_RESERVED07 0x00000080
#define LOG_CLASS_RESERVED08 0x00000100
#define LOG_CLASS_RESERVED09 0x00000200
#define LOG_CLASS_RESERVED10 0x00000400
#define LOG_CLASS_RESERVED11 0x00000800
#define LOG_CLASS_RESERVED12 0x00001000
#define LOG_CLASS_RESERVED13 0x00002000
#define LOG_CLASS_RESERVED14 0x00004000
#define LOG_CLASS_RESERVED15 0x00008000
#define LOG_CLASS_EVAL_MK    0x00010000 ///!< show output of (m,k)-evals
#define LOG_CLASS_EVAL_TS    0x00020000 ///!< show output of ts-evals
#define LOG_CLASS_RESERVED18 0x00040000
#define LOG_CLASS_RESERVED19 0x00080000
#define LOG_CLASS_RESERVED20 0x00100000
#define LOG_CLASS_RESERVED21 0x00200000
#define LOG_CLASS_RESERVED22 0x00400000
#define LOG_CLASS_RESERVED23 0x00800000
#define LOG_CLASS_RESERVED24 0x01000000
#define LOG_CLASS_RESERVED25 0x02000000
#define LOG_CLASS_RESERVED26 0x04000000
#define LOG_CLASS_RESERVED27 0x08000000
#define LOG_CLASS_RESERVED28 0x10000000
#define LOG_CLASS_RESERVED29 0x20000000
#define LOG_CLASS_RESERVED30 0x40000000
#define LOG_CLASS_RESERVED31 0x80000000

  //#define LOG_CLASS_EVAL_TS   0x00020000
  
  namespace detail {
#define LOG_N_PREFIX (sizeof(LogClass)*8)
    extern const std::string LOG_PREFIX[LOG_N_PREFIX];
    
    size_t bit2offset(uint32_t _bit);
    
    //template<class Ch, class Tr, class A>
    class output_policy {
    public:
      virtual void operator()(const std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ &s) = 0;
    };
    
    //template<class Ch, class Tr, class A>
    class lno_output : public output_policy/*<Ch, Tr, A>*/ {
    private:
      struct null_buffer : public std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ {
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
    
    //template<class Ch, class Tr, class A>
    class loutput_to_cout : public output_policy/*<Ch, Tr, A>*/ {
    public:
      typedef std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ stream_buffer;
    public:
      void operator()(const stream_buffer &s) {
	static std::mutex mtx;
	std::lock_guard<std::mutex> lck(mtx);
	std::cout << s.str() << std::endl;
      }
      
    private:
      /*
      static std::string now() {
	char buf[64];
	const time_t tm = time(0);  
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	return buf;
      }
      */
    };
    
    //template<class Ch = char, class Tr = std::char_traits<Ch>, class A = std::allocator<Ch> >
    class llogger {
      //typedef OutputPolicy<Ch, Tr, A> output_policy;
    public:
      llogger(LogClass cls) {
	myClass = cls;
	if (myClass & globalClass) {
	  buffer = new loutput_to_cout::stream_buffer;
	}
	else {
	  buffer = new lno_output::stream_buffer;
	}
	*buffer << "==" << LOG_PREFIX[bit2offset(cls)] << "== ";
      }

      static void setClass(LogClass cls) {
	globalClass = cls;
	//std::cout << "Global log level @ " << level << std::endl;
      }

      static void activateClass(std::string cls);
      static void deactivateClass(std::string cls);
      static LogClass getCurrentClass() { return llogger::globalClass; }

      ~llogger() {
	if (myClass & llogger::globalClass) {
	  loutput_to_cout/*<Ch, Tr, A>*/()(*buffer);
	}
	delete buffer;
      }

    public:
      template<class T>
	llogger &operator<<(const T &x) {
	*buffer << x;
	return *this;
      }
    private:
      static LogClass globalClass;
      int myClass;
      std::/*basic_*/ostringstream/*<Ch, Tr, A>*/* buffer;
      //typename output_policy::stream_buffer m_SS;
      static std::map<std::string, LogClass>* nameMapping;

      static void initNameMapping();

      static const std::map<std::string, LogClass>* getNameMapping() {
	if (nameMapping == NULL) {
	  initNameMapping();
	}
	return nameMapping;
      }

      /**
       * Guard for cleaning up memory
       * idea plucked from http://www.oop-trainer.de/Themen/Singleton.html
       */
      class Guard {
      public:
	~Guard() {
	  if (llogger::nameMapping != NULL) {
	    delete llogger::nameMapping;
	  }
	}
      };
      friend class Guard;

    };
  } // NS detail

  typedef detail::llogger/*<char, std::char_traits<char>, std::allocator<char> >*/ logger; 
#define LOG(class) logger(class)
  
} // NS tmssim

#endif /* !UTILS_LOGGER_H */
