/**
 * @file   az_xu_types.h
 * @brief  
 * @date   10/04/16
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
 *   - 2016-04-10 fware initial create
 */

#ifndef AZ_XU_TYPES_H
#define AZ_XU_TYPES_H


#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_XU_PRIORITY_MIN     1
#define AZ_XU_PRIORITY_MAX     99


/* basic macros */

/* basic types */
typedef uint64_t az_xu_core_mask_t;
typedef struct {
    int sched_policy;
    int priority;
    //int cpu_no;
    //int process_no;
    az_xu_core_mask_t core_mask;
    az_size_t   stack_size;

    int startdelay;
} az_xu_attr_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
