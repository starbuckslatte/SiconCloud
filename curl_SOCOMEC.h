#define     DEFAULT_IP		    "0.0.0.0"
#define     DEFAULT_PASSWD	    "admin"
#define     DEFAULT_PORT	    80

//+maxchuang add_polling_time_def #begin
#define     POLLING_TIME		5
//+maxchuang add_polling_time_def #end

#define		DEFAULT_DEV_NAME	(const char *)("NET VISION 7")

#define		TMPFILE				(const char *)("/var/curl/tmpfile")
#define		HEADFILE			(const char *)("/var/curl/headfile")
#define		POSTFILE            (const char *)("/var/curl/postfile")
#define		POSTEVTFILE         (const char *)("/var/curl/postevtfile")
#define		EVTHEADFILE         (const char *)("/var/curl/evtheadfile")
//for get company info
#define 	company_start		(const char *)("<company_list>")
#define 	company_end			(const char *)("</company_list>")
//for get device info
#define 	DEV_TYPE_START		(const char *)("<dev_type>")
#define 	DEV_TYPE_END		(const char *)("</dev_type>")
#define 	DEV_INFO_START		(const char *)("<dev_info>")
#define 	DEV_INFO_END		(const char *)("</dev_info>")
#define		SENSOR_MAC_START	(const char *)("<sensor_mac>")
#define		SENSOR_MAC_END		(const char *)("</sensor_mac>")
#define 	DATA_RATE_START		(const char *)("<data_rate>")
#define 	DATA_RATE_END		(const char *)("</data_rate>")

//+maxchuang new_cloudview_spec_20141009 #begin
#define		SENSOR_STATUS_START	(const char *)("<sensor_status>")
#define		SENSOR_STATUS_END	(const char *)("</sensor_status>")
//+maxchuang new_cloudview_spec_20141009 #end
//for get task request
#define 	TASK_ID_START		(const char *)("<task_id>")
#define 	TASK_ID_END			(const char *)("</task_id>")
#define 	DISCOVERY_START		(const char *)("<discovery>")
#define 	DISCOVERY_END		(const char *)("</discovery>")
#define 	ACTION_ID_START		(const char *)("<action_id>")
#define 	ACTION_ID_END		(const char *)("</action_id>")

#define		GET_TASK_REQUEST	(const char *)("cloud/public/service/api/get/format/xml/job/task")                         
#define		GET_DEVINFO_REQUEST	(const char *)("cloud/public/service/api/get/format/xml/job/devinfo")
#define		PUSH_TASK_ID	    (const char *)("cloud/public/service/api/push/format/xml/job/task/tid")                                                
#define		PUSH_AUTO_DATA	    (const char *)("cloud/public/service/api/push/format/xml/job/push_data")
#define		PUSH_AUTO_EVENT	    (const char *)("cloud/public/service/api/push/format/xml/job/push_event")                         


#define     POSTDATA_RETRY_COUNT  10
/*Header status*/
#define		HTTP_RES_OK			(const char *)("HTTP/1.1 200 OK")
#define		HTTP_RES_CONTINUE	(const char *)("HTTP/1.1 100 Continue")
/*Check data type*/
#define		PUSH_DATA							0
#define		PUSH_TASK_DISC						1
#define		PUSH_TASK_DO_ACT					2
#define		PUSH_EVENT							3
#define		GET_TASK							4
#define		GET_DEV_INFO                        5
/*POST data check status*/
#define     SEND_OK     1
#define     SEND_FAIL   0
//ming++ for nv7 >>
#define		DEVICE_MAIN_NV7	                24
#define 	NV7_SENSOR_TYPE	                25
#define     NV7_EMD_SENSOR_TYPE             26
//ming++ for nv7 <<
#define		GET_TASK_FAILED					0
#define		GET_TASK_PASSED					1
#define		GET_DEV_INFO_FAILED	  		    0
#define		GET_DEV_INFO_PASSED				1

/*CloudView severity level*/
#define     CLOUDVIEW_SEVERITY_DISCONNECT    0   
#define     CLOUDVIEW_SEVERITY_CONFIG_FAIL   1
#define     CLOUDVIEW_SEVERITY_CRITICAL      2
#define     CLOUDVIEW_SEVERITY_WARNING       3
#define     CLOUDVIEW_SEVERITY_INFORMATION   4
#define     CLOUDVIEW_SEVERITY_CONNECT       5


#define     CLOUDVIEW_ID_FOR_ON_BATTERY_CLEAR           1
#define     CLOUDVIEW_ID_FOR_ON_BATTERY                 2
                                                        
#define     CLOUDVIEW_ID_FOR_UPS_TEST_COMPLETE          4
                                                        
#define     CLOUDVIEW_ID_FOR_ALARM_INSERT               6
                                                        
#define     CLOUDVIEW_ID_FOR_ALARM_REMOVE               8
#define     CLOUDVIEW_ID_FOR_IM_STOP_CLEAR              9
#define     CLOUDVIEW_ID_FOR_IM_STOP                    10
#define     CLOUDVIEW_ID_FOR_OVERLOAD_CLEAR             11
#define     CLOUDVIEW_ID_FOR_OVERLOAD                   12
#define     CLOUDVIEW_ID_FOR_REDUNDANCE_LOST_CLEAR      13
#define     CLOUDVIEW_ID_FOR_REDUNDANCE_LOST            14
#define     CLOUDVIEW_ID_FOR_DISCONNECT_BAT_CLEAR       15
#define     CLOUDVIEW_ID_FOR_DISCONNECT_BAT             16
#define     CLOUDVIEW_ID_FOR_DISCHARGE_CLEAR            17
#define     CLOUDVIEW_ID_FOR_DISCHARGE                  18
#define     CLOUDVIEW_ID_FOR_BAT_LOW_CLEAR              19
#define     CLOUDVIEW_ID_FOR_BAT_LOW                    20
#define     CLOUDVIEW_ID_FOR_BAT_ALARM_CLEAR            21
#define     CLOUDVIEW_ID_FOR_BAT_ALARM                  22
#define     CLOUDVIEW_ID_FOR_CRITIC_AL_CLEAR            23
#define     CLOUDVIEW_ID_FOR_CRITIC_AL                  24
#define     CLOUDVIEW_ID_FOR_LOAD_OFF_CLEAR             25
#define     CLOUDVIEW_ID_FOR_LOAD_OFF                   26
#define     CLOUDVIEW_ID_FOR_NO_COM_CLEAR               27
#define     CLOUDVIEW_ID_FOR_NO_COM                     28
#define     CLOUDVIEW_ID_FOR_BATTERY_POWER_CLEAR        29
#define     CLOUDVIEW_ID_FOR_BATTERY_POWER              30
#define     CLOUDVIEW_ID_FOR_BAT_TEST_FAILED_CLEAR      31
#define     CLOUDVIEW_ID_FOR_BAT_TEST_FAILED            32
#define     CLOUDVIEW_ID_FOR_TEMP_AL_CLEAR              33
#define     CLOUDVIEW_ID_FOR_TEMP_AL                    34
#define     CLOUDVIEW_ID_FOR_ON_BYPASS_CLEAR            35
#define     CLOUDVIEW_ID_FOR_ON_BYPASS                  36
#define     CLOUDVIEW_ID_FOR_PREVENTIVE_ALARM_CLEAR     37
#define     CLOUDVIEW_ID_FOR_PREVENTIVE_ALARM           38
                                                        
#define     CLOUDVIEW_ID_FOR_SHUTDOWN_WARNING           40
                                                        
#define     CLOUDVIEW_ID_FOR_SHUTDOWN_REQUEST           42
                                                        
#define     CLOUDVIEW_ID_FOR_UPS_NORMAL                 44
                                                        
#define     CLOUDVIEW_ID_FOR_POWER_RESTORED             46
                                                        
#define     CLOUDVIEW_ID_FOR_ALARM_CANCELLED            48
                                                        
#define     CLOUDVIEW_ID_FOR_COM_ESTABLISHED            50
                                                        
#define     CLOUDVIEW_ID_FOR_SHUTDOWN_CANCELLED         52
                                                        
#define     CLOUDVIEW_ID_FOR_AGENT_RESTARTING           54
#define     CLOUDVIEW_ID_FOR_EMD_TEMP_LOW_CLEAR         55
#define     CLOUDVIEW_ID_FOR_EMD_TEMP_LOW               56
#define     CLOUDVIEW_ID_FOR_EMD_TEMP_HIGH_CLEAR        57
#define     CLOUDVIEW_ID_FOR_EMD_TEMP_HIGH              58
#define     CLOUDVIEW_ID_FOR_EMD_HUMI_LOW_CLEAR         59
#define     CLOUDVIEW_ID_FOR_EMD_HUMI_LOW               60
#define     CLOUDVIEW_ID_FOR_EMD_HUMI_HIGH_CLEAR        61
#define     CLOUDVIEW_ID_FOR_EMD_HUMI_HIGH              62
#define     CLOUDVIEW_ID_FOR_EMD_INPUT1_ACTIVE_CLEAR    63
#define     CLOUDVIEW_ID_FOR_EMD_INPUT1_ACTIVE          64
#define     CLOUDVIEW_ID_FOR_EMD_INPUT2_ACTIVE_CLEAR    65
#define     CLOUDVIEW_ID_FOR_EMD_INPUT2_ACTIVE          66
#define     CLOUDVIEW_ID_FOR_EMD_CONNECT                67
#define     CLOUDVIEW_ID_FOR_EMD_DISCONNECT             68
