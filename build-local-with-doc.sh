#!/bin/bash
# $Id: build-local-with-doc.sh 815 2014-12-15 13:40:52Z klugeflo $
cmake -DCMAKE_INSTALL_PREFIX="./" -DBUILD_DOCUMENTATION=on $@

