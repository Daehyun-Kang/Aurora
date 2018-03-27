#!/usr/bin/env python
import sys
import os
import logging
import datetime

from mkcommon import *

reload(sys)
sys.setdefaultencoding('utf-8')

cr_owner ='Fware'
cr_dates='2013-2017'

def mkfunctionDescrTemplate():
  fileInfo = "/**\n"
  fileInfo += " * @fn {0}\n".format("function name")
  fileInfo += " * @brief {0}\n".format("function-description")
  fileInfo += " * @param {0}\n".format("input-output-parameters")
  fileInfo += " * @return {0}\n".format("return-value")
  fileInfo += " * @exception {0}\n".format("none")
  fileInfo += " */\n\n\n"
  return fileInfo

def mkfunctionDescr(fn, brief, param, ret):
  fileInfo = "/**\n"
  fileInfo += " * @fn\t\t{0}\n".format(fn)
  fileInfo += " * @brief\t{0}\n".format(brief)
  fileInfo += " * @param\t{0}\n".format(param)
  fileInfo += " * @return\t{0}\n".format(ret)
  fileInfo += " * @exception\t{0}\n".format("none")
  fileInfo += " */\n"
  return fileInfo

UnitTestSysHeaderFile="aurora.h"
UnitTestVarPrefix="az_ut"
UnitTestFuncHasReturn='AZ_UT_FUNC_HAS_RETURN';
UnitTestFuncIsVoid='AZ_UT_FUNC_VOID';
UnitTestHeaderFile='az_unitTest.h';
AZTestHeaderFile='az_test.h';
AZTestCaseStruct='az_test_case_t';
AZTestCaseHandlerRetType='az_r_t';
AZTestCaseHandlerType='az_test_case_handler_t';
UnitTestFlags="az_ut_flags"
UnitTestTraceEnable="AZ_UT_FLAG_TRACE"

UnitTestCbType='AZ_UnitTest_cb_t';

UnitTestVectorStruct="az_ut_testvector_st"
UnitTestVectorArray="az_ut_testvector_array"

UnitTestRunCtxStruct="AZ_UT_TESTRUNCTX_ST"
UnitTestVectorCurDecl="AZ_UT_DECL_TESTVECTOR_CUR"
UnitTestVectorCur="AZ_UT_TESTVECTOR_CUR"
UnitTestInitRunCtxPtr="AZ_UT_INIT_TESTRUNCTX_PTR"
UnitTestInitVectorPtr="AZ_UT_INIT_TESTVECTOR_PTR"
UnitTestDefTestMain="AZ_UT_DEF_TEST_MAIN"
UnitTestMain="AZ_UT_TEST_MAIN"
UnitTestSetReason="az_ut_set_reason";
UnitTestPrintStart="AZ_UT_PRINT_START";
UnitTestPrintEndFail="AZ_UT_PRINT_END_FAIL";
UnitTestPrintEndPass="AZ_UT_PRINT_END_PASS";
UnitTestPrintBegin="AZ_UT_PRINT_BEGIN";
UnitTestPrintTerm="AZ_UT_PRINT_TERM";

def mkDeclareUnitTestVars(f, argn, *args):
  print "{0} {1}".format(argn, args)
  i = 0
  for arg in args:
    print "arg:",args[i]
    i=i+1

def mkDeclareUnitTestTargetFunction(f, funcdecl):
  f.write("/* this is unit test target function */\n");
  s = "extern {0};\n\n".format(funcdecl)
  f.write(s)


def mkfname(prefix, fn):
  return prefix + "_" + fn

def mkflags(fn):
  return mkfname(mkfname(UnitTestVarPrefix, fn), 'flags')

def mkfnHandler(hn, fn):
  return mkfname(UnitTestVarPrefix, mkfname(hn, fn))

def mkfnepilog(fn):
  return mkfname(UnitTestVarPrefix, mkfname("epilog", fn))

def mkfnCtxProlog(fn):
  return mkfname(UnitTestVarPrefix, mkfname(mkfname("prolog", "ctx"), fn))

def mkfnCtxEpilog(fn):
  return mkfname(UnitTestVarPrefix, mkfname(mkfname("epilog", "ctx"), fn))

def mkfnUTMain(fn):
  return mkfname(UnitTestVarPrefix, mkfname("main", fn))

def declTCInitHandler(f, fn):
  fname=mkfnHandler("init", fn)
  f.write("/* this is init handler for each test case */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def declTCSyncHandler(f, fn):
  fname=mkfnHandler("sync", fn)
  f.write("/* this is sync handler for each test case */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def declTCPrologHandler(f, fn):
  fname=mkfnHandler("prolog", fn)
  f.write("/* this is prolog handler for each test iteration */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def declTCRunHandler(f, fn):
  fname=mkfnHandler("run", fn)
  f.write("/* this is run handler for each test iteration */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def declTCEpilogHandler(f, fn):
  fname=mkfnHandler("epilog", fn)
  f.write("/* this is epilog handler for each test iteration */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def declTCTermHandler(f, fn):
  fname=mkfnHandler("term", fn)
  f.write("/* this is term handler for each test case */\n");
  f.write("extern {0} {1}({2} *pTC);\n\n".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))

def defTestVectorStruct(f, fn, rettype, args):
  f.write("/* this is to define test vector structure */\n");
  struct_name = mkfname(UnitTestVectorStruct, fn)
  f.write("struct {0} ".format(struct_name))
  f.write("{\n")
  argn = len(args)
  i=0
  while i < argn:
    f.write("  {0};\n".format(args[i].strip()))
    i=i+1
  if rettype != "void":
    f.write("  {0} expected_result;\n".format(rettype))
    f.write("  {0} result;\n".format(rettype))
  f.write("};\n\n")

def defTestVectorArray(f, fn):
  f.write("/* define the reference for test vectors */\n")
  struct_name = mkfname(UnitTestVectorStruct, fn)
  array_name = mkfname(UnitTestVectorArray, fn)
  f.write("struct {0} *{1} = NULL;\n\n".format(struct_name, array_name))

def declTestVectorCur(f, fn):
  f.write("{0}({1});\n\n".format(UnitTestVectorCurDecl, fn))

def insertTrace(f, fn):
  f.write("  if (({0} & {2}) || ({1} & {2})) ".format(mkflags(fn), UnitTestFlags, UnitTestTraceEnable))
  f.write("{\n")
  f.write("    az_rprintf(r, \"%s\" AZ_NL, \"...\");\n")
  f.write("  }\n")

def defTCInitHandler(f, fn):
  fname=mkfnHandler("init", fn)
  struct_name = mkfname(UnitTestVectorStruct, fn)
  array_name = mkfname(UnitTestVectorArray, fn)
  argname = "pTC"
  attrTcIterCount="test_iter_count"
  attrTestVector="test_vector"
  f.write(mkfunctionDescr(fname, "called for init test case", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *{3})".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct, argname))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  f.write("  if ({0}->{1} > 0) ".format(argname, attrTcIterCount)) 
  f.write("{\n")
  f.write("    {0} = (struct {1} *)az_malloc(sizeof({2}->{3}));\n".format(array_name, struct_name, argname, attrTcIterCount)) 
  f.write("    if (NULL == {0}) ".format(array_name))
  f.write("{\n")
  f.write("      r = AZ_ERR(MALLOC);\n") 
  f.write("    }\n")
  f.write("  }\n")
  f.write("  {0}->{1} = (void *){2};\n".format(argname, attrTestVector, array_name)) 
  f.write("  /* here you may want make up the test vector array values from xml configuration file */\n\n");
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCSyncHandler(f, fn):
  fname=mkfnHandler("sync", fn)
  f.write(mkfunctionDescr(fname, "called for prepare test case", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCPrologHandler(f, fn):
  fname=mkfnHandler("prolog", fn)
  f.write(mkfunctionDescr(fname, "called before each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCRunHandler(f, fn):
  fname=mkfnHandler("run", fn)
  f.write(mkfunctionDescr(fname, "called for each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCEpilogHandler(f, fn):
  fname=mkfnHandler("epilog", fn)
  f.write(mkfunctionDescr(fname, "called after each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCTermHandler(f, fn):
  fname=mkfnHandler("term", fn)
  struct_name = mkfname(UnitTestVectorStruct, fn)
  array_name = mkfname(UnitTestVectorArray, fn)
  argname = "pTC"
  attrTcIterCount="test_iter_count"
  attrTestVector="test_vector"
  f.write(mkfunctionDescr(fname, "called at test case terminating phase", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  f.write("  /* free the test vector array values from xml configuration file */\n\n");
  f.write("  if (NULL != {0}) ".format(array_name))
  f.write("{\n")
  f.write("    az_free({0});\n".format(array_name)) 
  f.write("    {0} = NULL;\n".format(array_name)) 
  f.write("    {0}->{1} = NULL;\n".format(argname, attrTestVector)) 
  f.write("  }\n\n")
  insertTrace(f, fn)
  f.write("  return r;\n");
  f.write("}\n\n");


def defSetReasonFunction(f, fname):
  declCurTestVector = UnitTestVectorCurDecl + "(" + fname + ");"
  curTestVector = UnitTestVectorCur + "(" + fname + ")"
  func = mkfname(UnitTestSetReason,fname)
  f.write(mkfunctionDescr(func, "called from macro AZ_UT_SET_REASON ", "file, line, reason", "void"))
  f.write("void {0}(char *file, int line, char *reason)\n".format(func))
  f.write("{\n");
  f.write("\textern "+declCurTestVector+"\n") 
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_reason", "reason"))
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_file", "file"))
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_line", "line"))
  f.write("}\n\n");

def parseFuncDecl(funcdecl):
  token = funcdecl.split(" ", 1)
  print token
  if len(token) != 2: 
    print "\"{0}\" is invalid func decl".format(funcdecl)
    return False
  rettype = token[0]
  token = token[1].split("(",1)
  #print token
  funcname = token[0]
  token = token[1].split(",")
  #print token
  argcount = len(token)
  token[argcount-1] = token[argcount-1].replace(")", "")
  token[argcount-1] = token[argcount-1].replace(";", "")
  funcstruct={'ret':rettype, 'name':funcname, 'args':token}
  return funcstruct

def argsToVector(rettype, args):
  argn = len(args)
  print args
  s = ""
  for arg in args:
    token = arg.strip().split(' ')
    s += "{0}=\"{1}\" ".format(token[1], token[0]) 

  s += "{0}=\"{1}\" ".format("expected_result", rettype) 
  s += "{0}=\"{1}\" ".format("result", rettype) 
  return s

def main():
  if len(sys.argv) < 3:
    print "usage: {0} filename funcdel".format(sys.argv[0])
    return

  #print "usage: {0} {1} {2}".format(sys.argv[0], sys.argv[1], sys.argv[2])
  username = get_user_name()
  brief = "" 

  filename=sys.argv[1]
  funcdecl = sys.argv[2]

  funcstruct = parseFuncDecl(funcdecl)

  if funcstruct == False:
    return
  
  #print funcstruct

  funcname = funcstruct['name'];
  funcret = funcstruct['ret'];
  funcargs = funcstruct['args'];

  if funcret == "void":
    func_has_return = False
  else:
    func_has_return = True

  filetag=filename.upper().replace('.', '_')
  fileinfo = mkfileinfo(filename, username, brief, cr_owner, cr_dates)

  f = open(sys.argv[1], "wb")
  f.write(fileinfo)
  f.write("/* include header files */\n")
  f.write("#include \"{0}\"\n".format(UnitTestSysHeaderFile))
  f.write("#include \"{0}\"\n".format(AZTestHeaderFile))
  f.write("/* TODO: include function dependant header files */\n\n")
  if func_has_return == False:
    f.write("#define {0}  1\n".format(UnitTestFuncIsVoid))
    f.write("#undef {0}  \n".format(UnitTestFuncHasReturn))
  else:
    f.write("#define {0}  1\n".format(UnitTestFuncHasReturn))
    f.write("#undef {0}  \n".format(UnitTestFuncIsVoid))
  f.write("#include \"{0}\"\n\n".format(UnitTestHeaderFile))
  f.write("/* declare global variables */\n\n")
  f.write("int {0} = {1};\n\n".format(mkflags(funcname), UnitTestTraceEnable))
  
  mkDeclareUnitTestTargetFunction(f, funcdecl)

  defTestVectorStruct(f, funcname, funcret, funcargs)
  defTestVectorArray(f, funcname)

  f.write("/* declare global callback functions */\n\n")
  declTCInitHandler(f, funcname)
  declTCSyncHandler(f, funcname)
  declTCPrologHandler(f, funcname)
  declTCRunHandler(f, funcname)
  declTCEpilogHandler(f, funcname)
  declTCTermHandler(f, funcname)

  f.write("\n\n")
  f.write("/* implement global callback functions */\n\n")
  functionInfo = mkfunctionDescrTemplate()
  f.write(functionInfo)
  defTCInitHandler(f, funcname)
  defTCSyncHandler(f, funcname)
  defTCPrologHandler(f, funcname)
  defTCRunHandler(f, funcname)
  defTCEpilogHandler(f, funcname)
  defTCTermHandler(f, funcname)
  f.write("/* === end of {0} === */\n".format(filetag))
  f.close();

  filenames=filename.split('.')
  tcname = UnitTestVarPrefix + '_' + filenames[0];
  filename = UnitTestVarPrefix +'_cfg_'+filenames[0] + '.xml'
  if os.path.isfile(filename):
    os.rename(filename, filename+'.bk')

  f = open(filename, "wb")

  xmlheader="<? xml version=\"1.0\" encoding=\"iso-8859-1\"?>"
  f.write("{0}\n".format(xmlheader));
  f.write("<testCase name=\"{0}\" prototype=\"{1}\">\n".format(tcname, funcdecl))
  f.write("  <oprs>\n")
  f.write("    <Init   name=\"{0}\" />\n".format(mkfnHandler("init", funcname)))
  f.write("    <Sync   name=\"{0}\" />\n".format(mkfnHandler("sync", funcname)))
  f.write("    <Prolog name=\"{0}\" />\n".format(mkfnHandler("prolog", funcname)))
  f.write("    <Run    name=\"{0}\" />\n".format(mkfnHandler("run", funcname)))
  f.write("    <Epilog name=\"{0}\" />\n".format(mkfnHandler("epilog", funcname)))
  f.write("    <Term   name=\"{0}\" />\n".format(mkfnHandler("term", funcname)))
  f.write("  </oprs>\n")
  f.write("  <testVector {0}>\n".format(argsToVector(funcret, funcargs)))
  f.write("    <row {0} _onoff=\"1\" />\n".format(argsToVector(funcret, funcargs)))
  f.write("  </testVector>\n")
  f.write("</testCase>".format(tcname))
  print filename

if __name__ == "__main__":
  main()
