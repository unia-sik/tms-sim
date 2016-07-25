/**
 * $Id: pproducertaskbuilder.h 1422 2016-06-22 08:59:13Z klugeflo $
 * @file pproducertaskbuilder.h
 * @brief Building of producer tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_PPRODUCERTASKBUILDER_H
#define XMLIO_PPRODUCERTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a PProducerTask object from the given XML-document.
   */
  class PProducerTaskBuilder: public IElementBuilder<Task> {
    
  public:
    /**
     * Builds a PProducerTask object
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };
  
} // NS tmssim

#endif /* XMLIO_PPRODUCERTASKBUILDER_H */
