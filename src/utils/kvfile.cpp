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
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: kvfile.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file kvfile.cpp
 * @brief Key-Value file for config data
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

/**
 * @todo replace atoi/strtoX by <string> header functions stoX.
 */

#include <utils/kvfile.h>

#include <cstdlib> // remove when switched to stoX functions
#include <fstream>
#include <stdexcept>

using namespace std;

#define BUFFLEN 256



namespace tmssim {


  KvFile::KvFile(std::string& filename)
    : map<string, string>()
  {
    readConfig(filename);
  }

  
  KvFile::KvFile(const char *filename)
    : map<string, string>()
  {
    readConfig(string(filename));
  }

  
  KvFile::~KvFile() {
  }


  bool KvFile::containsKey(const std::string& key) const {
    bool rv = true;
    try {
      //const string& value = this->at(key);
      this->at(key);
    }
    catch (const out_of_range& oor) {
      rv = false;
    }
    return rv;
  }


  bool KvFile::getBool(const std::string& key) const {
    const string& str = this->at(key);
    uint32_t val = atoi(str.c_str());
    return (bool) val;
  }

  
  uint8_t KvFile::getUInt8(const string& key) const {
    const string& str = this->at(key);
    uint32_t val = atoi(str.c_str());
    return (uint8_t) val;
  }
  

  uint16_t KvFile::getUInt16(const string& key) const {
    const string& str = this->at(key);
    uint32_t val = atoi(str.c_str());
    return (uint16_t) val;
  }


  uint32_t KvFile::getUInt32(const string& key) const {
    const string& str = this->at(key);
    uint32_t val = atoi(str.c_str());
    return (uint32_t) val;
  }
  

  uint64_t KvFile::getUInt64(const string& key) const {
    const string& str = this->at(key);
    uint64_t val = atoll(str.c_str());
    return val;
  }
  

  int8_t KvFile::getInt8(const string& key) const {
    const string& str = this->at(key);
    int32_t val = atoi(str.c_str());
    return (int8_t) val;
  }
  
  int16_t KvFile::getInt16(const string& key) const {
    const string& str = this->at(key);
    int32_t val = atoi(str.c_str());
    return (int16_t) val;
  }
  
  
  int32_t KvFile::getInt32(const string& key) const {
    const string& str = this->at(key);
    int32_t val = atoi(str.c_str());
    return (int32_t) val;
  }
  
  
  int64_t KvFile::getInt64(const string& key) const {
    const string& str = this->at(key);
    int64_t val = atoll(str.c_str());
    return val;
  }
  

  float KvFile::getFloat(const std::string& key) const {
    const string& str = this->at(key);
    float val = strtof(str.c_str(), NULL);
    return val;
  }


  double KvFile::getDouble(const std::string& key) const {
    const string& str = this->at(key);
    double val = strtof(str.c_str(), NULL);
    return val;
  }
  

  const std::string& KvFile::getString(const std::string& key) const {
    return this->at(key);
  }

  
  void KvFile::readConfig(const string& filename) {
    ifstream fin(filename.c_str());
    string line;
    char buffer[BUFFLEN];
    uint32_t state;
    while (!fin.eof()) {
      fin.getline(buffer, BUFFLEN);
      state = fin.rdstate();
      if (state & ios::eofbit)
	break;
      if (state != 0) {
	fin.close();
	throw KvFileException(state);
      }
      line = buffer;
      trim(line);
      // ignore blank lines...
      if (line.length() == 0)
	continue;
      // ...and comments
      if (line[0] == '#')
	continue;
      //cout << line << endl;;
      storeLine(line);
    }
    fin.close();
  }
  
  void KvFile::storeLine(const string& line) {
    size_t pos;
    string key, value;
    pos = line.find("=");
    key = line.substr(0, pos);
    trim(key);
    value = line.substr(pos+1, line.length()-pos);
    trim(value);
    (*this)[key] = value;
  }
  
  
  void KvFile::trim(string &str) {
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");  
    // if all spaces or empty return an empty string
    if(( string::npos == startpos ) || ( string::npos == endpos))
      str = "";
    else
      str = str.substr( startpos, endpos-startpos+1 );
  }
  

} // NS tmssim
