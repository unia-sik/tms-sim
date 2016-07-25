#!/bin/bash
# $Id: create-release.sh 1329 2016-01-21 10:25:40Z klugeflo $
# create release package of tms-sim

SVN_PATH="https://ginkgo.informatik.uni-augsburg.de/svn/timing-modelle/tms-sim/trunk"

COPY_PATHS="build doc src tasksets vis cfg"
COPY_FILES="CMakeLists.txt bugs.txt build-local*.sh changelog.txt README LICENSE"

PROJECT="tms-sim"

if [ -z "${1}" ]
then
    echo "Need a release number!"
    exit -1
fi

RELEASE="${PROJECT}-${1}"
RELEASE_FILE="${RELEASE}.tar.bz2"

if [ -d ${RELEASE} ]
then
    echo "Release directory ${RELEASE} exists already, removing it..."
    rm -rf ${RELEASE}
fi

if [ -f ${RELEASE_FILE} ]
then
    echo "Release file ${RELEASE_FILE} exists already, removing it..."
    rm -rf ${RELEASE_FILE}
fi


svn export ${SVN_PATH} ${PROJECT} > /dev/null
mkdir ${RELEASE}

for CP in ${COPY_PATHS}
do
    cp -r ${PROJECT}/${CP} ${RELEASE}
done

for CF in ${COPY_FILES}
do
    cp ${PROJECT}/${CF} ${RELEASE}
done

tar cjf ${RELEASE_FILE} ${RELEASE}

rm -rf ${PROJECT}
rm -rf ${RELEASE}

