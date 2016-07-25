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
 * $Id: stat.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file stat.h
 * @brief Collection of execution statistics
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_STAT_H
#define CORE_STAT_H

#include <string>
#include <map>
#include <list>
#include <vector>

#include <core/primitives.h>

#ifndef CANCEL_SLOTS
#define CANCEL_SLOTS 10
#endif

namespace tmssim {

  class Stat {
  public:
    
    /**
     * Creates a new Stat-Instance
     */
    Stat();
    
    /**
     * Destorys the current instance
     */
    virtual ~Stat();
  private:
    
    // Members
    
    std::string schedulerId;
    int taskNumber;
    
    std::vector<std::string> taskIds;
    
    // The Key of the maps is always the id-string of the corresponding task
    std::map<std::string,double> historyValues;
    std::map<std::string,double> meanUtilities;
    std::map<std::string,int> activations;
    std::map<std::string,int> completions;
    std::map<std::string,int> cancellations;
    std::map<std::string,int> execCancellations;
    std::map<std::string,int> misses;
    std::map<std::string,int> preemptions;
    std::map<std::string,double> utilitySums;
    std::map<std::string,double> utiltyCounts;
    std::map<std::string,std::vector<int> > delayCounters;

    int sumOfUtilities; // TODO: Baptize it better
    int sumOfUtilityCounts; // TODO: Baptize it better

    int cancelCtr[CANCEL_SLOTS]; // TODO: Convert this into std::vector
    int cancelSteps;
    TmsTime idleSteps;
    bool isVerbose;

    std::list<int> cancelStepList;

  public:
    /**
     * prints the collected statistics into the given output-stream
     * @param stream The stream where to put the statistics in
     */
    void print(std::ostream& stream);

    /**
     * Convenient operator to print a statistics object to an output-stream
     * @param out The stream where to print the statistics to
     * @param statistic The statistics object that should be printed
     * @return An outputstream-object which contains the statistics and can be used for concatination
     */
    friend std::ostream& operator<< (std::ostream& out, Stat& statistic);


    /**
     * Registers a TaskId
     * All task-referenced statistics will only be printed out, if the taskId was added with this method.
     * @param taskId The taskId(-string) that should be stored
     */
    void addTaskId(std::string taskId);

    // Getters and Setters

    const std::map<std::string, int> getActivations() const;
    void setActivations(std::string, int activations);

    const int* getCancelCtr() const;

    const std::map<std::string, int>& getCancellations() const;
    void setCancellations(std::string, int cancellations);

    const std::list<int>& getCancelStepList() const;
    void addToCancelStepList(int cancelStep);

    int getCancelSteps() const;
    void setCancelSteps(int cancelSteps);

    const std::map<std::string, int>& getCompletions() const;
    void setCompletions(std::string, int completions);

    const std::map<std::string, int>& getExecCancellations() const;
    void setExecCancellations(std::string, int execCancellations);

    const std::map<std::string, double>& getHistoryValues() const;
    void setHistoryValues(std::string, double historyValues);

    TmsTime getIdleSteps() const;
    void setIdleSteps(TmsTime idleSteps);

    const std::map<std::string, double>& getMeanUtilities() const;
    void setMeanUtilities(std::string, double meanUtilities);

    const std::map<std::string, int>& getMisses() const;
    void setMisses(std::string, int misses);

    const std::map<std::string, int>& getPreemptions() const;
    void setPreemptions(std::string, int preemptions);

    const std::string& getSchedulerId() const;
    void setSchedulerId(const std::string& schedulerId);

    int getSumOfActivations() const;
    void setSumOfActivations(int sumOfActivations);

    int getSumOfCancellations() const;
    void setSumOfCancellations(int sumOfCancellations);

    int getSumOfCompletions() const;
    void setSumOfCompletions(int sumOfCompletions);

    int getSumOfExecCancellations() const;
    void setSumOfExecCancellations(int sumOfExecCancellations);

    int getSumOfMisses() const;
    void setSumOfMisses(int sumOfMisses);

    int getSumOfPreemptions() const;
    void setSumOfPreemptions(int sumOfPreemptions);

    int getSumOfUtilities() const;
    void setSumOfUtilities(int sumOfUtilities);

    int getSumOfUtilityCounts() const;
    void setSumOfUtilityCounts(int sumOfUtilityCounts);

    int getTaskNumber() const;
    void setTaskNumber(int taskNumber);

    const std::map<std::string, double>& getUtilitySums() const;
    void setUtilitySums(std::string, double utilitySums);

    const std::map<std::string, double>& getUtiltyCounts() const;
    void setUtiltyCounts(std::string, double utiltyCounts);

    const std::map<std::string, std::vector<int> >& getDelayCounters() const;
    void setDelayCounters(const std::map<std::string, std::vector<int> >& delayCounters);

    /**
     * Checks if verbose output is enabeld
     * @return true, if verbose output is enabled, otherwise false
     */
    bool IsVerbose() const;

    /**
     * Sets the verbose output-flag
     * @param verbose signals, if verbose output, should be enabled or not
     */
    void setVerbose(bool verbose);
  };

} // NS tmssim

#endif /* CORE_STAT_H */
