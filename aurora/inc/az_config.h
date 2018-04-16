/**
 * @file   az_config.h
 * @brief  
 * @date   01/02/16
 * @author Daehyun Kang
 *
 * @copyright
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: 
 *   - 2016-02-01 Daehyun Kang initial create
 */

#ifndef AZ_CONFIG_H
#define AZ_CONFIG_H

#ifdef  linux
    #define CONFIG_LINUX    1
    #define CONFIG_M64      1
#elif defined(_WIN32) 
    #define CONFIG_WINDOWS  1
    #ifdef  _WIN64
        #define CONFIG_M64      1
    #else
        #undef CONFIG_M64 
    #endif
#else
  #error  "not supported system"
#endif

#ifndef CONFIG_M64
#define CONFIG_M32
#endif

#ifdef  CONFIG_AZ_PRELOAD_SO
#define _GNU_SOURCE
#endif

#define CONFIG_AZ_DEBUG             1
#undef CONFIG_AZ_DEBUG             

#define CONFIG_AZ_DEBUG_NATIVE_APP  1
#undef CONFIG_AZ_DEBUG_NATIVE_APP 

#define CONFIG_AZ_USE_TLS       1   ///< use thread specific location for thread var.

#define  CONFIG_AZ_MALLOC       ///< use az_malloc with extra infos 
//#undef  CONFIG_AZ_MALLOC 

#define CONFIG_AZ_DYN_THR_ALLOC  ///< dynamic thread allocation
//#undef CONFIG_AZ_DYN_THR_ALLOC  

#define CONFIG_UNIT_TEST        1
//#undef CONFIG_UNIT_TEST
//
#define CONFIG_TEST_CASE        1
//#undef CONFIG_TEST_CASE


#define  CONFIG_AZ_USE_STDARG   1

#define CONFIG_AZ_NAME_MAX      64

#ifdef  PATH_MAX
#define CONFIG_AZ_PATH_MAX      PATH_MAX
#else
#define CONFIG_AZ_PATH_MAX      512
#endif


#define CONFIG_AZ_LOG                           1
//#undef CONFIG_AZ_LOG           

#define CONFIG_AZ_LOG_DUP_STDOUT                1
#undef CONFIG_AZ_LOG_DUP_STDOUT                

#define CONFIG_AZ_LOG_DUP_USE_LOCAL_SOCKET       1
//#undef CONFIG_AZ_LOG_DUP_USE_LOCAL_SOCKET       

#define CONFIG_AZ_LOG_STDOUT_DUMP_FILE          1
#undef CONFIG_AZ_LOG_STDOUT_DUMP_FILE          

#define CONFIG_AZ_LOG_STDOUT_DUMP_FILE_SIZE     1024
#define CONFIG_AZ_LOG_STDOUT_DUMP_FILE_PREFIX   "az_stdout"


#define CONFIG_AZ_CLI           1
//#undef CONFIG_AZ_CLI           

#define CONFIG_AZM                      1
#undef  CONFIG_AZM                      

#ifdef CONFIG_AZM                      
#define CONFIG_AZ_CLI_SHELL_PROMPT      "$$"
#else
#define CONFIG_AZ_CLI_SHELL_PROMPT      "<<"
#endif

#define CONFIG_AZ_TCPSERVER_USE_SELECT  1
#undef CONFIG_AZ_TCPSERVER_USE_SELECT 

#define CONFIG_AZ_LOG_USE_SELECT  1
#undef CONFIG_AZ_LOG_USE_SELECT  

#define CONFIG_AZ_TRACE                 1

#ifdef CONFIG_AZ_DEBUG_NATIVE_APP  
#define CONFIG_AZ_TRACE_PTHREAD         1
#endif

#if     defined(CONFIG_AZM)                      
#undef CONFIG_AZ_TRACE           
#define CONFIG_AZ_CLI_INPUT_FRWD        1
#endif

#define  CONFIG_AZ_TRACE_SELF           1
//#undef  CONFIG_AZ_TRACE_SELF 

#define CONFIG_AZ_TRACE_INICONN         1
//#undef  CONFIG_AZ_TRACE_INICONN         

#if defined(CONFIG_AZ_TRACE_INICONN)
#undef  CONFIG_AZ_TRACE_SELF 
#endif

#define CONFIG_AZ_TRACE_TASKS           1
#define CONFIG_AZ_TRACE_DEPTH_MAX       16

//#ifdef CONFIG_AZ_TRACE 
//#undef CONFIG_AZ_LOG_DUP_STDOUT                
//#endif

#define CONFIG_AZ_CLI_SVR_PORT               53000
#define CONFIG_AZ_TRACE_MON_SVR_PORT         54000 
#define CONFIG_AZ_TRACE_PRB_SVR_PORT         55000 
#define CONFIG_AZ_PROBE_SVR_PORT             56000 
#define CONFIG_AZ_REMOTE_STDIO_SVR_PORT      59000 
#define CONFIG_AZ_TRZ_SVR_PORT_BASE          58000 

#define CONFIG_AZ_RSTDIO_ENABLED              1
//#undef CONFIG_AZ_RSTDIO_ENABLED              

#ifdef  CONFIG_AZ_MALLOC
#define CONFIG_AZ_MTRACE                1
#define CONFIG_AZ_MTRACE_LIST_SZ        1024
#endif

#ifdef CONFIG_AZM                      
#undef CONFIG_AZ_MTRACE 
#endif

#define CONFIG_AZ_CFG_FOLDER    "."
#define CONFIG_AZ_LOG_FOLDER    "log"
#define CONFIG_AZ_SYS_CFG_FILE  "az_sys_cfg.xml"
#define CONFIG_AZ_APP_CFG_FILE  "az_app_cfg.xml"

#define CONFIG_AZ_ERR_LOG             1
//#undef CONFIG_AZ_ERR_LOG


#define CONFIG_AZ_TCP_SVR_PORT_INC_MAX    16

#define CONFIG_AZ_EXP_CALLSTACK_DEPTH     21

#define CONFIG_AZ_SEGV_TEST               1
#undef CONFIG_AZ_SEGV_TEST               

#define CONFIG_AZ_TRZ_NODEID_MAX          256
#define CONFIG_AZ_TRZ_LOG_SVR_NODE        (CONFIG_AZ_TRZ_NODEID_MAX-1)
#define CONFIG_AZ_TRZ_NODEID_DEFAULT      0 

#define CONFIG_AZ_SYS_PRINT_TIMESTAMP     1
#define CONFIG_AZ_THREAD_PRTBUF_SZ           128 

#define CONFIG_AZ_TEST                    1
//#undef CONFIG_AZ_TEST                    

#define  CONFIG_AZ_SYS_LW_USE_SEM          1 
#undef  CONFIG_AZ_SYS_LW_USE_SEM          
#define  CONFIG_AZ_SYS_LW_USE_MUTEX        1 
#undef  CONFIG_AZ_SYS_LW_USE_MUTEX     

#define CONFIG_AZ_LOG_LEVEL_DFT           50 // AZ_LOG_LEVEL_INFO
#define CONFIG_AZ_LOG_FLAGS_DFT           0  // plain print 

#ifdef  CONFIG_AZ_DEBUG
#define CONFIG_AZ_LOG_LEVEL_DFT           70    // AZ_LOG_LEVEL_DEBUG
#define CONFIG_AZ_LOG_FLAGS_DFT           0x0f  // plain print 
#endif

#define CONFIG_AZ_PERF_MEASURE_LOOP_CNT_DFT   100000 //1000000
#define CONFIG_AZ_PERF_MEASURE_SAMPLES_DFT    100    //1000

#define CONFIG_AZ_THREAD_EXCPT_STK_SZ             16
#define CONFIG_AZ_THREAD_MSG_SZ                   64
#define CONFIG_AZ_THREAD_MSG_QUEUE_SZ             128

#ifdef  CONFIG_AZ_TEST
//#define CONFIG_AZ_THREAD_EXP_HANDLE           1
#endif

#define CONFIG_AZ_THREAD_EXP_HANDLE           1

#define CONFIG_AZ_ION_NONIO                 1
//#undef CONFIG_AZ_ION_NONIO                 

#define CONFIG_AZ_PROBE                   1
#define CONFIG_AZ_PROBE_ENABLE            1
#define CONFIG_AZ_PROBE_SAMPLES_MAX       409600

#define CONFIG_AZ_PROBE_SYS               1
#endif

