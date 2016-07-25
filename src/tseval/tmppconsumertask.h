/**
 * $Id: tmppconsumertask.h 906 2015-04-23 11:07:49Z klugeflo $
 * @file tmppconsumertask.h
 * @brief Temporary consumer task, only for reading from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/periodictask.h>

#include <list>

namespace tmssim {

  class TmpPConsumerTask : public PeriodicTask {
  public:

  /**
   * @param _id
   * @param _period
   * @param _executionTime
   * @param _priority
   * @param _responseTime
   * @param _producers takes ownership of this pointer, frees memory in destructor
   */
    TmpPConsumerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, TmsTimeInterval _responseTime, std::list<std::string>* _producers);
  
    TmpPConsumerTask(const TmpPConsumerTask& rhs);
    
    virtual ~TmpPConsumerTask();
    
    virtual Task* clone() const;
    
    virtual TmsTimeInterval getResponseTime() const { return responseTime; }

    virtual const std::list<std::string>* getProducers() const { return producers; }

    
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
     * @}
     */
    
    /**
     * Output of a human-readable description of this task
     */
    virtual std::ostream& print(std::ostream& ost) const;

  protected:
    virtual Job* spawnHook(TmsTime now);
    virtual bool completionHook(Job *job, TmsTime now);
    virtual std::string getShortId(void) const;

  private:

    TmsTimeInterval responseTime;
    std::list<std::string>* producers;

  };

  

} // NS tmssim
