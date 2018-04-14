/**
 * @file   azm_trace.c
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
#include "auron.h"
#include "azm_mcheck.h"
#include "azm_trace.h"


#ifdef  __GNUC__
/* declare global variables */
azm_trace_ctrl_t azm_trace_ctrl = {
  .state = AZ_TRACE_STATE_IDLE,
  .flags = AZM_TRACE_DISP_SHORT_FORMAT, 
  .read_fd = AZ_SOCK_INVALID,
  .write_fd = AZ_SOCK_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .prbSvrIpStr = "127.0.0.1",
  .prbSvrPort = CONFIG_AZM_TRACE_PRB_SVR_PORT, 
  .monSvrPort = CONFIG_AZM_TRACE_MON_SVR_PORT, 
  .filepath = {'\0'},
};

int azm_trace_thread_state = 0;
az_thread_t azm_trace_thread_default = NULL;

#define AZM_TRACE_FILTER_MAX  16
struct azm_trace_filter {
  int   count;
  char  keyword[AZM_TRACE_FILTER_MAX][CONFIG_AZ_NAME_MAX]; 
};

struct azm_trace_filter xu_filter = {0}; 
struct azm_trace_filter func_filter = {0}; 

int azm_trace_filter_matched(char *name, struct azm_trace_filter *filter)
{
  int j, k;
  if (filter->count <= 0) return 0;
  for (j = 0, k = filter->count; j < AZM_TRACE_FILTER_MAX && k > 0; j++) {
    if (filter->keyword[j][0] == 0) continue;
    k--;
    if (!strcmp(name, filter->keyword[j])) return 1; 
  }
  return 0;
}
int azm_trace_filter_show(struct azm_trace_filter *filter)
{
  int j, k = 0;
  if (filter->count <= 0) return 1;
  for (j = 0; j < AZM_TRACE_FILTER_MAX && k < filter->count; j++) {
    if (filter->keyword[j][0] == 0) continue;
    k++;
    if (k % 8 == 0) { 
      az_cli_printf("%s" AZ_NL, filter->keyword[j]);
    } else {
      az_cli_printf("%s" "  ", filter->keyword[j]);
    }
  }
  return 0;
}
int azm_trace_filter_add(struct azm_trace_filter *filter, char *name)
{
  int j, k = AZM_TRACE_FILTER_MAX - filter->count;
  if (k <= 0) return 0;
  for (j = 0; j < AZM_TRACE_FILTER_MAX && k > 0; j++) {
    if (filter->keyword[j][0] == 0) {
      strncpy(filter->keyword[j], name, CONFIG_AZ_NAME_MAX);
      filter->count++;
      return 1;
    }
    k--;
  }
  return 0;
}
int azm_trace_filter_del(struct azm_trace_filter *filter, char *name)
{
  int j, k = 0;
  if (filter->count <= 0) return 0;
  for (j = 0; j < AZM_TRACE_FILTER_MAX && k < filter->count; j++) {
    if (!strcmp(filter->keyword[j], name)) {
      filter->count--;
      filter->keyword[j][0] = 0;
      return 1;
    }
    k++;
  }
  return 0;
}
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


extern int az_attr_no_instrument libtrace_init(
          const char *file_name, 
          const char *section_name,
          const char *target);
extern int az_attr_no_instrument libtrace_close(void);
extern int az_attr_no_instrument libtrace_resolve(
          void *addr, 
          char *buf_func, size_t buf_func_len, 
          char *buf_file, size_t buf_file_len,  
          ...
          );
/* implement global functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int resolve_symbol = 0;
az_diag_info_t rxinfo;
static char buf[256];
int azm_trace_proc_recv(int fd)
{
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl; 
  int nlen, tlen, qlen;
  az_trace_info_t *info = &rxinfo.trace;
  char  *bp = info;
  static uint64_t pkts = 0;

  //memset(info,0, sizeof(*info)); 
  info->length = 0;
  tlen = 0;
  qlen = sizeof(info->code) + sizeof(info->length); 
  do {
    nlen = recv(ctrl->read_fd, bp, qlen - tlen, 0);
    if (nlen < 0) {
      return AZ_ERR(FAIL);
      break;
    }
    tlen += nlen; bp += nlen;
    if (tlen == qlen) {
      break;
    }
  } while (1);

  if (0) {
  if (info->code < 0x20) {
    az_sys_eprintf("%u  %u\n", info->code, info->length);
  } else {
    az_sys_eprintf("%c  %u\n", info->code, info->length);
  }
  }

  qlen = info->length; 
  do {
    nlen = recv(ctrl->read_fd, bp, qlen-tlen, 0);  
    if (nlen < 0) {
      return AZ_ERR(FAIL);
      break;
    }
    tlen += nlen; bp += nlen;
  } while (tlen != qlen);
  //az_memdisp(STDOUT_FILENO, &info, tlen, 1, 0, STDIN_FILENO);

  nlen = 0;
    pkts++;
    char  func_name[AZ_NAME_MAX + 1] = {0};
    char  file_name[AZ_PATH_MAX + 1] = {0};
    if (info->code < 0x20) {
      az_mchk_info_t *mchk_info = (az_mchk_info_t *)info;
      int lcnt = AZ_MCK_INFO_LIST_SZ(mchk_info->length);
      int j, slen;
      az_ref_t addr, callsite;
      uint32_t size;
      if (mchk_info->code == AZ_TRACE_CODE_RPT_MALLOC) {
        if ((xu_filter.count + func_filter.count) > 0) {
        } else {
        AZM_MALLOC_STATS()->nTotal = mchk_info->nTotal;
        if (resolve_symbol) {
          for (j = 0; j < lcnt; j++) {
            addr = mchk_info->list[j].addr;
            callsite = mchk_info->list[j].call;
            size = mchk_info->list[j].size;
            AZM_MTRACE_ALLOC_PUSH(addr, callsite, size, AZM_MALLOC_STATS()); 
            libtrace_resolve(callsite, func_name, AZ_NAME_MAX, file_name, AZ_PATH_MAX);
            slen = sprintf(buf, "[A] %ld %p %p %u %21s %21s\n", 
                mchk_info->nTotal, addr, callsite, size, func_name, file_name); 
            write(STDOUT_FILENO, buf, slen);
          }
        } else {
          for (j = 0; j < lcnt; j++) {
            addr = mchk_info->list[j].addr;
            callsite = mchk_info->list[j].call;
            size = mchk_info->list[j].size;
            AZM_MTRACE_ALLOC_PUSH(addr, callsite, size, AZM_MALLOC_STATS()); 
            slen = sprintf(buf, "[A] %ld %p %p %u\n", mchk_info->nTotal, addr, callsite, size); 
            write(STDOUT_FILENO, buf, slen);
          }
        }
        }
      } else 
      if (mchk_info->code == AZ_TRACE_CODE_RPT_FREE ) {
        if ((xu_filter.count + func_filter.count) > 0) {
        } else {
        AZM_MALLOC_STATS()->nTotal = mchk_info->nTotal;
        if (resolve_symbol) {
          for (j = 0; j < lcnt; j++) {
            addr = mchk_info->list[j].addr;
            callsite = mchk_info->list[j].call;
            size = mchk_info->list[j].size;
            AZM_MTRACE_ALLOC_POP(addr, callsite, size, AZM_MALLOC_STATS()); 
            libtrace_resolve(callsite, func_name, AZ_NAME_MAX, file_name, AZ_PATH_MAX);
            slen = sprintf(buf, "[F] %ld %p %p %u %21s %21s\n", 
                mchk_info->nTotal, addr, callsite, size, func_name, file_name); 
            write(STDOUT_FILENO, buf, slen);
          }
        } else {
          for (j = 0; j < lcnt; j++) {
            addr = mchk_info->list[j].addr;
            callsite = mchk_info->list[j].call;
            size = mchk_info->list[j].size;
            AZM_MTRACE_ALLOC_POP(addr, callsite, size, AZM_MALLOC_STATS()); 
            slen = sprintf(buf, "[F] %ld %p %p %u\n", mchk_info->nTotal, addr, callsite, size); 
            write(STDOUT_FILENO, buf, slen);
          }
        }
        }
      } else 
      if (info->code == AZ_TRACE_CODE_RSP_QUERY) {
        azm_trace_proc_query_rsp(info->query, info->data, sizeof(info->data));
      } else
      if (info->code == AZ_TRACE_CODE_FILENAME) {
        az_trace_fileinfo_t *fileinfo = (az_trace_fileinfo_t *)info;
        strncpy(ctrl->filepath, fileinfo->filename, sizeof(ctrl->filepath)-1);
        az_printf("libtrace init for %s\n", ctrl->filepath); 
        if (ctrl->filepath[0]) {
          int r = libtrace_init(ctrl->filepath, NULL, NULL);
          if (r < 0) {
            az_printf("libtrace init fail for %s\n", ctrl->filepath);
            resolve_symbol = 0;
          } else {
            az_printf("libtrace init ok for %s\n", ctrl->filepath);
            resolve_symbol = 1; 
          }
        }
      } else {
        az_sys_printf("INVALID CODE: %x\n", info->code);
      }
    } else {
      char *xu_name = (info->xu_name[0])? info->xu_name:"????";
      if (resolve_symbol) {
      char  ok = 0;
      libtrace_resolve(info->func, func_name, AZ_NAME_MAX, NULL, 0);
      if ((xu_filter.count + func_filter.count) > 0) {
        if (azm_trace_filter_matched(xu_name, &xu_filter ) > 0) ok = 1; 
        if (!ok) {
        if (azm_trace_filter_matched(func_name, &func_filter ) > 0) ok = 1; 
        }
      } else {
        ok = 1;
      }
      if (ok) {
      if (ctrl->flags == AZM_TRACE_DISP_LONG_FORMAT) {
        libtrace_resolve(info->caller, NULL, 0, file_name, AZ_PATH_MAX);
        nlen = sprintf(buf, "%c%ld %10s:%p [%02d] %lu.%lu %21s %21s\n", 
            info->code, pkts, xu_name, info->xu, info->depth, 
            info->ts.tv_sec%3600, info->ts.tv_nsec/1000, func_name, file_name); 
      } else {
        nlen = sprintf(buf, "%c%ld %10s [%02d] %lu.%lu %p:%21s\n", 
            info->code, pkts, xu_name, info->depth, 
            info->ts.tv_sec%3600, info->ts.tv_nsec/1000, info->func, func_name); 
      }
      }
    } else {
      if (azm_trace_filter_matched(xu_name, &xu_filter )) { 
      nlen = sprintf(buf, "%c%ld %10s:%p [%02d] %p %p %lu.%lu\n", 
          info->code, pkts, xu_name, info->xu, info->depth,
          info->func, info->caller, info->ts.tv_sec, info->ts.tv_nsec); 
      }
    }
    if (nlen > 0) {
      write(STDOUT_FILENO, buf, nlen);
      if (fd > 0) write(fd, buf, nlen);
    }
    }

  return nlen;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_proc_default(void *arg)
{
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl; 
  az_trace_info_t info;
  int r;

  az_printf("%s start...\n", __FUNCTION__);

  az_sys_xu_open_iomux();

  if (ctrl->filepath[0]) {
    r = libtrace_init(ctrl->filepath, NULL, NULL);
    if (r < 0) {
      az_printf("libtrace init fail for %s\n", ctrl->filepath);
      resolve_symbol = 0;
    } else {
      az_printf("libtrace init ok for %s\n", ctrl->filepath);
      resolve_symbol = 1;
    }
  }
  int fd = open("/tmp/xxx", O_RDWR|O_CREAT|O_CLOEXEC, 0666);  
  
  if (ctrl->state == AZ_TRACE_STATE_IDLE) {
    az_sys_socket_t cliSock;
    r = az_inet_openTcpClient(ctrl->prbSvrIpStr, ctrl->prbSvrPort, 
        NULL, 0, &cliSock);
    if (r != AZ_SUCCESS) {
      az_cli_printf("connect fail to trace server %s:%u\n",
          ctrl->prbSvrIpStr, ctrl->prbSvrPort);
      azm_trace_thread_state = 0;
    } else {
      azm_trace_thread_state = 1;
      ctrl->read_fd = cliSock;
      ctrl->write_fd = cliSock;
      ctrl->state |= AZ_TRACE_STATE_INIT;
    
      info.code = AZ_TRACE_CODE_CMD_START;
      send(ctrl->write_fd, &info, sizeof(info), 0);
    }
  } 

  if (ctrl->read_fd != AZ_SOCK_INVALID) {
    ctrl->state |= AZ_TRACE_STATE_BUSY;
    azm_trace_thread_state = 1;
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP|AZ_SYS_IO_RDHUP);
  }

  long count = 0;
  az_sys_io_event_t ioevt;
  while (azm_trace_thread_state) {
    r = az_sys_xu_wait_iomux(&ioevt, 1, 3000);
    if (r > 0) {
      if (ioevt.data.fd != ctrl->read_fd) {
        continue;
      }
      if (ioevt.events & (AZ_SYS_IO_HUP|AZ_SYS_IO_RDHUP)) {
          az_sys_eprintf("disconnected %x\n", ioevt.events);
          break;
      }
      if (ioevt.events & AZ_SYS_IO_IN) {
        r =  azm_trace_proc_recv(fd);
      } else {
        az_sys_eprintf("events %x\n", ioevt.events);
        break;
      }
    }
    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    if (r < 0) {
      az_sys_xu_iomux_del(ctrl->read_fd);
      az_sys_socket_delete(ctrl->read_fd);
      ctrl->read_fd = AZ_SOCK_INVALID;
      ctrl->write_fd = AZ_SOCK_INVALID;
      break;
    }
  }
  if (fd > 0) close(fd);

  ctrl->filepath[0] = 0;

  if (ctrl->write_fd != AZ_SOCK_INVALID) {
    info.code = AZ_TRACE_CODE_CMD_STOP;
    send(ctrl->write_fd, &info, sizeof(info), 0);
    az_thread_sleep(1000000);

    az_sys_xu_iomux_del(ctrl->read_fd);
    az_sys_socket_delete(ctrl->write_fd);
    ctrl->read_fd = AZ_SOCK_INVALID;
    ctrl->write_fd = AZ_SOCK_INVALID;
  }
  //az_sys_xu_close_iomux();

  if (resolve_symbol) {
    libtrace_close();
  }

  ctrl->state &= ~AZ_TRACE_STATE_BUSY;
  azm_trace_thread_state = 0;

  az_printf("%s end...\n", __FUNCTION__);
  fflush(stdout);

  return r;
}

struct az_tcpserver_oprs azm_trace_svr_oprs;
az_tcpserver_t azm_trace_svr = {
  .state = 0,
  .config.name = "rtraceSvr", 
  .config.ipAddrStr = NULL, 
  .config.port = CONFIG_AZ_TRACE_MON_SVR_PORT,
  .config.backlog = 16, 
  .config.flags = 0, 
  .sock = AZ_SOCK_INVALID, 
  .thread = NULL,
  .priv = NULL,
  .oprs = &azm_trace_svr_oprs, 
};

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_svr_onClientConnection(void *ctx, az_sock_t cliSock, void *cliAddrIn)
{
  int r = AZ_SUCCESS;
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl;
  struct sockaddr_in *cliAddr = (struct sockaddr_in *)cliAddrIn;
  char cliIpStr[32] = {0};
  az_tcpserver_t *svr = (az_tcpserver_t *)ctx;

  svr->state |= AZ_TCPSERVER_STATE_CLIBUSY;

  az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
  az_sys_eprintf("new trace connection from %s:%u" AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));

  do {
    if (ctrl->state != AZ_TRACE_STATE_IDLE) {
      az_sys_eprintf("trace busy state=%x..., reject connection" AZ_NL,
          ctrl->state);
      r = AZ_ERR(AGAIN);
    }
    ctrl->state |= AZ_TRACE_STATE_INIT;
    ctrl->read_fd = cliSock;
    ctrl->write_fd = cliSock;

    r = azm_trace_proc_default(ctrl);

    ctrl->state &= ~AZ_TRACE_STATE_INIT;
  } while (0);

  svr->state &= ~AZ_TCPSERVER_STATE_CLIBUSY;
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_start()
{
  int r = AZ_SUCCESS;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_trace_svr;
  
  azm_trace_svr_oprs = az_tcpserver_oprs_default;
  azm_trace_svr_oprs.onClientConnection = azm_trace_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(svr, NULL, &azm_trace_svr_oprs, NULL)) {
      az_printf("trace mon server: %s create reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (svr->oprs->start)(svr);
  } while (0);
  az_printf("trace mon server: %s start reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_stop()
{
  int r;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_trace_svr;
  do {
    if (azm_trace_thread_state && svr->state & AZ_TCPSERVER_STATE_CLIBUSY) { 
      r = azm_trace_stop_default_thread();
    }
    r = (svr->oprs->stop)(svr);
    if (r) break;
    r = az_tcpserver_delete(svr);
  } while (0);
  az_printf("trace mon server: %s stop reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_enable_remote(int onoff)
{
  int r;
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl; 
  az_trace_info_t info;

  if (azm_trace_thread_state) {
    info.code = AZ_TRACE_CODE_CMD_ENABLE;
    info.depth = (short)onoff;
    send(ctrl->write_fd, &info, sizeof(info), 0);
  } else {
    az_printf("%s", "remote target not connected\n");
    r = AZ_FAIL;
  }
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_send_query_cmd(short query, char *data, int datalen)
{
  int r;
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl; 
  az_trace_info_t info;

  if (azm_trace_thread_state) {
    info.code = AZ_TRACE_CODE_CMD_QUERY;
    info.query = query; 
    if (datalen > 0 && data != NULL) {
      if (datalen > sizeof(info.data)) datalen = sizeof(info.data);
      memcpy(info.data, data, datalen);
    }
    send(ctrl->write_fd, &info, sizeof(info), 0);
  } else {
    //az_sys_printf("%s", "remote target not connected\n");
    r = AZ_FAIL;
  }
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_proc_query_rsp(short query, char *data, int datalen)
{
  int r = AZ_SUCCESS;

  do {
    switch (query) {
      case AZ_TRACE_QUERY_CLIPORT:
        {
          uint16_t port = *(uint16_t *)data;
          az_printf("remot cli port = %u" AZ_NL, port);
        }
        break;
      default:
        r = AZ_ERR(INVALID);
        break;
    }
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *azm_trace_thread_proc_default(void *arg)
{
  int r;
  
  r = azm_trace_proc_default(&azm_trace_ctrl); 

  return NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  azm_trace_start_default_thread()
{
  int r;

  if (azm_trace_thread_state == 0) {
    azm_trace_thread_default = NULL;
    r = az_thread_create("rtraceDefault", azm_trace_thread_proc_default, NULL, NULL, &azm_trace_thread_default);
  } else {
    r = AZ_ERR(AGAIN);
    az_cli_printf("already started!\n");
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  azm_trace_stop_default_thread()
{
  int r;
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl;

  if (azm_trace_thread_state) {
    azm_trace_thread_state = 0;
    while (ctrl->state & AZ_TRACE_STATE_BUSY) {
      az_thread_sleep(1000000);
    }
    ctrl->state = AZ_TRACE_STATE_IDLE;
  } else {
    r = AZ_ERR(AGAIN);
    az_cli_printf("already stopped!\n");
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_cli_cmd(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;
  azm_trace_ctrl_t *ctrl = &azm_trace_ctrl;

  do {
    if (argc < 2) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    if (!strcmp(argv[1], "s")) { 
      if (argc > 2) {
        strncpy(ctrl->filepath, argv[2], sizeof(ctrl->filepath)); 
      }
      if (argc > 3) {
        strncpy(ctrl->prbSvrIpStr, argv[3], sizeof(ctrl->prbSvrIpStr)); 
      }
      if (argc > 4) {
        ctrl->prbSvrPort = strtol(argv[4], NULL, 10);
      }
      az_cli_printf("start trace %s on %s:%u\n", ctrl->filepath, ctrl->prbSvrIpStr, ctrl->prbSvrPort);

      azm_trace_start_default_thread();
      break;
    }
    if (!strcmp(argv[1], "t")) { 
      azm_trace_stop_default_thread();
      break;
    }
    if (!strcmp(argv[1], "q")) { 
      r = AZ_CLI_CMD_SHOWHELP;
      if (argc < 3) {
        break;
      }
      if (!strcmp(argv[2], "cliport")) {
        r = azm_trace_send_query_cmd(AZ_TRACE_QUERY_CLIPORT, NULL, 0);
        if (r >= 0) {
          r = AZ_CLI_CMD_SUCCESS;
        }
      }
      break;
    }
    if (!strcmp(argv[1], "f")) { 
      if (argc < 3) {
        if (azm_trace_filter_show(&xu_filter)) {
          az_cli_printf("no xu filters\n");
        }
        if (azm_trace_filter_show(&func_filter)) {
          az_cli_printf("no function filters\n");
        }
        az_cli_printf("usage: %s %s [xa/xd/fa/fd] [name]\n", argv[0], argv[1]);
      } else if (argc == 4) {
        if (argv[2][0] == 'x') {
          if (argv[2][1] == 'a') {
            if (azm_trace_filter_add(&xu_filter, argv[3])) {
              az_cli_printf("add %s success\n", argv[3]);
            } else {
              az_cli_printf("add %s fail\n", argv[3]);
            }
          } else if (argv[2][1] == 'd') {
            azm_trace_filter_del(&xu_filter, argv[3]);
          }
        } else 
        if (argv[2][0] == 'f') {
          if (argv[2][1] == 'a') {
            azm_trace_filter_add(&func_filter, argv[3]);
          } else if (argv[2][1] == 'd') {
            azm_trace_filter_del(&func_filter, argv[3]);
          }
        }
      }
      break;
    }
    int onoff = strtol(argv[1], NULL, 10);
    if (onoff == 1 || onoff == 0) {
      if (argc < 3) {
        azm_trace_enable_remote(onoff);
        break;
      }
      void *xu = strtol(argv[2], NULL, 16);
      az_cli_printf("rt %s %s\n", argv[2], onoff? "on":"off");
    }
    r = AZ_CLI_CMD_SHOWHELP;
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_init()
{
  int r;
  az_cli_cmd_reg("rt", azm_trace_cli_cmd, "rtrace [s/t/0/1/q/f] [thread/cliport]\t\t\t; remote trace control", 0);

  azm_mcheck_init();

  r = azm_trace_start();
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_trace_deinit()
{
  int r;

  r = azm_trace_stop();
  return r;
}
#if 0
#endif
#endif // __GNUC__

/* === end of AZM_TRACE_C === */
