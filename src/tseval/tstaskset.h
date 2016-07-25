/**
 * $Id: tstaskset.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tstaskset.h
 * @brief Taskset for evaluation of time synchronous execution
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TSEVAL_TSTASKSET_H
#define TSEVAL_TSTASKSET_H 1

#include <tseval/dependencymatrix.h>
#include <tseval/periodgenerator.h>
#include <tseval/pproducertask.h>
#include <tseval/pconsumertask.h>

#include <utils/kvfile.h>
#include <utils/random.h>

#include <list>
#include <ostream>
#include <string>

namespace tmssim {

  class TsTaskSet {
  private:
    /**
     * @brief temporary container for task set generation
     */
    struct TempTask {
    TempTask() : period(0), wc(0), executionTime(0), priority(0), responseTime(0), isProducer(false) {}
      TmsTimeInterval period;
      TmsTimeInterval wc;
      TmsTimeInterval executionTime;
      TmsPriority priority;
      TmsTimeInterval responseTime;
      bool isProducer;
    };

  public:
    static const std::string CONFIG_PARAMETERS[];
    static const size_t N_CONFIG_PARAMETERS;

    /**
     * @param _kvCfg
     * @param _seed
     * @param _dataLog
     */
    TsTaskSet(const KvFile& _kvCfg, unsigned int _seed, std::ostream* _dataLog);
    ~TsTaskSet();

    static bool validate(const KvFile& kvfile, std::string* msg);

    std::ostream& print(std::ostream& ost) const;

    TmsTime getHyperPeriod() const { return hyperPeriod; }

    /**
     * The caller has to ensure to free the memory occupied by the tasks
     * and the container!
     */
    Taskset* getSimTasks();
    //std::vector<Task*> getSimTasks() { return tasks; }    

    void setDataLog(std::ostream* ost);
    unsigned int getTasksSeed() const { return seedTasks; };
    
  protected:

  private:

    static TmsTime calculateHyperPeriod(TempTask** tasks, size_t nTasks);
    /**
     * @brief Least Common multiple.
     * Uses 
     * \operatorname{gcd}(m,n) \cdot \operatorname{lcm}(m,n) = |m \cdot n| 
     * @param a
     * @param b
     * @return LCM
     */
    static TmsTime calculateLCM(TmsTime a, TmsTime b);
    /**
     * @brief Greatest common divisor.
     * Uses Euclid algorithm:
     * function gcd(a, b)
     *   while b ≠ 0
     *     t := b
     *     b := a mod b
     *     a := t
     *   return a
     * @param a
     * @param b
     * @return GCD
     */
    static TmsTime calculateGCD(TmsTime a, TmsTime b);

    /**
     * @return false, if no taskset was found after $N$ retries, true else
     */
    bool generateTempTaskSet();

    //TempTask* generateTempTasks(size_t nTasks, TmsTimeInterval minPeriod, TmsTimeInterval maxPeriod, double targetUtilisation);
    TempTask* generateTempTasks(size_t nTasks, PeriodGenerator pgen, double targetUtilisation);

    /**
     * Response Time analysis according to Audsley, using algorithm
     * presented in Buttazzo p. 106
     * @param tasks must be ordered by their priority (highest first)
     * @param n number of tasks
     * @return true if all tasks can keep their deadlines
     */
    bool responseTimeAnalysis(TempTask** tasks, size_t n);
    //bool responseTimeAnalysis(list<TsTaskSet::TempTask*>& tasks);

    void createRealTasks(PProducerTask**& _producers, PConsumerTask**& _consumers);
    void createProducersFromTempTasks(PProducerTask**& _producers);
    void createConsumersFromTempTasks(PConsumerTask**& _consumers, PProducerTask**& _producers);
    
    /*
    void generateProducers();
    void generateConsumers();
    void generateMatrix();
    */    
    
    unsigned int seedTasks;
    Random random;
    const KvFile& kvCfg;
    size_t nProducers;
    size_t nConsumers;

    TempTask* tmpProducers;
    TempTask* tmpConsumers;
    TmsTime hyperPeriod;
    DependencyMatrix* dependencies; ///!< rows = consumers
    
    PProducerTask** myProducers;
    PConsumerTask** myConsumers;
    //std::vector<Task*> tasks; ///!< Tasks for simulation

    std::ostream* dataLog;
  };


  std::ostream& operator<<(std::ostream& ost, const TsTaskSet& ts);

  
} // NS tmssim

#endif // !TSEVAL_TSTASKSET_H
