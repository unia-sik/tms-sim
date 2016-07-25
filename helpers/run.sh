#!/bin/bash
# $Id: run.sh 1430 2016-07-15 07:58:02Z klugeflo $
# Author: Florian Kluge <kluge@informatik.uni-augsburg.de>

# Main script for distributed execution of tms-sim evals
# run this script on any machine that takes part in the simulations
#
# Preconditions:
# - You have several computers available to run your simulations
# - Simulation scripts are stored on a server directory which is mounted
#   on all machines
#
# Usage:
# - Create directories WAIT_DIR, RUN_DIR, FIN_DIR, TIME_DIR.
# - Create scripts to execute concrete simultions; if applicable, the
#   simulations should run multithreaded themselves (use the -m switch
#   when calling one of the tms-sim binaries, if supported)
# - Put the scripts into WAIT_DIR
# - Execute this script on any machine that should take part in the
#   simulations; it will execute one run script after another from the
#   WAIT_DIR
#

WAIT_DIR=wait
RUN_DIR=run
FIN_DIR=finished
TIME_DIR=times

LOGFILE="${TIME_DIR}/$(hostname).log"

# This must be the same on all machines!
LOCKFILE=runlock

function getScript() {
    lockfile-create ${LOCKFILE}
    lockfile-touch ${LOCKFILE} &
    # Save the PID of the lockfile-touch process
    BADGER="$!"

    SCRIPT=$(ls ${WAIT_DIR} | head -n 1)
    if [ ! -z "${SCRIPT}" ] ; then
	mv ${WAIT_DIR}/${SCRIPT} ${RUN_DIR}
	echo ${SCRIPT}
    fi
    
    kill "${BADGER}"
    lockfile-remove ${LOCKFILE}
}


NEXT_SCRIPT=$(getScript)

while [ ! -z "${NEXT_SCRIPT}" ] ; do
    echo "Now executing ${NEXT_SCRIPT} ..."
    echo "Now executing ${NEXT_SCRIPT} ..." >> ${LOGFILE}
   
    { time ${RUN_DIR}/${NEXT_SCRIPT}; } 2> ${TIME_DIR}/${NEXT_SCRIPT}.txt
    mv ${RUN_DIR}/${NEXT_SCRIPT} ${FIN_DIR}
    
    NEXT_SCRIPT=$(getScript)
done
