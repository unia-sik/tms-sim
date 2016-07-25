/**
 * $Id: pconsumertaskbuilder.h 1422 2016-06-22 08:59:13Z klugeflo $
 * @file pconsumertaskbuilder.h
 * @brief Building of pconsumer tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_PCONSUMERTASKBUILDER_H
#define XMLIO_PCONSUMERTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a TmpPConsumerTask object from the given XML-document.
   * Note: only a TmpPConsumerTask object is created, from which the
   * actual PConsumerTask can be created when the PProducerTasks are known.
   */
  class PConsumerTaskBuilder: public IElementBuilder<Task> {
    
  public:
    /**
     * Builds a TmpPConsumerTask object
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };
  
} // NS tmssim

#endif /* XMLIO_PCONSUMERTASKBUILDER_H */
