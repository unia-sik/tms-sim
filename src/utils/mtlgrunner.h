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
 * $Id: mtlgrunner.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mtrunner.h
 * @brief Framework for running multithreaded evaluations with extended
 *        generator thread
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <iostream>

namespace tmssim {

  /**
   * Multithreaded simulation
   */
  template<class WorkClass, class ResultClass>
    class MtLgRunner {

    typedef std::list<WorkClass*>(GenerationFunction)();
    typedef ResultClass*(WorkFunction)(size_t, WorkClass*);
    typedef void (AggregationFunction)(ResultClass*);
    
  public:
    /**
     * @param _generationFunction Function that generates the work. Once
     * this function returns NULL, generation is assumed to be finished.
     * @param _workFunction Function that is executed by each worker thread.
     * @param _aggregationFunction aggregates or outputs the results produced
     * by the worker threads.
     * @param _threads How many worker threads shall be run
     */
    MtLgRunner(GenerationFunction* _generationFunction,
	     WorkFunction* _workFunction,
	     AggregationFunction* _aggregationFunction,
	     size_t _threads)
      : generationFunction(_generationFunction),
      workFunction(_workFunction),
      aggregationFunction(_aggregationFunction),
      threads(_threads),
      generationFinished(false),
      workFinished(false)
	{
	  tWorker = new std::thread[threads];
	}
    
    ~MtLgRunner() {
      delete[] tWorker;
    }


    /**
     * Execute multithreaded work
     */
    void run() {
      tGenerator = std::thread(&MtLgRunner::generationThread, this);
      size_t tids[threads];
      for (size_t i = 0; i < threads; ++i) {
	tids[i] = i;
	tWorker[i] = std::thread(&MtLgRunner::workThread, this, tids[i]);
      }
      tAggregator = std::thread(&MtLgRunner::aggregationThread, this);

      tGenerator.join();
      for (size_t i = 0; i < threads; ++i) {
	tWorker[i].join();
      }
      resultLock.lock();
      workFinished = true;
      resultCond.notify_all();
      resultLock.unlock();
      tAggregator.join();

    }
    

  private:

    void putWork(WorkClass* work) {
      std::unique_lock<std::mutex> lck(workLock);
      workPool.push_back(work);
      workCond.notify_one();
    }

    
    WorkClass* getWork() {
      std::unique_lock<std::mutex> lck(workLock);
      while (!generationFinished && workPool.size() == 0)
	workCond.wait(lck);
      WorkClass* work = NULL;
      if (workPool.size() > 0) {
	work = workPool.front();
	workPool.pop_front();
      }
      else if (generationFinished) {
	work = NULL;
      }
      else {
	// should not happen
	abort();
      }
      return work;
    }

    void putResult(ResultClass* result) {
      std::unique_lock<std::mutex> lck(resultLock);
      resultPool.push_back(result);
      resultCond.notify_one();
    }

    
    ResultClass* getResult() {
      std::unique_lock<std::mutex> lck(resultLock);
      while (!workFinished && resultPool.size() == 0)
	resultCond.wait(lck);
      ResultClass* result = NULL;
      if (resultPool.size() > 0) {
	result = resultPool.front();
	resultPool.pop_front();
      }
      else if (workFinished) {
	result = NULL;
      }
      else {
	// should not happen
	abort();
      }
      return result;
    }


    void generationThread() {
      //std::cout << "G";
      std::list<WorkClass*> workList = generationFunction();
      while (workList.size() > 0) {
	for (WorkClass* work: workList) {
	  putWork(work);
	}
	workList = generationFunction();
      }
      workLock.lock();
      generationFinished = true;
      workCond.notify_all();
      workLock.unlock();
      //std::cout << "g";
    }


    void workThread(size_t tid) {
      //std::cout << "W";

      while (WorkClass* work = getWork()) {
	ResultClass* result = workFunction(tid, work);
	putResult(result);
      }
      //std::cout << "w";
    }

    
    void aggregationThread() {
      //std::cout << "A";
      while (ResultClass* result = getResult()) {
	aggregationFunction(result);
      }
      //std::cout << "a";
    }

    
    
    GenerationFunction* generationFunction;
    WorkFunction* workFunction;
    AggregationFunction* aggregationFunction;
    size_t threads;
    
    std::list<WorkClass*> workPool;
    std::list<ResultClass*> resultPool;

    std::thread tGenerator;
    std::thread* tWorker;
    std::thread tAggregator;

    bool generationFinished;
    std::mutex workLock;
    std::condition_variable workCond;

    bool workFinished;
    std::mutex resultLock;
    std::condition_variable resultCond;

  };

  //template<class WorkClass, class ResultClass>

  
} // NS tmssim
