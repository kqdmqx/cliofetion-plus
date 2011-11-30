/*
  This file is part of the RSSReminder project.

  Copyright (C) 2011 glace <glacebai@gmail.com>
  Copyright (C) 2011 DoDo
  Copyright (C) 2011 kqdmqx <kqdmqx@gmail.com>
  Copyright (C) 2011 lucifer

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "xmlitem.h"

int xatoi( char * inStr ){
    int result;
    sscanf( inStr, "%d", &result );
    return result;
}

int xitoa( char * outStr, int Innum){
    return sprintf(outStr, "%d", Innum);
}

int importXmlItemFileForAlarm( const char * filename, struct xmlItemAlarm_t * xmlItemAlarmList, int * xmlItemListAlarmTotal ){
    int subscript;
    int subscriptCount;
    int attrSubscript;
    xmlChar * longStr;
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    *xmlItemListAlarmTotal = 0;

    xmlKeepBlanksDefault(0);

    // create the doc and root node
    xmlDocPtr doc = xmlReadFile( filename, "UTF-8", XML_PARSE_RECOVER );

    if ( doc == NULL ){
        // printf("Error: xml Data file \"%s\" import failed.\n", filename);
        return 0;
    }

    xmlNodePtr root_node = xmlDocGetRootElement( doc );

    if ( root_node == NULL ){
        // printf("Error: failed to get the root of xml doc.\n");
        return 0;
    }

    if ( xmlStrcmp( root_node->name, BAD_CAST "xmlItemListAlarm") != 0 ){
        // printf("Error: name of root node of xml doc not matched.\n");
        return 0;
    }

    if ( xmlHasProp( root_node, BAD_CAST "xmlItemListAlarmTotal") ){
        longStr = xmlGetProp( root_node, BAD_CAST "xmlItemListAlarmTotal" );
        *xmlItemListAlarmTotal = xatoi( (char *) longStr );
        if ( *xmlItemListAlarmTotal > MAX_ITEMLIST_SAVE ) *xmlItemListAlarmTotal = MAX_ITEMLIST_SAVE;
        // printf("itemListTotal: %d\n", *itemListTotal );
    }else{
        // printf("Error: failed to get the itemListTotal.\n");
        return 0;
    }

    node = root_node->xmlChildrenNode;

    for (subscriptCount=1; subscriptCount <= *xmlItemListAlarmTotal; subscriptCount++){
        // printf("subscriptCount: %d\n", subscriptCount);
        if ( node == NULL ){
            // printf("Error: not enough item node.\n");
            *xmlItemListAlarmTotal = 0;
            return 0;
        }

        subscript = subscriptCount;

        xmlItemAlarmList[subscript].message = NULL;

        sonNode = node->xmlChildrenNode;
        while( sonNode != NULL ){
            //     longStr = xmlNodeGetContent( sonNode );
            // printf("name: %s\ncontent: %s\n", (char *) sonNode->name, (char *) longStr);

            if ( xmlStrcmp( sonNode->name, BAD_CAST "message" ) == 0){
                longStr = xmlNodeGetContent( sonNode );
                xmlItemAlarmList[subscript].message = malloc( strlen(longStr) +1 );
                strcpy( xmlItemAlarmList[subscript].message, (char *) longStr );
            }else{
                // printf("Error: unknown sonNode of xml doc item, %s", (char *) sonNode->name );
                *xmlItemListAlarmTotal = 0;
                return 0;
            }

            sonNode = sonNode->next;
        }

        node = node->next;
    }
    printf("import success.\n");
    // printf("xmlItemListTotal: %d\n", *xmlItemListTotal);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 1;
}

int exportXmlItemFileForAlarm( const char * filename, struct xmlItemAlarm_t * xmlItemAlarmList, int * xmlItemListAlarmTotal ){
    int subscript;
    char longStr[100000];
    char message[500];
    xmlNodePtr node;
    xmlNodePtr content;
    xmlNodePtr sonNode;
    xmlNodePtr grandsonNode;

    // create the doc and root node
    xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
    xmlNodePtr root_node = xmlNewNode( NULL, BAD_CAST "xmlItemListAlarm" );

    xitoa( longStr, *xmlItemListAlarmTotal);
    xmlNewProp( root_node, BAD_CAST "xmlItemListAlarmTotal", BAD_CAST longStr );

    // set the root node
    xmlDocSetRootElement( doc, root_node );

    // if ( *xmlItemListAlarmTotal > *xmlItemListTotal ) *xmlItemListAlarmTotal = *xmlItemListTotal;

    for (subscript=1; subscript <= *xmlItemListAlarmTotal; subscript++){
        node = xmlNewNode( NULL, BAD_CAST "item" );
        xmlAddChild( root_node, node );

        // xitoa( longStr, subscript );
        // xmlNewProp( node, BAD_CAST "subscript", BAD_CAST longStr );

        sonNode = xmlNewNode( NULL, BAD_CAST "message" );
        // content = xmlNewCDataBlock( NULL, BAD_CAST longStr, strlen(longStr) );
        // printf( "%s",longStr );
        content = xmlNewCDataBlock( NULL, BAD_CAST xmlItemAlarmList[subscript].message, strlen(xmlItemAlarmList[subscript].message) );
        xmlAddChild( sonNode, content );
        xmlAddChild( node, sonNode );
    }

    int nRel = xmlSaveFormatFileEnc( filename, doc, "UTF-8", 1);
    /*if (nRel != -1)
        printf(" created, %d bytes.\n", nRel);*/

    xmlFreeDoc(doc);

    if (nRel != -1) return 1;
    else return 0;
}
