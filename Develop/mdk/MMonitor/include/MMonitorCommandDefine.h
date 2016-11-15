#ifndef _MMONITORCOMMANDDEFINE
#define _MMONITORCOMMANDDEFINE



#define MMONITOR_REQUEST_COMMAND				"REQUEST"
#define MMONITOR_RESPONSE_COMMAND				"RESPONSE"


#define MMCXML_TRANSMIT_REQUEST					"REQUEST"
#define MMCXML_TRANSMIT_RESPONSE				"RESPONSE"
#define MMCXML_TRANSMIT_ID						"id"
#define MMCXML_TRANSMIT_TASKNAME				"task_name"
#define MMCXML_TRANSMIT_OUTPUTSTRING			"output_string"
#define MMCXML_TRANSMIT_TARGET					"target"
#define MMCXML_TRANSMIT_LASTUPDATETIME			"last_update_time"
#define MMCXML_TRANSMIT_IP						"ip"
#define MMCXML_TRANSMIT_PORT					"port"


#define MONITOR_PROTOCOL_FILENAME				"protocol.xml"
#define MONITOR_PROTOCOLXML_MAX_BUFFER_LEN		256

#define MONITOR_PROTOCOL_REQUEST				"REQUEST"
#define MONITOR_PROTOCOL_RESPONSE				"RESPONSE"
#define MONITOR_PROTOCOL_ID						"id"
#define MONITOR_PROTOCOL_DESCRIPTION			"description"

#define MONITOR_PROTOCOLPARAM					"PARAM"
#define MONITOR_PROTOCOLPARAM_TYPE				"type"
#define MONITOR_PROTOCOLPARAM_DESCRIPTION		"description"

// ping protocol
#define MONITOR_PING_PROTOCOLID					"MonitorPing"
#define MONITOR_REQUEST_PING					"<REQUEST id="MonitorPing">\
												 <PARAM type="string" description="request task">task_name</PARAM>\
												 <PARAM type="string" description="target">target</PARAM>\
												 <PARAM type="string" description="target ip">ip</PARAM>\
												 <PARAM type="string" description="target port">port</PARAM>\
												 </REQUEST>"
#define MONITOR_RESPONSE_PING					"<RESPONSE id="MonitorPing">\
												 <PARAM type="string" description="response task">task_name</PARAM>\
												 <PARAM type="string" description="target">target</PARAM>\
												 <PARAM type="string" description="response ip">ip</PARAP>\
												 <PARAM type="string" description="response port">port</PARAM>\
												 </RESPONSE>"

#endif