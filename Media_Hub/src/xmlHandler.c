/*
 * xmlHandler.c
 *
 *  Created on: Feb 7, 2015
 *      Author: duyphan
 */



#include "xmlHandler.h"
#include "logger.h"

char *getRootElementContent(char *xmlbuff, int lenght){

	xmlDocPtr xmldoc;
	xmlNodePtr xmlrootnode;
	xmlChar *xmlcontent;
	char *rootcontent;
	rootcontent = malloc(64);
	memset(rootcontent, 0, 64);

	xmldoc = xmlReadMemory(xmlbuff, lenght, "tmp.xml",NULL, 0);

	if( xmldoc == NULL){
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlReadMemory failed");
		return NULL;
	}

	xmlrootnode = xmlDocGetRootElement(xmldoc);
	if(xmlrootnode == NULL){
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlDocGetRootElement failed", __FUNCTION__);
		return NULL;
	}

	xmlcontent = xmlNodeListGetString(xmldoc, xmlrootnode->children->children->children, 1);
	appLog(LOG_DEBUG, "xmlNodeListGetString called: %s ", xmlcontent);

	strcpy(rootcontent, (char *)xmlcontent);
	appLog(LOG_DEBUG, "memcpy called ");
	xmlFreeDoc(xmldoc);
	appLog(LOG_DEBUG, "free called ");
	return rootcontent;

}
