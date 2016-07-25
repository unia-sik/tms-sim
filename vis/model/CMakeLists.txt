# $Id: CMakeLists.txt 893 2015-04-15 14:55:47Z klugeflo $

set(model_SOURCES
	defaulttaskstatedecoder.cpp
	jobinfo.cpp
	logfilereader.cpp
	mktaskstatedecoder.cpp
	taskstatedecoder.cpp
	taskstatedecoderfactory.cpp
	)

add_library(model OBJECT ${model_SOURCES})
