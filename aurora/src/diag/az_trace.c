/**
 * @file   az_trace.c
 * @brief  
 * @date   04/03/18
 * @author dhkang
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
 *   - 2018-03-04 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "mon/az_trace.h"
#include "cli/az_cli_api.h"
#include "net/az_tcpserver.h"

#ifdef  CONFIG_AZ_TRACE

#ifdef  __GNUC__
/* declare global variables */
az_trace_ctrl_t az_trace_ctrl = {
  .state = AZ_TRACE_STATE_IDLE,
  .read_fd = AZ_SYS_FD_INVALID,
  .write_fd = AZ_SYS_FD_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .monSvrIpStr = "127.0.0.1",
  .monSvrPort = CONFIG_AZ_TRACE_MON_SVR_PORT,
};

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
void  az_trace_begin(void)
{
  char *name; 
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;
  do {
    #ifdef  CONFIG_AZ_TRACE_SELF
    int sv[2];
    int r = socketpair(AF_LOCAL, SOCK_NONBLOCK|SOCK_CLOEXEC|SOCK_DGRAM, 0, sv); 
    if (r == 0) {
      ctrl->read_fd = sv[0];
      ctrl->write_fd = sv[1];
    } else {
      az_sys_eprintf("socketpair error=%d\n", errno); 
    }
    /*
    ctrl->read_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (ctrl->read_fd < 0) {
      az_sys_eprintf("socket error=%d\n", ctrl->read_fd);
      break;
    }
    name = AZ_TRACE_SOCK_SVR_NAME; 
    unlink(name);
    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, name);
    bind(ctrl->read_fd, (struct sockaddr *)&addr, sizeof(addr));

    ctrl->write_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (ctrl->write_fd < 0) {
      az_sys_eprintf("socket error=%d\n", ctrl->write_fd);
      break;
    }
    name = AZ_TRACE_SOCK_CLI_NAME; 
    unlink(name);
    struct sockaddr_un cli_addr;
    bzero(&cli_addr, sizeof(cli_addr));
    cli_addr.sun_family = AF_UNIX;
    strcpy(cli_addr.sun_path, name);
    bind(ctrl->write_fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

    int r = connect(ctrl->write_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (r < 0) {
      az_sys_eprintf("connect to %s error = %d" AZ_NL, 
          AZ_TRACE_SOCK_SVR_NAME, r);
      break;
    }
    */
    ctrl->state |= AZ_TRACE_STATE_INIT;
    #elif defined(CONFIG_AZ_TRACE_INICONN)
    az_sys_socket_t cliSock;
    int r = az_inet_openTcpClient(ctrl->monSvrIpStr, ctrl->monSvrPort,
        NULL, 0, &cliSock);
    if (r != AZ_SUCCESS) {
      az_sys_eprintf("connect to trace mon %s:%u error = %d" AZ_NL, 
          ctrl->monSvrIpStr, ctrl->monSvrPort, r);
      break;
    } else {
      az_sys_eprintf("connect to trace mon %s:%u ok" AZ_NL, 
          ctrl->monSvrIpStr, ctrl->monSvrPort);
    }
    ctrl->read_fd = cliSock;
    ctrl->write_fd = cliSock;
    ctrl->state |= AZ_TRACE_STATE_INIT;
  
    az_trace_fileinfo_t fileinfo = {
      .code = AZ_TRACE_CODE_FILENAME,
      .length = sizeof(az_trace_fileinfo_t),
    };
    strncpy(fileinfo.filename, az_getExeFileName(), sizeof(fileinfo.filename)-1);
    az_sys_printf("exe file:%s" AZ_NL, fileinfo.filename);
    r = send(ctrl->write_fd, &fileinfo, fileinfo.length, 0);
    //az_sys_eprintf("%u %u\n", fileinfo.code, fileinfo.length);
    if (r == fileinfo.length) {
      ctrl->state |= AZ_TRACE_STATE_SNDF;
    }
    #endif
  } while (0);
  //az_sys_eprintf0("begin\n");
}

void  az_trace_end(void)
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;

  ctrl->state &= ~AZ_TRACE_STATE_INIT;

  #ifdef  CONFIG_AZ_TRACE_SELF
  az_sys_fd_t fd = ctrl->read_fd;
  ctrl->read_fd = AZ_SYS_FD_INVALID;
  az_sys_fs_close(fd);

  fd = ctrl->write_fd;
  ctrl->write_fd = AZ_SYS_FD_INVALID;
  az_sys_fs_close(fd);
  #elif defined(CONFIG_AZ_TRACE_INICONN)
  az_sys_socket_t cliSock = ctrl->write_fd;
  ctrl->write_fd = AZ_SOCK_INVALID;
  if (cliSock != AZ_SOCK_INVALID) {
    az_sys_socket_delete(cliSock);
  }
  ctrl->read_fd = AZ_SOCK_INVALID;
  #endif

  //az_sys_eprintf0("end\n");
}

int az_trace_start()
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;
  if (ctrl->state & AZ_TRACE_STATE_INIT) {
    ctrl->state |= AZ_TRACE_STATE_BUSY;
  }

  return AZ_TRACE_IS_ACTV();
}

int az_trace_stop()
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;
  if (ctrl->state & AZ_TRACE_STATE_BUSY) {
    ctrl->state &= ~AZ_TRACE_STATE_BUSY;
  }

  return !AZ_TRACE_IS_ACTV();
}
int az_trace_enable(int onoff)
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;

  if (onoff) {
    ctrl->state |= AZ_TRACE_STATE_BUSY;
  } else {
    ctrl->state &= ~AZ_TRACE_STATE_BUSY;
  }
  return AZ_TRACE_IS_ACTV();
}

void az_attr_no_instrument __cyg_profile_func_enter(void *func, void *caller);
void az_attr_no_instrument __cyg_profile_func_exit(void *func, void *caller);

#ifdef  CONFIG_AZ_TRACE_PTHREAD
AZ_SYS_TLS az_xu_trace_info_t *az_pthread_trace_info = NULL;
#endif
void __cyg_profile_func_enter(void *func, void *caller)
{
  az_trace_info_t info;
  az_xu_trace_info_t *xu_info;
  do {
    if (!AZ_TRACE_IS_ACTV()) break;

    #ifdef  CONFIG_AZ_TRACE_PTHREAD
    if (az_pthread_trace_info == NULL) {
      az_pthread_trace_info = az_malloc(sizeof(az_xu_trace_info_t));
      az_pthread_trace_info->thread = pthread_self();
    }
    xu_info = az_pthread_trace_info; 
    if (xu_info) {
      info.xu = xu_info->thread; 
    #else
    xu_info = az_xu_traceinfo(); 
    if (xu_info) {
      info.xu = az_xu_self();
    #endif
      if (!AZ_XU_TRACE_ENABLED(xu_info)) break;
      AZ_XU_TRACE_SET_CURFUNC(xu_info, func);
      info.depth = xu_info->depth++;
      #ifdef  CONFIG_AZ_TRACE_PTHREAD
      strncpy(info.xu_name, xu_info->name, sizeof(info.xu_name)-1);
      #else
      strncpy(info.xu_name, AZ_XU_NAME(info.xu), sizeof(info.xu_name)-1);
      #endif
    } else {
      info.xu = NULL;
      info.depth = 0; 
    }
    clock_gettime(CLOCK_REALTIME, &info.ts);
    info.code = AZ_TRACE_CODE_FUNC_ENTER;
    info.length = AZ_TRACE_INFO_LEN_DFT;
    info.func = func;
    info.caller = caller;

    send(az_trace_ctrl.write_fd, &info, info.length, 0);
    //if (xu_info) {
    //  az_sys_eprintf("%s %p\n", info.xu_name, info.func);
      //write(STDOUT_FILENO, "E0\n", 3);
    //}
  } while (0);
}
void __cyg_profile_func_exit(void *func, void *caller)
{
  az_trace_info_t info;
  az_xu_trace_info_t *xu_info;
  do {
    if (!AZ_TRACE_IS_ACTV()) return;

    #ifdef  CONFIG_AZ_TRACE_PTHREAD
    xu_info = az_pthread_trace_info; 
    if (xu_info) {
      info.xu = xu_info->thread;
    #else
    xu_info = az_xu_traceinfo(); 
    if (xu_info) {
      info.xu = az_xu_self();
    #endif
      if (!AZ_XU_TRACE_ENABLED(xu_info)) break;
      xu_info->depth--;
      if  (xu_info->depth < 0) xu_info->depth = 0; 
      info.depth = xu_info->depth;
      AZ_XU_TRACE_SET_CURFUNC(xu_info, NULL);
      #ifdef  CONFIG_AZ_TRACE_PTHREAD
      strncpy(info.xu_name, xu_info->name, sizeof(info.xu_name)-1);
      #else
      strncpy(info.xu_name, AZ_XU_NAME(info.xu), sizeof(info.xu_name)-1);
      #endif
    } else {
      info.xu = NULL;
      info.depth = 0; 
    }
    clock_gettime(CLOCK_REALTIME, &info.ts);
    info.code = AZ_TRACE_CODE_FUNC_EXIT;
    //info.length = sizeof(info);
    info.length = AZ_TRACE_INFO_LEN_DFT;
    info.func = func;
    info.caller = caller;

    int snt = send(az_trace_ctrl.write_fd, &info, info.length, 0);
    //if (xu_info) {
      //az_sys_eprintf("%s %p %d\n", info.xu_name, info.func, snt);
      //write(STDOUT_FILENO, "E0\n", 3);
      //az_memdisp(STDOUT_FILENO, &info, info.length, 1, 0, STDIN_FILENO); 
    //}
  } while (0);
}

int az_cli_cmd_trace(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;

  do {
    if (argc < 3) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    int onoff = strtol(argv[1], NULL, 10);
    if (!strcmp("all", argv[2])) {
      onoff = az_trace_enable(onoff);
      az_cli_printf("trace all %s\n", onoff? "on":"off");
      break;
    }
    az_xu_t xu = az_xu_find(argv[2]);
    if (xu == NULL) {
      az_cli_printf("thread %s not found\n", argv[2]);
      break;
    }
    onoff = az_xu_set_trace(&xu->trace_info, onoff);
    az_cli_printf("trace %s %s\n", argv[2], onoff? "on":"off");
  } while (0);

  return r;
}

int az_trace_thread_state = 0;
az_xu_t az_trace_thread_default = NULL;

#ifdef  CONFIG_AZ_TRACE_SELF

void *az_trace_thread_proc_default(void *arg)
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl; 
  az_trace_thread_state = 1;
  int r, tlen = 0;
  az_size_t nlen; 
  az_trace_info_t info;
  //int nfds = 0;
  //fd_set fds;
  //struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
  char buf[128];

  az_sys_printf("%s start..." AZ_NL, __FUNCTION__);

  az_cli_cmd_reg("trace", az_cli_cmd_trace, "trace [0/1] [all/thread]\t\t\t;trace functions per thread", 0);

  //FD_ZERO(&fds);
  
  az_sys_io_event_t ioevt;
  az_sys_xu_open_iomux();
  if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
    //nfds = ctrl->read_fd;
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
  } else {
    az_trace_thread_state = 0;
  }
  long count = 0;
  while (az_trace_thread_state) {
    //if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
    //  FD_SET(ctrl->read_fd, &fds);
    //}
    //tv.tv_sec = 3;
    //r = select(nfds + 1, &fds, NULL, NULL, &tv);
    r = az_sys_xu_wait(&ioevt, 1, 3000);
    if (r > 0) {
    //if (FD_ISSET(ctrl->read_fd, &fds)) {
      if (ioevt.data.fd != ctrl->read_fd) continue;
      if (ioevt.events & AZ_SYS_IO_HUP) {
        break;
      }
      char *bp = &info;
      tlen = 0;
      do {
        nlen = read(ctrl->read_fd, bp, sizeof(info) - tlen);  
        if (nlen < 0) break;
        tlen += nlen; bp += nlen;
        if (tlen != sizeof(info)) {
          continue;
        }
        #ifdef  CONFIG_AZ_TRACE_PTHREAD
        nlen = sprintf(buf, "%c %p [%02d] %p %p %lu.%lu\n", 
                      info.code, info.xu, info.depth,
                      info.func, info.caller, info.ts.tv_sec, info.ts.tv_nsec); 
        #else
        nlen = sprintf(buf, "%c %10s:%p [%02d] %p %p %lu.%lu\n", 
                      info.code, info.xu_name, info.xu, info.depth,
                      info.func, info.caller, info.ts.tv_sec, info.ts.tv_nsec); 
        #endif
        //buf[nlen] = 0;
        //printf("%s", buf);
        write(STDOUT_FILENO, buf, nlen);
        break;
      } while (1);
    //}
    }
    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    az_sys_eprintf("r: %d\n", r); 
    break;
  }
  if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
    az_sys_xu_iomux_del(ctrl->read_fd);
  } 
  az_sys_xu_close_iomux();

  return NULL;
}

az_r_t  az_trace_start_default_thread()
{
  az_r_t r;

  az_trace_thread_default = NULL;
  r = (az_r_t)az_xu_create("traceDefault", az_trace_thread_proc_default, NULL, NULL, &az_trace_thread_default);
  //az_sys_printf("%s: %p\n", __FUNCTION__, az_log_thread_default);

  return (r < AZ_SUCCESS)? r:AZ_SUCCESS;
}

az_r_t  az_trace_stop_default_thread()
{
  int r;

  az_trace_thread_state = 0;
  r = az_xu_delete(az_trace_thread_default);

  return r;
}
#else
struct az_tcpserver_oprs az_trace_svr_oprs;
az_tcpserver_t az_trace_svr = {
  .state = 0,
  .config.name = "tracesvr", 
  .config.ipAddrStr = NULL, 
  .config.port = CONFIG_AZ_TRACE_PRB_SVR_PORT,
  .config.backlog = 16, 
  .config.flags = 0, 
  .sock = AZ_SOCK_INVALID, 
  .thread = NULL,
  .priv = NULL,
  .oprs = &az_trace_svr_oprs, 
};

void *az_trace_thread_proc_default(void *arg)
{
  az_trace_ctrl_t *ctrl = &az_trace_ctrl; 
  int r;
  az_size_t nlen, tlen; 
  az_trace_info_t info;
  int nfds = 0;
  //fd_set fds;
  //struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
  char buf[128];

  
  az_sys_io_event_t ioevt;
  az_sys_xu_open_iomux();
  do {
    if (ctrl->read_fd == AZ_SOCK_INVALID) {
      az_trace_thread_state = 0;
      break;
    }
    //nfds = ctrl->read_fd;
    //FD_ZERO(&fds);
    az_trace_thread_state = 1;
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
    if (ctrl->state & AZ_TRACE_STATE_SNDF) {     
      break;
    }
    az_trace_fileinfo_t fileinfo = {
      .code = AZ_TRACE_CODE_FILENAME,
      .length = sizeof(az_trace_fileinfo_t), 
    };
    strncpy(fileinfo.filename, az_getExeFileName(), sizeof(fileinfo.filename)-1);
    az_sys_printf("exe file:%s" AZ_NL, fileinfo.filename);
    r = send(ctrl->write_fd, &fileinfo, fileinfo.length, 0);
    if (r == fileinfo.length) {
        ctrl->state |= AZ_TRACE_STATE_SNDF;
    }
  } while (0);

  az_sys_printf("%s start %d..." AZ_NL, __FUNCTION__, az_trace_thread_state);

  long count = 0;

  while (az_trace_thread_state) {
    //if (ctrl->read_fd != AZ_SOCK_INVALID) {
    //  FD_SET(ctrl->read_fd, &fds);
    //}
    //tv.tv_sec = 1;
    //r = select(nfds + 1, &fds, NULL, NULL, &tv);
    r = az_sys_xu_wait_iomux(&ioevt, 1, 1000); 
    //if (FD_ISSET(ctrl->read_fd, &fds)) {
    while (r > 0) {
      char *bp = &info;
      tlen = 0;
      if (ioevt.data.fd != ctrl->read_fd) break;
      if (ioevt.events & AZ_SYS_IO_HUP) {
        az_trace_stop();
        az_trace_thread_state = 0;
        break;
      }
      do {
        nlen = read(ctrl->read_fd, bp, sizeof(info) - tlen);  
        if (nlen < 0) break;
        tlen += nlen; bp += nlen;
        if (tlen == sizeof(info)) {
          // process control message
          if (info.code == AZ_TRACE_CODE_CMD_START) {
            az_printf("%s", "trace start cmd received\n");
            az_trace_start();
            break;
          } 
          if (info.code == AZ_TRACE_CODE_CMD_STOP) {
            #ifdef  CONFIG_AZ_MTRACE
            az_mcheck_disableMtrace();
            #endif
            az_printf("%s", "trace stop cmd received\n");
            az_trace_stop();
            az_trace_thread_state = 0;
            break;
          } 
          if (info.code == AZ_TRACE_CODE_CMD_ENABLE) {
            int s = az_trace_enable((int)info.depth);
            az_printf("trace on functions %s" AZ_NL, s? "enabled":"disabled");
            break;
          } 
          if (info.code == AZ_TRACE_CODE_CMD_QUERY) {
            info.code = AZ_TRACE_CODE_RSP_QUERY;
            info.length = sizeof(info);
            switch (info.query) {
              case AZ_TRACE_QUERY_CLIPORT:
                *(uint16_t *)info.data = az_cli_getSvrPortNo();
                break;
              default:
                info.query = AZ_TRACE_QUERY_UNKNOWN;
                break;
            }
            send(az_trace_ctrl.write_fd, &info, info.length, 0);
            //az_sys_eprintf("%u %u\n", info.code, info.length);
          }
          break;
        }
      } while (1);
      #ifdef  CONFIG_AZ_MTRACE
      if (az_trace_thread_state) {
        az_mchk_info_t mchk_info;
        if (az_mcheck_mkMallocRpt(&mchk_info) > 0) {
          send(az_trace_ctrl.write_fd, &mchk_info, mchk_info.length, 0);
          //az_sys_eprintf("%u %u\n", mchk_info.code, mchk_info.length);
        }
        if (az_mcheck_mkFreeRpt(&mchk_info) > 0) {
          send(az_trace_ctrl.write_fd, &mchk_info, mchk_info.length, 0);
          //az_sys_eprintf("%u %u\n", mchk_info.code, mchk_info.length);
        }
      }
      #endif
      if (az_trace_thread_state) {
        continue;
      }
    }
    if (r == 0) {
      #ifdef  CONFIG_AZ_MTRACE
      int cnt;
      az_mchk_info_t mchk_info;
      if ((cnt = az_mcheck_mkMallocRpt(&mchk_info)) > 0) {
        az_printf("send malloc rpt %d\n", cnt);
        send(az_trace_ctrl.write_fd, &mchk_info, mchk_info.length, 0);
        //az_sys_eprintf("%u %u\n", mchk_info.code, mchk_info.length);
      } else {
        //az_printf("no send malloc rpt %d\n", info.mchk_count);
      }
      if ((cnt = az_mcheck_mkFreeRpt(&mchk_info)) > 0) {
        az_printf("send free rpt %d\n", cnt);
        send(az_trace_ctrl.write_fd, &mchk_info, mchk_info.length, 0);
        //az_sys_eprintf("%u %u\n", mchk_info.code, mchk_info.length);
      } else {
        //az_printf("no send free rpt %d\n", info.mchk_count);
      }
      #endif
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    if (r < 0 || 0 == az_trace_thread_state) {
      az_sys_socket_t sd = ctrl->write_fd;
      az_sys_xu_iomux_del(ctrl->read_fd);
      ctrl->state &= ~AZ_TRACE_STATE_BUSY;
      ctrl->write_fd = AZ_SOCK_INVALID;
      az_sys_socket_delete(sd);
      ctrl->read_fd = AZ_SOCK_INVALID;
      az_sys_printf("%s socket broken" AZ_NL, __FUNCTION__);
      az_trace_stop();
      break;
    }
  }
  az_sys_xu_close_iomux();

  ctrl->state &= ~AZ_TRACE_STATE_SNDF;
  az_trace_thread_state = 0;
  ctrl->state &= ~AZ_TRACE_STATE_INIT;
  az_sys_printf("%s end..." AZ_NL, __FUNCTION__);

  return NULL;
}

int az_trace_svr_onClientConnection(void *ctx, az_sock_t cliSock, void *cliAddrIn)
{
  int r = AZ_SUCCESS;
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;
  struct sockaddr_in *cliAddr = (struct sockaddr_in *)cliAddrIn;
  char cliIpStr[32] = {0};

  az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));

  if (ctrl->state & AZ_TRACE_STATE_INIT) {
    az_sys_eprintf("new trace connection from %s:%u..., but trace busy" AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));
    az_sys_socket_delete(cliSock);
  } else {
    ctrl->write_fd = cliSock;
    ctrl->read_fd = cliSock;

    ctrl->state |= AZ_TRACE_STATE_INIT;
    az_trace_start();

    az_trace_thread_state = 1;

    az_trace_thread_proc_default(ctrl);

    //az_trace_stop();
    //ctrl->write_fd = AZ_SOCK_INVALID;
    //ctrl->read_fd = AZ_SOCK_INVALID;

    ctrl->state &= ~AZ_TRACE_STATE_INIT;
  }

  return r;
}

#if defined(CONFIG_AZ_TRACE_INICONN)
az_r_t  az_trace_start_default_thread()
{
  az_r_t r = AZ_SUCCESS;
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;

  az_cli_cmd_reg("trace", az_cli_cmd_trace, "trace [0/1] [all/thread]\t\t\t;trace functions per thread", 0);

  if (ctrl->state & AZ_TRACE_STATE_INIT) {
    az_trace_thread_default = NULL;
    r = (az_r_t)az_xu_create("traceDefault", az_trace_thread_proc_default, NULL, NULL, &az_trace_thread_default);
  }

  az_trace_svr_oprs = az_tcpserver_oprs_default;
  az_trace_svr_oprs.onClientConnection = az_trace_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(&az_trace_svr, NULL, &az_trace_svr_oprs, NULL)) {
    az_printf("trace server: %s create result=%d:%s" AZ_NL,
      az_trace_svr.config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (az_trace_svr.oprs->start)(&az_trace_svr);
  } while (0);
  az_printf("trace server: %s start result=%d:%s" AZ_NL,
      az_trace_svr.config.name, r, az_err_str(r));

  return r;
}
az_r_t  az_trace_stop_default_thread()
{
  int r;
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;

  do {
    if (ctrl->state & AZ_TRACE_STATE_INIT) {
      if (az_trace_thread_state) {
        az_trace_thread_state = 0;
        while (ctrl->state & AZ_TRACE_STATE_INIT) {
          az_xu_sleep(1000000);
        }
      }
    }
    r = (az_trace_svr.oprs->stop)(&az_trace_svr);
    if (r) break;
    r = az_tcpserver_delete(&az_trace_svr);
  } while (0);
  az_printf("trace server: %s stop result=%d:%s" AZ_NL, 
      az_trace_svr.config.name, r, az_err_str(r));

  return r;
}

#else
az_r_t  az_trace_start_default_thread()
{
  int r = AZ_SUCCESS;

  az_cli_cmd_reg("trace", az_cli_cmd_trace, "trace [0/1] [all/thread]\t\t\t;trace functions per thrread", 0);

  az_trace_svr_oprs = az_tcpserver_oprs_default;
  az_trace_svr_oprs.onClientConnection = az_trace_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(&az_trace_svr, NULL, &az_trace_svr_oprs, NULL)) {
    az_printf("trace server: %s create result=%d:%s" AZ_NL,
      az_trace_svr.config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (az_trace_svr.oprs->start)(&az_trace_svr);
  } while (0);
  az_printf("trace server: %s start result=%d:%s" AZ_NL,
      az_trace_svr.config.name, r, az_err_str(r));

  return r;
}
az_r_t  az_trace_stop_default_thread()
{
  int r;
  az_trace_ctrl_t *ctrl = &az_trace_ctrl;

  do {
    if (ctrl->state & AZ_TRACE_STATE_INIT) {
      if (az_trace_thread_state) {
        az_trace_thread_state = 0;
        while (ctrl->state & AZ_TRACE_STATE_INIT) {
          az_xu_sleep(1000000);
        }
      }
    }
    r = (az_trace_svr.oprs->stop)(&az_trace_svr);
    if (r) break;
    r = az_tcpserver_delete(&az_trace_svr);
  } while (0);
  az_printf("trace server: %s stop result=%d:%s" AZ_NL, 
      az_trace_svr.config.name, r, az_err_str(r));

  return r;
}
#endif // CONFIG_AZ_TRACE_INICONN
#endif

#endif // __GNUC__

#else
int az_trace_start()
{
  return AZ_ERR(NOT_SUPPORT);
}
int az_trace_stop()
{
  return AZ_ERR(NOT_SUPPORT);
}
az_r_t  az_trace_start_default_thread()
{
  return AZ_ERR(NOT_SUPPORT);
}
az_r_t  az_trace_stop_default_thread()
{
  return AZ_ERR(NOT_SUPPORT);
}
#endif //CONFIG_AZ_TRACE
/* === end of AZ_TRACE_C === */
