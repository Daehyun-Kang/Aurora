/**
 * @file   az_prof.h
 * @brief  
 * @date   09/04/16
 * @author fware
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
 *   - 2016-04-09 fware initial create
 */

#ifndef AZ_PROF_H
#define AZ_PROF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define az_prof_tdiff(a,b)  ((a.tv_sec-b.tv_sec)*1E9 + (a.tv_nsec-b.tv_nsec))

/* basic types */
typedef struct az_prof_elm {
  az_sys_timespec_t t0, t1;
  az_int64_t count;
  az_int64_t time;
} az_prof_elm_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
static inline az_int64_t az_prof_getres(az_int64_t lcnt)
{
  az_sys_timespec_t a;
  az_sys_timespec_t res;
  az_int64_t j;

  clock_getres(CLOCK_REALTIME, &res);
  for (j = 1; j < lcnt; j++) {
    clock_getres(CLOCK_REALTIME, &a);
    res.tv_sec = (res.tv_sec + a.tv_sec)/2;
    res.tv_nsec = (res.tv_nsec + a.tv_nsec)/2;
  }

  return res.tv_sec*1E9 + res.tv_nsec;
}

static inline az_int64_t az_prof_measure_gettime(az_int64_t lcnt)
{
  az_sys_timespec_t t0, t1;
  az_sys_timespec_t a;
  az_int64_t i;

  clock_gettime(CLOCK_REALTIME, &t0);
  for (i = 0; i < lcnt; i++) {
    clock_gettime(CLOCK_REALTIME_COARSE, &a);
  }
  clock_gettime(CLOCK_REALTIME, &t1);

  return az_prof_tdiff(t1, t0);
}

static inline az_int64_t az_prof_measure_loop(az_int64_t lcnt)
{
  az_sys_timespec_t t0, t1;
  az_int64_t i;

  clock_gettime(CLOCK_REALTIME, &t0);
  for (i = 0; i < lcnt; i++) {
  }
  clock_gettime(CLOCK_REALTIME, &t1);

  return az_prof_tdiff(t1, t0);
}

static inline void az_prof_elm_init(az_prof_elm_t *p, az_int64_t lcnt)
{
  p->count = lcnt * 2;
  p->time = 0;
}
static inline void az_prof_elm_measure(az_prof_elm_t *p)
{
  az_sys_timespec_t t0, t1;
  if (p->count & 1) {
    clock_gettime(CLOCK_REALTIME, &(p->t1));
    if (0 == p->time) p->time = az_prof_tdiff(t1, t0);
    else { p->time  = (p->time + az_prof_tdiff(t1, t0))/2; }
  } else {
    clock_gettime(CLOCK_REALTIME, &(p->t0));
  }
  p->count--;
}
static inline void az_prof_elm_end(az_prof_elm_t *p)
{
  clock_gettime(CLOCK_REALTIME, &(p->t0));
}


/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
