/*
 * xmlHandler.c
 *
 *  Created on: Feb 7, 2015
 *      Author: duyphan
 */

#include <stdio.h>
#include <strings.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlstring.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlversion.h>
#include "xmlHandler.h"
#include "logger.h"

int findElement(xmlDocPtr doc, xmlNodePtr node, char *element_name,
		char *result);

/* Get xml message type
 * return: notify/request/response
 * xmlbuff: xml message content
 * must free return variable after using
 */
char *getXmlMessageType(char *xmlbuff) {

	xmlDocPtr xmldoc;
	xmlNodePtr xmlrootnode;
	xmlChar *xmlcontent;

	xmldoc = xmlReadMemory(xmlbuff, (int) strlen(xmlbuff), "tmp.xml", NULL, 0);

	if (xmldoc == NULL) {
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlReadMemory failed");
		return NULL;
	}

	xmlrootnode = xmlDocGetRootElement(xmldoc);
	if (xmlrootnode == NULL) {
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlDocGetRootElement failed", __FUNCTION__);
		return NULL;
	}
	if (strcmp((char *) xmlrootnode->name, (char *) "message")) {
		appLog(LOG_DEBUG, "message is invalid!!");
		return NULL;
	}
	xmlcontent = xmlNodeListGetString(xmldoc, xmlrootnode->children->children,
			1);
	xmlFreeDoc(xmldoc);

	return (char *) xmlcontent;

}

int findElement(xmlDocPtr doc, xmlNodePtr node, char *element_name,
		char *result) {

	xmlChar *content;
	node = node->children;
	while (node != NULL) {
		if (strcmp((char *) node->name, element_name)) {
			if (node->children->children != NULL) {
				findElement(doc, node, element_name, result);
			}
		} else {
			content = xmlNodeListGetString(doc, node->children, 1);
			strcpy(result,(char *)content);
			free(content);
//			result = (char *)content;
			return 1;
		}
		node = node->next;
	}
	return 0;
}

/*
 * get element content by name
 * xmlbuff: xml content
 * name: name of element
 * return element content
 * must free pointer which point to return value
 */
char *getXmlElementByName(char *xmlbuff, char *name) {

	xmlDocPtr xmldoc;
	xmlNodePtr xmlrootnode;
	char *xmlcontent;
	xmlcontent = malloc(512);
	memset(xmlcontent, 0x00, 512);

	xmldoc = xmlReadMemory(xmlbuff, (int) strlen(xmlbuff), "tmp.xml", NULL, 0);
	if (xmldoc == NULL) {
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlReadMemory failed");
		return NULL;
	}
	xmlrootnode = xmlDocGetRootElement(xmldoc);
	if (xmlrootnode == NULL) {
		xmlFreeDoc(xmldoc);
		appLog(LOG_DEBUG, "xmlDocGetRootElement failed", __FUNCTION__);
		return NULL;
	}
	appLog(LOG_DEBUG, "root node name: %s", xmlrootnode->name);
	if (strcmp((char *) xmlrootnode->name, (char *) "message")) {
		appLog(LOG_DEBUG, "message is invalid!!");
		return NULL;
	}
	int ret = findElement(xmldoc, xmlrootnode, name, xmlcontent);
	appLog(LOG_DEBUG,"%s: %s",name, xmlcontent);
	return xmlcontent;
}
