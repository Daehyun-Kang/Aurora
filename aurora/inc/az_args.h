/**
 * @file   az_args.h
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

#ifndef ARR_ARGS_H
#define ARR_ARGS_H

/* constants */

/* basic macros */
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__,\
                        30,29,28,27,26,25,24,23,22,21,\
                        20,19,18,17,16,15,14,13,12,11,\
                        10,9,8,7,6,5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,\
                         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,\
                         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30,\
                        N,...) N

#define _VFUNC_(name, n)  name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, VA_NUM_ARGS(__VA_ARGS__)) (__VA_ARGS__)

#ifdef __cplusplus
extern "C"
{
#endif

/* basic types */

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
