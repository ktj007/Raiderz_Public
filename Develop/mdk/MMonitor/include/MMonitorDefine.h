#ifndef _MMONITORDEFINE
#define _MMONITORDEFINE

namespace mmonitor {

// MTask
#define TASK_TAG						"TASK"
#define TASK_NAME						"name"	
#define TASK_TYPE						"type"
#define TASK_REQUEST					"request"
#define TASK_TARGET						"target"
#define TASK_INTERVAL					"interval"
#define TASK_IP							"ip"
#define TASK_PORT						"port"
#define TASK_TYPE_NETWORK				"network"
#define TASK_TYPE_LOCAL					"local"


// MTaskManager
#define TASK_NETWORK_TYPE				"network"
#define TASK_LOCAL_TYPE					"local"


// MMonitorProtocol
#define MONITOR_REQEUST_COMMAND			"REQUEST"
#define MONITOR_RESPONSE_COMMAND		"RESPONSE"

#define MONITOR_PROTOCOLID				"id"
#define MONITOR_PROTOCOLPARAM			"PARAM"
#define MONITOR_PROTOCOLPARAMNAME		"name"


#define MMONITOR_XML_BUFFER_MAX_LEN		128

// Network port
#define MONITOR_SERVER_PORT				9000
#define MONITOR_CLIENT_PORT				9001

// ping
#define MONITOR_PING_INTERVAL			(1000 * 60)	// 1분에 한번씩 Ping을 보낸다.

// ping task
#define MONITOR_PING_TASK_NAME			"MonitorPing"

}

#endif