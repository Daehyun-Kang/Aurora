/**
 * @file   az_perf.c
 * @brief  
 * @date   29/03/18
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
 *   - 2018-03-29 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"
#include "mon/az_debug.h"
#include "az_test.h"
#include "az_tc.h"

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


static az_xu_config_t xu_cfg = {
  .stackSize = 0,
  .policy = SCHED_FIFO,
  .priority = 99,
  .coremask = 0x0002,
  .startdelay = 0,
};
static az_xu_t xu = NULL;

static void *xu_perf_calibrate(void *arg)
{
  az_sys_timespec_t time, stime, etime, dtime;
  int j, cnt = CONFIG_AZ_PERF_MEASURE_LOOP_CNT_DFT/100;
  int k = 0, samples = CONFIG_AZ_PERF_MEASURE_SAMPLES_DFT/10;
  clockid_t id = CLOCK_MONOTONIC;
  uint64_t  t1, t2;
  uint64_t acc1 = 0, acc2 = 0, acc3 = 0, acc4 = 0;
  double acc_mhz = 0;
  uint64_t tsc = 0, csc = 0, lsc = 0;

  az_testproj_t *testproj = (az_testproj_t *)arg;

  while (k++ < samples) {
    // measure loop
    t1 = az_timestamp();
    for (j = 0; j < cnt; j++) {
      asm volatile ("nop");
    }
    t2 = az_timestamp();

    acc1 += (t2 - t1);

    // measure timestamp
    t1 = az_timestamp();
    for (j = 0; j < cnt; j++) {
      az_timestamp();
    }
    t2 = az_timestamp();

    acc3 += (t2 - t1);

    // measure clock_gettime
    t1 = az_timestamp();
    for (j = 0; j < cnt; j++) {
      clock_gettime(id, &time);
    }
    t2 = az_timestamp();

    acc2 += (t2 - t1);
  } 
  k--;
  // timestamp cycles
  tsc = (acc3 - acc1)/ (cnt*k);
  // clock_gettime cycles 
  csc = (acc2 - acc1) / (cnt*k);
  // loop cycles 
  lsc = (acc1 - tsc*k)/(cnt*k);

  cnt = CONFIG_AZ_PERF_MEASURE_LOOP_CNT_DFT;
  samples = CONFIG_AZ_PERF_MEASURE_SAMPLES_DFT;
  acc1 = 0, acc2 = 0, acc3 = 0, acc4 = 0;
  k = 0;
  uint64_t dur1, dur2, dur3;
  while (k++ < samples) {
    // measure loop
    t1 = az_timestamp();
    for (j = 0; j < cnt; j++) {
      asm volatile ("nop");
    }
    t2 = az_timestamp();

    acc1 += (t2 - t1);
    dur1 = t2 - t1 - tsc;

    // measure timestamp
    t1 = az_timestamp();
    for (j = 0; j < cnt; j++) {
      az_timestamp();
    }
    t2 = az_timestamp();

    acc3 += (t2 - t1);
    dur3 = t2 - t1 - tsc - lsc*cnt;

    // measure clock_gettime

    t1 = az_timestamp();
    clock_gettime(id, &stime);
    for (j = 0; j < cnt-1; j++) {
      clock_gettime(id, &time);
    }
    clock_gettime(id, &etime);
    t2 = az_timestamp();

    acc2 += (t2 - t1);

    dur2 = t2 - t1 - csc; // in cycles
    dtime = *az_timespec_diff(&etime, &stime); // in nano second
    acc4 = az_timespec_value(&dtime);

    double mhz = dur2*1000/acc4;
    acc_mhz += mhz;

    dur2 = t2 - t1 - csc - tsc - lsc*cnt; // in cycles

    // timestamp cycles sum
    tsc = (tsc*k + dur3/cnt)/(k+1); 
    // clock_gettime cycles sum
    csc = (csc*k + dur2/cnt)/(k+1); 
    // loop cycles sum
    lsc = (lsc*k + dur1/cnt)/(k+1);
  } 
  acc_mhz = acc_mhz/(k-1);

  uint64_t tsc1 = 0, csc1 = 0, lsc1 = 0;
  // timestamp cycles
  tsc1 = (acc3 - acc1)/ (cnt*k);
  // clock_gettime cycles 
  csc1 = (acc2 - acc1) / (cnt*k);
  // loop cycles 
  lsc1 = (acc1 - tsc*k)/(cnt*k);

  /*
  az_dlog("%ld/%f %ld/%f %ld/%f %f\n", 
      tsc1, (double)tsc1 * 1000/acc_mhz,
      csc1, (double)csc1 * 1000/acc_mhz,
      lsc1, (double)lsc1 * 1000/acc_mhz,
      acc_mhz);

  az_dlog("%ld/%f %ld/%f %ld/%f %f\n", 
      tsc, (double)tsc * 1000/acc_mhz,
      csc, (double)csc * 1000/acc_mhz,
      lsc, (double)lsc * 1000/acc_mhz,
      acc_mhz);
  */

    t1 = az_timestamp();
    for (j = 0; j < cnt*k; j++) {
      asm volatile ("nop");
    }
    t2 = az_timestamp();
    dur1 = ((t2 - t1) - tsc)/(cnt*k); 
    lsc = (dur1 + lsc1 + lsc)/3;

    t1 = az_timestamp();
    for (j = 0; j < cnt*k; j++) {
      az_timestamp();
    }
    t2 = az_timestamp();
    dur1 = ((t2 - t1) - tsc)/(cnt*k) - lsc; 
    tsc = (dur1 + tsc1 + tsc)/3;

    t1 = az_timestamp();
    for (j = 0; j < cnt*k; j++) {
      clock_gettime(id, &time);
    }
    t2 = az_timestamp();
    dur1 = ((t2 - t1) - tsc)/(cnt*k) - lsc; 
    csc = (dur1 + csc1 + csc)/3;

  az_ilog("%ld/%f %ld/%f %ld/%f %f\n", 
      tsc, (double)tsc * 1000/acc_mhz,
      csc, (double)csc * 1000/acc_mhz,
      lsc, (double)lsc * 1000/acc_mhz,
      acc_mhz);


  // measure clock_gettime 
  testproj->perf_cal.lsc = lsc; 
  testproj->perf_cal.tsc = tsc; 
  testproj->perf_cal.csc = csc; 
  testproj->perf_cal.mhz = acc_mhz; 

  az_xu_sendEvent(testproj->xu_id, AZ_XU_EVENT_TEST_STOPPED);
  return NULL;
}
/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_r_t az_perf_calibrate(az_testproj_t *testproj)
{
  az_r_t  r = AZ_SUCCESS;
  az_ion_id_t xu_id;
  az_xu_event_t received;

  do {
    xu = NULL;
    xu_id = az_xu_create("perfCal", xu_perf_calibrate, testproj, &xu_cfg, &xu);
    if (xu_id < 0) break;

    r = az_xu_recvEvent(0xffff, 0, -1, &received);
    if (r < 0) break;
    if (!(received & AZ_XU_EVENT_TEST_STOPPED)) {
      az_dlog("perf cal interrupted by event %x\n", received);
      az_xu_stop(xu_id);
      az_xu_delete(xu_id);
      xu = NULL;
      r = AZ_ERR(INTERRUPT);
      break;
    } else {
      az_dlog("perf lsc=%lu tsc=%lu csc=%lu cpu=%f MHz\n", 
          testproj->perf_cal.lsc,
          testproj->perf_cal.tsc,
          testproj->perf_cal.csc,
          testproj->perf_cal.mhz);
    }
  } while (0);
  
  return r;
}


/* === end of AZ_PERF_C === */
