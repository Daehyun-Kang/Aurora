/**
 * @file   az_unitTest.h
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

#ifndef AZ_UNITTEST_H
#define AZ_UNITTEST_H

#include "az_args.h"

#define AZ_UT_FLAG_TRACE       1


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  CONFIG_UNIT_TEST

/* define constants */

/* define basic macros */

extern  int az_ut_flags;

/* define basic types */
typedef int (*AZ_UnitTest_cb_t)(void *);

/* define structures */
struct az_ut_testProjResult; 
typedef struct az_ut_testCaseResult {
  int   index;
  char  *name;
  int   pass;
  int   fail;
  int   success;
  int   failure;

  struct az_ut_testProjResult *testProj; 
  struct az_ut_testCaseResult *next;
} az_ut_testCaseResult_t;

typedef struct az_ut_testProjResult {
  int   index;
  char  *name;
  int   testCaseCount;
  int   pass;
  int   fail;
  int   success;
  int   failure;

  az_ut_testCaseResult_t *tcHead;
  az_ut_testCaseResult_t *tcTail;

  struct az_ut_testProjResult *next;
} az_ut_testProjResult_t;

typedef struct az_ut_result {
  int testProjCount;

  az_ut_testProjResult_t  *tpHead;
  az_ut_testProjResult_t  *tpTail;
} az_ut_result_t;

static inline void az_ut_result_init(az_ut_result_t *p) 
{
  p->testProjCount = 0;
  p->tpHead = p->tpTail = NULL;
}
static inline void az_ut_add_testProjResult(az_ut_result_t *p, az_ut_testProjResult_t *r) 
{
  if (p->testProjCount == 0) {
    p->tpHead = p->tpTail = r;
    r->index = p->testProjCount;
    p->testProjCount++;
  } else {
    p->tpTail->next = r;
    r->next = NULL;
    p->tpTail = r;
    r->index = p->testProjCount;
    p->testProjCount++;
  }
}
static inline void az_ut_testProjResult_init(az_ut_testProjResult_t *p, char *name) 
{
  p->name = name;
  p->testCaseCount = 0;
  p->tcHead = p->tcTail = NULL;
  p->pass = 0;
  p->fail = 0;
  p->success = 0;
  p->failure = 0;
  p->next = NULL;
}
static inline void az_ut_add_testCaseResult(az_ut_testProjResult_t *p, az_ut_testCaseResult_t *r) 
{
  if (p->testCaseCount == 0) {
    p->tcHead = p->tcTail = r;
    r->index = p->testCaseCount;
    p->testCaseCount++;
  } else {
    p->tcTail->next = r;
    r->next = NULL;
    p->tcTail = r;
    r->index = p->testCaseCount;
    p->testCaseCount++;
  }
  r->testProj = p;
}
static inline void az_ut_testCaseResult_init(az_ut_testCaseResult_t *p, char *name) 
{
  p->name = name;
  p->pass = 0;
  p->fail = 0;
  p->success = 0;
  p->failure = 0;
  p->next = NULL;
}
static inline void  az_ut_add_testCaseResult_update(az_ut_testCaseResult_t *p, int pass, int fail, int success, int failure)
{
  az_ut_testProjResult_t *q = p->testProj; 
  p->pass = pass; q->pass += pass;
  p->fail = fail; q->fail += fail;
  p->success = success; q->success += success;
  p->failure = failure; q->failure += failure;
}

extern az_ut_result_t az_ut_result;

#define AZ_UT_RESULT_CUR() ((az_ut_result_t *)&az_ut_result)
#define AZ_UT_CurTestProjResult() (AZ_UT_RESULT_CUR()->tpTail)
#define AZ_UT_CurTestCaseResult() (AZ_UT_CurTestProjResult()->tcTail)
/* define structured types */

/* define macros */
#define FILE_NULL     NULL
#define REASON_NULL   NULL

#define AZ_UT_RESULT_SUCCESS  0
#define AZ_UT_RESULT_ERROR    -1
#define AZ_UT_RESULT_SEGV     -11

// define test vectors
#define AZ_UT_TESTVECTOR(func) struct testvector_##func 
#define AZ_UT_DECL_TESTVECTOR_VAR(func, var) AZ_UT_TESTVECTOR(func) (var)
#define AZ_UT_DECL_TESTVECTOR_PTR(func, var) AZ_UT_TESTVECTOR(func) *(var)
#define AZ_UT_INIT_TESTVECTOR_VAR(func, var, val) \
  AZ_UT_TESTVECTOR(func) (var) = (val)
#define AZ_UT_INIT_TESTVECTOR_PTR(func, var, val) \
  AZ_UT_TESTVECTOR(func) *(var) = (AZ_UT_TESTVECTOR(func) *)(val)
#define AZ_UT_DECL_TESTVECTOR_CUR(func) \
  AZ_UT_TESTVECTOR(func) *pCurTestVector_##func
#define AZ_UT_TESTVECTOR_CUR(func) (pCurTestVector_##func)

#define _AZ_UT_TESTVECTOR_ST1(func) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason;};

#define _AZ_UT_TESTVECTOR_ST2(func, t0) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; };

#define _AZ_UT_TESTVECTOR_ST3(func, t0, t1 ) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; };

#define _AZ_UT_TESTVECTOR_ST4(func, t0, t1, t2) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; t2 arg2; };

#define _AZ_UT_TESTVECTOR_ST5(func, t0, t1, t2, t3) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; t2 arg2; t3 arg3};

#define _AZ_UT_TESTVECTOR_ST6(func, t0, t1, t2, t3, t4) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; t2 arg2; t3 arg3; t4 arg4;};

#define _AZ_UT_TESTVECTOR_ST7(func, t0, t1, t2, t3, t4, t5) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; t2 arg2; t3 arg3; t4 arg4; t5 arg5;};

#define _AZ_UT_TESTVECTOR_ST8(func, t0, t1, t2, t3, t4, t5, t6) \
  AZ_UT_TESTVECTOR(func) { int _index; AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog; char *_file; int _line; char *_reason; t0 arg0; t1 arg1; t2 arg2; t3 arg3; t4 arg4; t5 arg5; t6 arg6;};

#define AZ_UT_TESTVECTOR_ST(...) VFUNC(_AZ_UT_TESTVECTOR_ST, __VA_ARGS__)

#define az_ut_arg(n) arg##n

//define test vector array
#define AZ_UT_TESTVECTOR_ARRAY(val, n, func, ...)\
  AZ_UT_TESTVECTOR(func) (val)[n] = { __VA_ARGS__ };

// defined test run context
#define AZ_UT_TESTRUNCTX(func) struct testrun_##func 
#define AZ_UT_DECL_TESTRUNCTX_VAR(func, var) AZ_UT_TESTRUNCTX(func) (var) 
#define AZ_UT_DECL_TESTRUNCTX_PTR(func, var) AZ_UT_TESTRUNCTX(func) *(var) 
#define AZ_UT_INIT_TESTRUNCTX_VAR(func, var, val) AZ_UT_TESTRUNCTX(func) (var) = (val) 
#define AZ_UT_INIT_TESTRUNCTX_PTR(func, var, val) AZ_UT_TESTRUNCTX(func) *(var) = (AZ_UT_TESTRUNCTX(func) *)(val) 

#define _AZ_UT_TESTRUNCTX_ST_DEFAULT(func) \
  char *_title; int _argc; char **_argv; \
  AZ_UT_TESTVECTOR(func) *_input; int _result;\
  AZ_UnitTest_cb_t _prolog; AZ_UnitTest_cb_t _epilog;\
  AZ_UT_TESTVECTOR(func) *_tv; int _tv_count;\
  int _pass; int _fail;\
  int _success; int _failure\

#define _AZ_UT_TESTRUNCTX_ST1(func) \
  AZ_UT_TESTRUNCTX(func) {_AZ_UT_TESTRUNCTX_ST_DEFAULT(func);};

#define _AZ_UT_TESTRUNCTX_ST2(func, a0) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;};

#define _AZ_UT_TESTRUNCTX_ST3(func, a0, a1) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;};

#define _AZ_UT_TESTRUNCTX_ST4(func, a0, a1, a2) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;a2;};

#define _AZ_UT_TESTRUNCTX_ST5(func, a0, a1, a2, a3) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;a2;a3;};

#define _AZ_UT_TESTRUNCTX_ST6(func, a0, a1, a2, a3, a4) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;a2;a3;a4;};

#define _AZ_UT_TESTRUNCTX_ST7(func, a0, a1, a2, a3, a4, a5) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;a2;a3;a4;a5;};
#define _AZ_UT_TESTRUNCTX_ST8(func, a0, a1, a2, a3, a4, a5, a6) \
  AZ_UT_TESTRUNCTX(func) { _AZ_UT_TESTRUNCTX_ST_DEFAULT(func);a0;a1;a2;a3;a4;a5;a6;};

#define AZ_UT_TESTRUNCTX_ST(...) VFUNC(_AZ_UT_TESTRUNCTX_ST, __VA_ARGS__)

// define test run functions
#define _AZ_UT_EXE_FUNC2(func, args) (func)()
#define _AZ_UT_EXE_FUNC3(func, args, t0) args.arg0 = (func)()
#define _AZ_UT_EXE_FUNC4(func, args, t0, t1) args.arg0 = (func)(args.arg1)
#define _AZ_UT_EXE_FUNC5(func, args, t0, t1, t2) args.arg0 = (func)(args.arg1, args.arg2)
#define _AZ_UT_EXE_FUNC6(func, args, t0, t1, t2, t3) args.arg0 = (func)(args.arg1, args.arg2, args.arg3)
#define _AZ_UT_EXE_FUNC7(func, args, t0, t1, t2, t3, t4) args.arg0 = (func)(args.arg1, args.arg2, args.arg3, args.arg4)
#define _AZ_UT_EXE_FUNC8(func, args, t0, t1, t2, t3, t4, t5) args.arg0 = (func)(args.arg1, args.arg2, args.arg3, args.arg4, args.arg5)
#define _AZ_UT_EXE_FUNC9(func, args, t0, t1, t2, t3, t4, t5, t6) args.arg0 = (func)(args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6)

#define AZ_UT_EXE_FUNC(...) VFUNC(_AZ_UT_EXE_FUNC, __VA_ARGS__)

#define _AZ_UT_EXE_VOID_FUNC2(func, args) (func)()
#define _AZ_UT_EXE_VOID_FUNC3(func, args, t0)  (func)(args.arg0)
#define _AZ_UT_EXE_VOID_FUNC4(func, args, t0, t1) (func)(args.arg0, args.arg1)
#define _AZ_UT_EXE_VOID_FUNC5(func, args, t0, t1, t2) (func)(args.arg0, args.arg1, args.arg2)
#define _AZ_UT_EXE_VOID_FUNC6(func, args, t0, t1, t2, t3) (func)(args.arg0, args.arg1, args.arg2, args.arg3)
#define _AZ_UT_EXE_VOID_FUNC7(func, args, t0, t1, t2, t3, t4) (func)(args.arg0, args.arg1, args.arg2, args.arg3, args.arg4)
#define _AZ_UT_EXE_VOID_FUNC8(func, args, t0, t1, t2, t3, t4, t5) (func)(args.arg0, args.arg1, args.arg2, args.arg3, args.arg4, args.arg5)
#define _AZ_UT_EXE_VOID_FUNC9(func, args, t0, t1, t2, t3, t4, t5, t6) (func)(args.arg0, args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6)

#define AZ_UT_EXE_VOID_FUNC(...) VFUNC(_AZ_UT_EXE_VOID_FUNC, __VA_ARGS__)

#define AZ_UT_RUN(args, i, pCtx, func, ... ) \
  while (1) {\
    extern AZ_UT_DECL_TESTVECTOR_CUR(func); \
    int _err = 0;\
    int (*_cb_f)(AZ_UT_TESTRUNCTX(func) *);\
    AZ_UT_TESTVECTOR(func) *_input = NULL;\
    AZ_UT_TESTRUNCTX(func) *_ctx = (AZ_UT_TESTRUNCTX(func) *)(pCtx);\
    if (_ctx) { \
      ((AZ_UT_TESTRUNCTX(func) *)(_ctx))->_input = (args) + i;\
      _input = ((AZ_UT_TESTRUNCTX(func) *)(_ctx))->_input;\
      pCurTestVector_##func = _input; \
    };\
    if (_ctx) { \
      if (_input->_prolog) {\
       _cb_f = _input->_prolog;\
       if ((_err = (_cb_f)(_ctx)) < 0) { \
        break;\
       } \
      }\
      AZ_UT_EXE_FUNC(func, (args)[i], __VA_ARGS__);\
      if (_input->_epilog) { \
       _cb_f = _input->_epilog;\
       _err = (_cb_f)(_ctx);\
       if (_err) _ctx->_fail++;\
       else _ctx->_pass++;\
      }\
    }\
    break;\
  }

#define AZ_UT_RUN_VOID(args, i, pCtx, func, ... ) \
  while (1) {\
    extern AZ_UT_DECL_TESTVECTOR_CUR(func); \
    int _err = 0;\
    int (*_cb_f)(AZ_UT_TESTRUNCTX(func) *);\
    AZ_UT_TESTVECTOR(func) *_input = NULL;\
    AZ_UT_TESTRUNCTX(func) *_ctx = (AZ_UT_TESTRUNCTX(func) *)(pCtx);\
    if (_ctx) { \
      ((AZ_UT_TESTRUNCTX(func) *)(_ctx))->_input = (args) + i;\
      _input = ((AZ_UT_TESTRUNCTX(func) *)(_ctx))->_input;\
      pCurTestVector_##func = _input; \
    };\
    if (_ctx) { \
      if (_input->_prolog) {\
       _cb_f = _input->_prolog;\
       if ((_err = (_cb_f)(_ctx)) < 0) { \
        break;\
       } \
      }\
      AZ_UT_EXE_VOID_FUNC(func, args[i], __VA_ARGS__);\
      if (_input->_epilog) { \
       _cb_f = _input->_epilog;\
       _err = (_cb_f)(_ctx);\
       if (_err) _ctx->_fail++;\
       else _ctx->_pass++;\
      }\
    }\
    break;\
  }


#define AZ_UT_TEST_MAIN(func, ...)  az_ut_main_##func(__VA_ARGS__)
#define CALL_AZ_UT_SET_REASON(func, ...) az_ut_set_reason_##func(__VA_ARGS__)

#define AZ_UT_SET_REASON(func, file, line, ...) {\
  extern AZ_UT_DECL_TESTVECTOR_CUR(func); \
  char reason_buf[1024], *str;\
  int reason_len = snprintf(reason_buf, sizeof(reason_buf), __VA_ARGS__);\
  str = malloc(reason_len+1); strcpy(str, reason_buf);\
  CALL_AZ_UT_SET_REASON(func, file, line, str);\
}

#define AZ_UT_SET_OK(func) {\
  extern AZ_UT_DECL_TESTVECTOR_CUR(func); \
  CALL_AZ_UT_SET_REASON(func, NULL, -1, NULL);\
}

#define ARRAY_SIZE(a)   (sizeof(a)/sizeof((a)[0]))

#ifdef AZ_UT_FUNC_HAS_RETURN 
#define AZ_UT_DEF_TEST_MAIN(title, prolog, epilog, tvs, func, ... ) \
az_ut_testCaseResult_t  az_ut_testCaseResult_##func;\
int  AZ_UT_TEST_MAIN(func, int argc, char *argv[]) {\
  AZ_UT_DECL_TESTRUNCTX_VAR(func, ctx) = {\
  ._title = title,\
  ._argc = argc,\
  ._argv = argv,\
  ._prolog = prolog,\
  ._epilog = epilog,\
  ._tv = &tvs,\
  ._tv_count = ARRAY_SIZE(tvs),\
  ._pass = 0,\
  ._fail = 0,\
  ._success = 0,\
  ._failure = 0,\
  ._result = -1}; \
  int i, _err = 0;\
  int (*_cb_f)(AZ_UT_TESTRUNCTX(func) *);\
  az_ut_testCaseResult_t  *pCurTCR = &az_ut_testCaseResult_##func;\
  az_ut_testCaseResult_init(pCurTCR, #func);\
  az_ut_add_testCaseResult(AZ_UT_CurTestProjResult(),pCurTCR);\
    if (ctx._prolog) { \
        _cb_f = ctx._prolog;\
        _err = (_cb_f)(&ctx); \
    } \
    /*if (!_err) {*/\
        for (i = 0; i < ctx._tv_count; i++) {\
            AZ_UT_RUN(ctx._tv, i, &ctx, func, __VA_ARGS__); \
        }\
    /*}*/\
    if (ctx._epilog) { \
        _cb_f = ctx._epilog;\
        _err = (_cb_f)(&ctx);\
    }\
  az_ut_add_testCaseResult_update(pCurTCR,ctx._pass, ctx._fail, ctx._success, ctx._failure);\
    return _err;\
}
#else
#define AZ_UT_DEF_TEST_MAIN(title, prolog, epilog, tvs, func, ... ) \
az_ut_testCaseResult_t  az_ut_testCaseResult_##func;\
int  AZ_UT_TEST_MAIN(func, int argc, char *argv[]) {\
  AZ_UT_DECL_TESTRUNCTX_VAR(func, ctx) = {\
  ._title = title,\
  ._argc = argc,\
  ._argv = argv,\
  ._prolog = prolog,\
  ._epilog = epilog,\
  ._tv = &tvs,\
  ._tv_count = ARRAY_SIZE(tvs),\
  ._pass = 0,\
  ._fail = 0,\
  ._success = 0,\
  ._failure = 0,\
  ._result = -1}; \
  int i, _err = 0;\
  int (*_cb_f)(AZ_UT_TESTRUNCTX(func) *);\
  az_ut_testCaseResult_t  *pCurTCR = &az_ut_testCaseResult_##func;\
  az_ut_testCaseResult_init(pCurTCR, #func);\
  az_ut_add_testCaseResult(AZ_UT_CurTestProjResult(),pCurTCR);\
    if (ctx._prolog) { \
        _cb_f = ctx._prolog;\
        _err = (_cb_f)(&ctx); \
    } \
    /*if (!_err) {*/\
        for (i = 0; i < ctx._tv_count; i++) {\
            AZ_UT_RUN_VOID(ctx._tv, i, &ctx, func, __VA_ARGS__); \
        }\
    /*}*/\
    if (ctx._epilog) { \
        _cb_f = ctx._epilog;\
        _err = (_cb_f)(&ctx);\
    }\
  az_ut_add_testCaseResult_update(pCurTCR,ctx._pass, ctx._fail, ctx._success, ctx._failure);\
    return _err;\
}
#endif

#ifdef AZ_UT_FUNC_HAS_RETURN
#undef AZ_UT_FUNC_HAS_RETURN 
#endif
#ifdef AZ_UT_FUNC_VOID
#undef AZ_UT_FUNC_VOID
#endif

#define AZ_UT_PRINT_B(pCtx) \
    do { \
    printf("\n\n[B] [%s] test run [%d]\n",pCtx->_title, pCtx->_tv_count);\
    } while (0);

#define AZ_UT_PRINT_BEGIN(pCtx) AZ_UT_PRINT_B(pCtx)

#define AZ_UT_PRINT_T(pCtx) \
  do {\
    printf("[T] [%s] test run [%d] pass [%d] fail [%d]\n",pCtx->_title, pCtx->_tv_count, pCtx->_pass, pCtx->_fail); \
  } while (0);

#define AZ_UT_PRINT_TERM(pCtx) AZ_UT_PRINT_T(pCtx)

#define AZ_UT_PRINT_START(pCtx, pInput) \
  do { \
    printf("[S] [%s] test run [%d]\n",pCtx->_title, pInput->_index); \
  } while (0); 

#define AZ_UT_PRINT_END_FAIL(pCtx, pInput, err) \
  do {\
    printf("[E] [%s] test run [%d] [%s] [%d] [%s:%d] [%s]\n",\
            pCtx->_title, pInput->_index,\
          (!err)? "PASS":"FAIL", err,\
            pInput->_file, pInput->_line, pInput->_reason);\
  } while (0);

#define AZ_UT_PRINT_END_PASS(pCtx, pInput, err) \
  do {\
    printf("[E] [%s] test run [%d] [%s] [%d] \n",\
            pCtx->_title, pInput->_index,\
          (!err)? "PASS:success":"PASS:failure", err); \
    if (!err) pCtx->_success++;\
    else pCtx->_failure++; \
  } while (0);
/* variabls exposed */

/* inline functions */

/* function prototypes exposed*/

#else
#define AZ_UT_SET_REASON(func, ...) 
#define AZ_UT_SET_OK(func, ...) 

#endif

#ifdef __cplusplus
}
#endif


#endif
