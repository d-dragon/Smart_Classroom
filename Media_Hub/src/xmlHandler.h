/*
 * xmlHandler.h
 *
 *  Created on: Feb 7, 2015
 *      Author: duyphan
 */

#ifndef XMLHANDLER_H_
#define XMLHANDLER_H_

#include <stdio.h>
#include <strings.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlstring.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlversion.h>

//get content from xml doc by element name
char *getRootElementContent(char *xmldoc, int lenght);


#endif /* XMLHANDLER_H_ */
