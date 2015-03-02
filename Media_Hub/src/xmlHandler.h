/*
 * xmlHandler.h
 *
 *  Created on: Feb 7, 2015
 *      Author: duyphan
 */

#ifndef XMLHANDLER_H_
#define XMLHANDLER_H_



//get content from xml doc by element name
char *getXmlMessageType(char *xmlbuff);
char *getXmlElementByName(char *xmlbuff, char *name);



#endif /* XMLHANDLER_H_ */
