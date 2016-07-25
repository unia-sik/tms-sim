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
 * $Id: kvfile.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file kvfile.h
 * @brief Key-Value file for config data
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_KVFILE_H
#define UTILS_KVFILE_H 1

#include <map>
#include <string>
#include <stdint.h>

namespace tmssim {

  /**
   * @brief Class for reading a key-value file.
   *
   * This class reads/maps Unix-style key-value files. File entries have
   * have the form "KEY=VALUE". Comment lines start with '#'.
   */
  class KvFile : public std::map<std::string, std::string> {
  
  public:
    //PerConfig();
    KvFile(std::string& filename);
    KvFile(const char *filename);
    ~KvFile();

    bool containsKey(const std::string& key) const;

    bool getBool(const std::string& key) const;
    
    uint8_t getUInt8(const std::string& key) const;
    uint16_t getUInt16(const std::string& key) const;
    uint32_t getUInt32(const std::string& key) const;
    uint64_t getUInt64(const std::string& key) const;
    
    int8_t getInt8(const std::string& key) const;
    int16_t getInt16(const std::string& key) const;
    int32_t getInt32(const std::string& key) const;
    int64_t getInt64(const std::string& key) const;

    float getFloat(const std::string& key) const;
    double getDouble(const std::string& key) const;
    
    const std::string& getString(const std::string& key) const;
    
  private:
    void readConfig(const std::string& filename);
    void storeLine(const std::string& line);
    static void trim(std::string &str);
    
  };
  
  
  class KvFileException {
  public:
  KvFileException(uint32_t err) : error(err) {}
  public:
    uint32_t error;
  };
  

} // NS tmssim

#endif // !UTILS_KVFILE_H
