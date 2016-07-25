/**
 * $Id: pconsumertask.h 985 2015-09-09 11:58:44Z klugeflo $
 * @file pconsumertask.h
 * @brief Periodic processor task for time synchronous execution eval 
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TSEVAL_PCONSUMER_TASK_H
#define TSEVAL_PCONSUMER_TASK_H 1

#include <taskmodels/periodictask.h>
#include <core/job.h>

#include <tseval/pproducertask.h>

#include <list>
#include <map>
#include <ostream>
#include <vector>

namespace tmssim {

  /**
   * The following data availability models are implemented:
   * - LIB: latest is best
   * - LET: data is available at the end of the period
   * - RTA: data is available when the worst-case response time expires
   *
   * @todo Logging to file
   */
  class PConsumerTask : public PeriodicTask {
  protected:

    /**
     * @brief Specialisation of #Job that allows some monitoring.
     * @todo Make #Job methods virtual
     */
    class PConsumerJob : public Job {
    public:
    PConsumerJob(PConsumerTask* task, unsigned int jid, TmsTime _activationTime, TmsTimeInterval _executionTime, TmsTime _absDeadline, TmsPriority _priority, int _nProducers, PProducerTask* _producers[]);
      virtual ~PConsumerJob();

      bool execStep(TmsTime now);

      const TmsTimeElement* getDtLIB() const { return dtLIB; }
      const TmsTimeElement* getDtLET() const { return dtLET; }
      const TmsTimeElement* getDtRTA() const { return dtRTA; }
      TmsTime getFirstStep() { return firstStep; }

  private:
      PConsumerTask* pTask;
      int nProducers;
      PProducerTask** producers;
      TmsTime firstStep;
      TmsTimeElement* dtLIB;
      TmsTimeElement* dtLET;
      TmsTimeElement* dtRTA;
  };
  friend class PConsumerJob;

  public:

  static const std::string EVALS[];
  static const size_t N_EVALS;
  
  /**
   * @param _id
   * @param _period
   * @param _executionTime
   * @param _priority
   * @param _responseTime
   * @param _nProducers
   * @param _producers takes ownership of this pointer, frees memory in destructor
   * @param _dataLog where to write log data (time stamps of input data);
   *                 this object must be managed by one of the higher-level
   *                 classes
   */
    PConsumerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, TmsTimeInterval _responseTime, size_t _nProducers, PProducerTask** _producers, std::ostream* _dataLog);
  
    PConsumerTask(const PConsumerTask& rhs);
    
    virtual ~PConsumerTask();
    
    virtual Task* clone() const;

    virtual void setResponseTime(TmsTimeInterval _responseTime) { responseTime = _responseTime; }
    virtual TmsTimeInterval getResponseTime() const { return responseTime; }

    virtual size_t getNProducers() const { return nProducers; }
    
    virtual PProducerTask** getProducers() const { return producers; }

    /**
     * @return the current dataLog object
     */
    std::ostream* getDataLog() const { return dataLog; }

    /**
     * Overwrite the dataLog stream that is used for logging data time stamps
     * @param _dataLog
     */
    void setDataLog(std::ostream* _dataLog) { dataLog = _dataLog; }
    
    
    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */
    
    
    /**
     * @name Time management
     * @{
     */
    
    /**
     * notification that actual execution of a job has started
     * @todo extend #Job exec calls by time parameter
     */
    virtual void executionStarted(const PConsumerTask::PConsumerJob *pjob, TmsTime now);

    /**
     * @}
     */
    
    /**
     * Output of a human-readable description of this task
     */
    virtual std::ostream& print(std::ostream& ost) const;

    virtual std::string getPrintableData() const;

    void calculateStatistics();


    struct Statistics {
    public:
    Statistics(size_t _bsize=0, double _mu=0, double _var=0, double _sigma=0)
    : bsize(_bsize), mu(_mu), var(_var), sigma(_sigma) {}

      size_t bsize;
      double mu;
      double var;
      double sigma;
      std::vector<TmsTimeInterval> rawDataSorted;

      // Gap statistics (only for RTA, will waste some memory in this place!)
      TmsTimeInterval minGap;
      TmsTimeInterval maxGap;
    };
    
  protected:
    virtual Job* spawnHook(TmsTime now);
    virtual bool completionHook(Job *job, TmsTime now);
    virtual std::string getShortId(void) const;

  private:

    double calculateMu(const std::list<TmsTimeInterval>* data) const;
    double calculateVar(const std::list<TmsTimeInterval>* data, double mu) const;
    void sortData(std::vector<TmsTimeInterval>* dataSorted, const std::list<TmsTimeInterval>* data);
    
    TmsTimeInterval responseTime;
    size_t nProducers;
    PProducerTask** producers;
    std::ostream* dataLog;

    /**
     * For each sensor we keep a map that stores a list for each eval scenario.
     * It should suffice to only store time intervals, these should never get
     * too large to exceed the limit.
     */
    std::map<std::string, std::list<TmsTimeInterval>*>* ages;

    std::map<std::string, Statistics*>* statistics;

    /**
     * Store gaps between logical data availability time and consumer
     * activation.
     */
    std::list<TmsTimeInterval>* rtaGaps;

  };


  std::ostream& operator<<(std::ostream& ost, const PConsumerTask::Statistics& stat);

} // NS tmssim

#endif // !TSEVAL_PCONSUMER_TASK_H
