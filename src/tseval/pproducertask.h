/**
 * $Id: pproducertask.h 906 2015-04-23 11:07:49Z klugeflo $
 * @file pproducertask.cpp
 * @brief Periodic sensor task for time synchronous execution eval
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TSEVAL_PPRODUCER_TASK_H
#define TSEVAL_PPRODUCER_TASK_H 1

#include <taskmodels/periodictask.h>

#include <map>
#include <ostream>
#include <string>

#include <boost/circular_buffer.hpp>


namespace tmssim {

  struct TmsTimeElement;
    /**
   * @brief Nice output of #TmsTimeElement.
   * @param ost
   * @param tte
   * @return ost
   */
  std::ostream& operator<<(std::ostream& ost, const TmsTimeElement& tte);

  
  /**
   * Store real and logical availability times of sensor data
   */
  struct TmsTimeElement {
  public:
    /**
     * @param _tReal real availability time, e.g. when actual sensor
     * computation has finished
     * @param _tLogical Time when data is made available according to
     * the execution model. This may be later, e.g. at the end of the
     * task's period in the case of LET.
     */
  TmsTimeElement(TmsTime _tReal, TmsTime _tLogical)
  : tReal(_tReal), tLogical(_tLogical) {}

    /**
     * Use this constructor if the logical availability time is the same as
     * the real on, e.g. for latest-is-best.
     * @param _tReal
     */
  TmsTimeElement(TmsTime _tReal=0)
  : tReal(_tReal), tLogical(_tReal) {}
    
  TmsTimeElement(const TmsTimeElement& te)
  : tReal(te.tReal), tLogical(te.tLogical) {}

    TmsTimeElement& operator=(const TmsTimeElement& rhs) {
      tReal = rhs.tReal;
      tLogical = rhs.tLogical;
      return *this;
    }
    
    TmsTime tReal;
    TmsTime tLogical;
  };

  /**
   * @brief Nice output of #TmsTimeElement.
   * @param ost
   * @param tte
   * @return ost
   */
  std::ostream& operator<<(std::ostream& ost, const TmsTimeElement& tte);

  /**
   *
   */
  typedef boost::circular_buffer<TmsTimeElement> TimeBuffer;
  
  /**
   * The following data availability models are implemented:
   * - LIB: latest is best
   * - LET: data is available at the end of the period
   * - RTA: data is available when the worst-case response time expires
   *
   * @todo Logging to file
   */
  class PProducerTask : public PeriodicTask {
  public:
    /**
     * @param _id
     * @param _period
     * @param _executionTime
     * @param _ct
     * @param _priority
     * @param _bufferSize maximum buffer size
     * @param _responseTime
     */
    PProducerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, size_t _bufferSize, TmsTimeInterval _responseTime, std::ostream* _dataLog);

    PProducerTask(const PProducerTask& rhs);

    virtual ~PProducerTask();

    virtual Task* clone() const;

    virtual void setResponseTime(TmsTimeInterval _responseTime) { responseTime = _responseTime; }
    virtual TmsTimeInterval getResponseTime() const { return responseTime; };
    
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
     * @param type use one of LIB, LET or RTA
     * @param time the time the data is read
     * @return the time the last data element before time was available
     */
    virtual TmsTimeElement getData(const std::string& type, TmsTime time) const;

    virtual std::ostream& print(std::ostream& ost) const;
    
  protected:
    //virtual Job* spawnHook(int now);
    virtual bool completionHook(Job *job, TmsTime now);
    //virtual bool cancelHook(Job *job);
    virtual std::string getShortId(void) const;

    
  private:
    size_t bufferSize;
    TmsTimeInterval responseTime;
    std::map<std::string, TimeBuffer> data;
    std::ostream* dataLog;
  };

} // NS tmssim

#endif // !TSEVAL_PPRODUCER_TASK_H
