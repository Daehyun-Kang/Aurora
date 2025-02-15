/**
 * @file   az_test_report.c
 * @brief  
 * @date   27/03/18
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
 *   - 2018-03-27 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"
#include "mon/az_debug.h"
#include "az_test.h"

/* declare global variables */

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

int az_test_testproj_report(az_testproj_t *testproj)
{
  az_assert(NULL != testproj);
  char *bp = az_thread_prtbuf;
  int blen = sizeof(az_thread_prtbuf);
  int nlen, tlen = 0;
  az_sys_timespec_t *dtime;

  char *hline = "--------------------------------------------------------------------------------";
  char *lfmt = "%s" AZ_NL;
  char *sfmt = "%3s %20s %10s %10s %10s %10s %10s" AZ_NL;
  char *ifmt = "%3d %20s %10d %10d %10d %10d %3d.%06d" AZ_NL;
  char *cfmt = "%3d %20s %10c %10c %10c %10c %3d.%06d" AZ_NL;

  printf(AZ_NL AZ_NL);
  printf(lfmt, "========================= AURORA FRAMEWORK TEST RESULT =========================" AZ_NL);
  _AZ_SNPRINTF(tlen, bp, blen, "Test Project:%s %24s", testproj->name, " ");
  nlen = az_print_timestampInDatetime(bp, blen, AZ_TIMESTAMP_STR, &testproj->stime);
  _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
  _AZ_SNPRINTF(tlen, bp, blen, " - ");
  nlen = az_print_timestampInDatetime(bp, blen, AZ_TIMESTAMP_STR, &testproj->etime);
  _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
  printf("%s" AZ_NL, az_thread_prtbuf); 

  printf(lfmt, hline);
  printf(sfmt, "No", "Test Case Name", 
                  "PASS", "FAIL", "SUCCESS", "FAILURE", "DURATION");
  printf(lfmt, hline);

  int j;
  az_testcase_t *testcase = testproj->testcaselist;


  for (j = 0; j < testproj->testcase_count; j++, testcase++) {
    dtime = az_timespec_diff(&testcase->etime, &testcase->stime);
    if (testcase->onoff) {
      printf(ifmt, testcase->index, testcase->name, 
        testcase->report.pass,
        testcase->report.fail,
        testcase->report.success,
        testcase->report.failure,
        dtime->tv_sec, dtime->tv_nsec/100000
        );
      continue;
    }
      printf(cfmt, testcase->index, testcase->name, '*', '*', '*', '*',0,0);
  }

  printf(lfmt, hline);

  dtime = az_timespec_diff(&testproj->etime, &testproj->stime);
  printf(ifmt, testproj->testcase_count, "TOTAL",
                  testproj->report.pass, testproj->report.fail,
                  testproj->report.success, testproj->report.failure,
                  dtime->tv_sec, dtime->tv_nsec/100000
                  );
  printf(lfmt, hline);
  
  printf(AZ_NL);

  if ((testproj->report.failure + testproj->report.fail) > 0) {
    int k;
    az_testiter_t *iter; 
    testcase = testproj->testcaselist;
    char *reason;
    int reason_len;
    printf("------------------------------- FAILURE LIST -----------------------------------" AZ_NL);
    printf("%20s %5s %6s  %-45s" AZ_NL, "Test Case Name","iter#", "result", "reason");
    printf(lfmt, hline);
    for (j = 0; j < testproj->testcase_count; j++, testcase++) {
      if (!testcase->onoff) { continue; }
      iter = testcase->test_iter.list;
      for (k = 0; k < testcase->test_iter.count; k++, iter++) {
        if (iter->result == AZ_SUCCESS) continue;
        if (iter->reason[0] == 0) continue;
        if ((reason_len = strlen(iter->reason)) > 45) {
          reason = iter->reason + (reason_len - 42);
          printf("%20s %5d %6d  ...%-42s" AZ_NL,  testcase->name, 
            iter->index, iter->result, reason);
        } else {
          reason = iter->reason;
          printf("%20s %5d %6d  %-45s" AZ_NL,  testcase->name, 
            iter->index, iter->result, reason);
        }
      }
    }
  }
  printf(lfmt, hline);
  printf(AZ_NL AZ_NL);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_test_perf_testproj_report(az_testproj_t *testproj)
{
  az_assert(NULL != testproj);
  char *bp = az_thread_prtbuf;
  int blen = sizeof(az_thread_prtbuf);
  int nlen, tlen = 0;
  az_sys_timespec_t *dtime;

  char *hline = "--------------------------------------------------------------------------------";
  char *lfmt = "%s" AZ_NL;
  char *sfmt = "%3s %20s %10s %10s %10s %10s %10s" AZ_NL;
  char *ifmt1 = "%3d %20s %10d %10d %10s %10s %3d.%06d" AZ_NL;
  char *ifmt2 = "%3s %20d %10s %10s %10ld %10.6f %3s.%-6s" AZ_NL;
  char *cfmt = "%3d %20s %10c %10c %10c %10c %3d.%06d" AZ_NL;

  printf(AZ_NL AZ_NL);
  printf(lfmt, "===================== AURORA FRAMEWORK PERFORMANCE TEST RESULT ==================" AZ_NL);
  _AZ_SNPRINTF(tlen, bp, blen, "Test Project:%s %24s", testproj->name, " ");
  nlen = az_print_timestampInDatetime(bp, blen, AZ_TIMESTAMP_STR, &testproj->stime);
  _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
  _AZ_SNPRINTF(tlen, bp, blen, " - ");
  nlen = az_print_timestampInDatetime(bp, blen, AZ_TIMESTAMP_STR, &testproj->etime);
  _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
  printf("%s" AZ_NL, az_thread_prtbuf); 

  printf(lfmt, hline);
  printf(sfmt, "No", "Test Case Name", 
                  "PASS", "FAIL", "CYCLES", "TIME(ns)", "DURATION");
  printf(lfmt, hline);

  int j, k;
  az_testcase_t *testcase = testproj->testcaselist;
  az_testiter_t *iter; 


  for (j = 0; j < testproj->testcase_count; j++, testcase++) {
    dtime = az_timespec_diff(&testcase->etime, &testcase->stime);
    if (testcase->onoff) {
      printf(ifmt1, testcase->index, testcase->name, 
        testcase->report.pass,
        testcase->report.fail,
        "-", "-",
        dtime->tv_sec, dtime->tv_nsec/100000
        );

      iter = testcase->test_iter.list;
      for (k = 0; k < testcase->test_iter.count; k++, iter++) {
      printf(ifmt2, " ", iter->samples, "-", "-", 
        iter->result, (double)(iter->result)*1000/testproj->perf_cal.mhz,
        "-","-");
      }
      continue;
    }
      printf(cfmt, testcase->index, testcase->name, '*', '*', '*', '*',0,0);
  }

  printf(lfmt, hline);

  dtime = az_timespec_diff(&testproj->etime, &testproj->stime);
  printf(ifmt1, testproj->testcase_count, "TOTAL",
                  testproj->report.pass, testproj->report.fail,
                  "-", "-",
                  dtime->tv_sec, dtime->tv_nsec/100000
                  );
  printf(lfmt, hline);
  
  printf(AZ_NL AZ_NL);
}
/* === end of AZ_TEST_REPORT_C === */
