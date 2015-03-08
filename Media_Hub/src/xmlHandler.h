/*
 * xmlHandler.h
 *
 *  Created on: Feb 7, 2015
 *      Author: duyphan
 */

#ifndef XMLHANDLER_H_
#define XMLHANDLER_H_

#define XML_MESSGAE_TYPE		"type"
#define XML_MESSGAE_INFO		"info"
#define XML_MESSAGE_NOTIFY		"notify"
#define XML_MESSAGE_REQUEST 	"request"
#define XML_MESSAGE_RESPONSE	"response"

//get content from xml doc by element name
char *getXmlMessageType(char *xmlbuff);
char *getXmlElementByName(char *xmlbuff, char *name);



#endif /* XMLHANDLER_H_ */
