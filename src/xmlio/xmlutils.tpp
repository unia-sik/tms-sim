#include <string>
#include <sstream>
#include <libxml/parser.h>
//#include <core/scobjects.h>

//using namespace std;

namespace tmssim {

template<class T>
T XmlUtils::getNodeValue(xmlNodePtr cur, xmlDocPtr doc) {
	xmlChar* str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	T value = XmlUtils::convertFromXML<T>(str);
	xmlFree(str);
	return value;
}

template<class T>
T XmlUtils::convertFromXML(xmlChar* str) {
	// Handling for Hexadecimal values (0x...)
	T ret;

	if (xmlStrchr(str, '.') != NULL) {
		// It is an Double or Float
		stringstream ss;
		ss << str;
		ss >> ret;
	} else {
		// It is an Integer
		int length = xmlStrlen(str);
		if (length > 2) {
			// could be binary or hex or decimal
			xmlChar* baseTwoChar = xmlStrsub(str, 0, 2);
			xmlChar* baseOneChar = xmlStrsub(str, 0, 1);
			if (xmlStrEqual(baseTwoChar, (xmlChar*) "0x")) {
				ret = strtoul((char*) str, NULL, 16);
			} else if (xmlStrEqual(baseTwoChar, (xmlChar*) "0b")) {
				ret = strtoul((char*) str, NULL, 2);
			} else if (xmlStrEqual(baseOneChar, (xmlChar*) "0")) {
				ret = strtoul((char*) str, NULL, 8);
			} else {
				ret = strtoul((char*) str, NULL, 10);
			}
			xmlFree(baseTwoChar);
			xmlFree(baseOneChar);
		} else if (length > 1) {
			// could be octal or decimal
			xmlChar* baseOneChar = xmlStrsub(str, 0, 1);
			if (xmlStrEqual(baseOneChar, (xmlChar*) "0")) {
				ret = strtoul((char*) str, NULL, 8);
			} else if (length <= 2) {
				ret = strtoul((char*) str, NULL, 10);
			}
			xmlFree(baseOneChar);
		} else {
			// can only be decimal
			ret = strtoul((char*) str, NULL, 10);
		}
	}
	return ret;
}

template<class T>
std::string XmlUtils::convertToXML(const T val) {
	std::stringstream ss;
	ss << val;
	return ss.str();
}

template<class T>
std::string XmlUtils::convertToXML(const T val, format_function pFunc ) {

	string prefix;

	if(pFunc == hex) {
		prefix = "0x";
	} else if(pFunc == dec) {
		prefix == "";
	} else if(pFunc == oct) {
		prefix == "O";
	}

	std::stringstream ss;
	ss << prefix << pFunc << val;
	return ss.str();
}

} // NS tmssim