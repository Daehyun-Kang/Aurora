#!/usr/bin/env python
import sys
import os
import logging
import datetime

from mkcommon import *

reload(sys)
sys.setdefaultencoding('utf-8')

cr_owner ='Fware'
cr_dates='2013-2018'

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

AZTestCaseSysHeaderFile="aurora.h"
AZTestCaseVarPrefix="az_pt"
AZTestCaseFuncHasReturn='AZ_TC_FUNC_HAS_RETURN';
AZTestCaseFuncIsVoid='AZ_TC_FUNC_VOID';
AZTestCaseHeaderFile='az_tc.h';
AZTestHeaderFile='az_test.h';
AZTestCaseStruct='az_test_case_t';
AZTestCaseHandlerRetType='az_r_t';
AZTestCaseHandlerType='az_test_case_handler_t';
AZTestCaseFlags="az_tc_flags"
AZTestCaseTraceEnable="AZ_TC_FLAG_TRACE"

AZTestCaseVectorStruct="az_pt_testvector_st"
AZTestCaseVectorArray="az_pt_testvector_array"
AZTestCaseVectorType="az_pt_tv"
AZTestCaseTestVectorSZ="AZ_TC_TV_SZ"
AZXEType="az_xml_element_t"
AZTestIterType="az_test_iter_t"

AZ_SYS_TLS="AZ_SYS_TLS"
AZCurTestCasePointer="az_tc_cur"
AZTestSampleType="int"
AZTestResultType="uint64_t"

def mkDeclareAZTestCaseVars(f, argn, *args):
  print "{0} {1}".format(argn, args)
  i = 0
  for arg in args:
    print "arg:",args[i]
    i=i+1

def mkDeclareAZTestCaseTargetFunction(f, funcdecl):
  f.write("/* this is unit test target function */\n");
  s = "extern {0};\n\n".format(funcdecl)
  f.write(s)


def mkfname(prefix, fn):
  return prefix + "_" + fn

def mkflags(fn):
  return mkfname(mkfname(AZTestCaseVarPrefix, fn), 'flags')

def mkfnHandler(hn, fn):
  return mkfname(AZTestCaseVarPrefix, mkfname(hn, fn))

def mkfnepilog(fn):
  return mkfname(AZTestCaseVarPrefix, mkfname("epilog", fn))

def mkfnCtxProlog(fn):
  return mkfname(AZTestCaseVarPrefix, mkfname(mkfname("prolog", "ctx"), fn))

def mkfnCtxEpilog(fn):
  return mkfname(AZTestCaseVarPrefix, mkfname(mkfname("epilog", "ctx"), fn))

def mkTVStruct(fn):
  return "struct " + mkfname(AZTestCaseVectorStruct, fn)
def mkTVType(fn):
  return mkfname(AZTestCaseVectorType, fn) + "_t"

def mkTCTVSize(pTC, fn):
  typename = mkTVType(fn)
  return "{0}({1}, {2})".format(AZTestCaseTestVectorSZ ,pTC, typename)

def mkfnUTMain(fn):
  return mkfname(AZTestCaseVarPrefix, mkfname("main", fn))

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
  struct_name = mkfname(AZTestCaseVectorStruct, fn)
  f.write("struct {0} ".format(struct_name))
  f.write("{\n")
  argn = len(args)
  i=0
  while i < argn:
    arg = args[i].strip()
    if len(arg) == 0:
      i = i + 1
      continue
    f.write("  {0};\n".format(arg))
    i=i+1
  f.write("  {0} samples;\n".format(AZTestSampleType))
  f.write("  {0} result;\n".format(AZTestResultType))
  f.write("};\n")
  type_name = mkTVType(fn)
  f.write("typedef struct {0} {1};\n".format(struct_name, type_name))

def declCurTestCase(f, fn):
  f.write("/* declare current test case pointer */\n")
  f.write("extern {2} {0} *{1};\n\n".format(AZTestCaseStruct, AZCurTestCasePointer, AZ_SYS_TLS))

def declTestVectorArray(f, fn):
  f.write("/* declare the reference for test vectors */\n")
  struct_name = mkfname(AZTestCaseVectorStruct, fn)
  array_name = mkfname(AZTestCaseVectorArray, fn)
  f.write("extern struct {0} *{1};\n\n".format(struct_name, array_name))

def defTestVectorArray(f, fn):
  f.write("/* define the reference for test vectors */\n")
  struct_name = mkfname(AZTestCaseVectorStruct, fn)
  array_name = mkfname(AZTestCaseVectorArray, fn)
  f.write("struct {0} *{1} = NULL;\n\n".format(struct_name, array_name))

def declTestVectorCur(f, fn):
  f.write("{0}({1});\n\n".format(AZTestCaseVectorCurDecl, fn))

def insertTrace(f, fn, macro):
  f.write("  if (({0} & {2}) || ({1} & {2})) ".format(mkflags(fn), AZTestCaseFlags, AZTestCaseTraceEnable))
  f.write("{\n")
  if len(macro) > 0:
    f.write("    {0};\n".format(macro))
  f.write("  }\n")

def defTCInitHandler(f, fn):
  fname=mkfnHandler("init", fn)
  struct_name = mkfname(AZTestCaseVectorStruct, fn)
  array_name = mkfname(AZTestCaseVectorArray, fn)
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
  f.write("    {0} = (struct {1} *)az_malloc({2});\n".format(array_name, struct_name, mkTCTVSize(argname, fn))) 
  f.write("    if (NULL == {0}) ".format(array_name))
  f.write("{\n")
  f.write("      r = AZ_ERR(MALLOC);\n") 
  f.write("    }\n")
  f.write("    /* here may set the test vector array values from xml configuration file */\n\n");
  f.write("    int j;\n")
  f.write("    {0} *tv = {1};\n".format(mkTVType(fn), array_name))
  xe = "xe"
  f.write("    {0} *{1};\n".format(AZXEType,xe))
  f.write("    {0} *iter = AZ_TC_ITER_LIST({1});\n".format(AZTestIterType, argname))
  f.write("    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) ".format(AZTestIterType))
  f.write("{\n")
  f.write("      AZ_TC_ITER_SET_TV(iter, tv);\n")
  f.write("      xe = az_xml_element_find_attr(iter->xml, \"samples\");\n")
  f.write("      if (xe) tv->samples = strtol(xe->kv.value, NULL, 10);\n")

  f.write("    }\n")
  f.write("  }\n")
  f.write("  AZ_TC_SET_TV({0}, {1});\n".format(argname, array_name)) 
  insertTrace(f, fn, "AZ_TC_PRINT_B(pTC)")
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCSyncHandler(f, fn):
  fname=mkfnHandler("sync", fn)
  f.write(mkfunctionDescr(fname, "called for prepare test case", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  insertTrace(f, fn, "")
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCPrologHandler(f, fn):
  fname=mkfnHandler("prolog", fn)
  f.write(mkfunctionDescr(fname, "called before each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  f.write("  {0} *iter = AZ_TC_CUR_ITER(pTC);\n".format(AZTestIterType))
  f.write("  {0} *tv = AZ_TC_ITER_GET_TV(iter, {1});\n\n".format(mkTVStruct(fn),mkTVType(fn)))
  insertTrace(f, fn, "AZ_TC_PRINT_START(pTC, iter)")
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCRunHandler(f, fn):
  fname=mkfnHandler("run", fn)
  f.write(mkfunctionDescr(fname, "called for each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  f.write("  {0} *iter = AZ_TC_CUR_ITER(pTC);\n".format(AZTestIterType))
  f.write("  {0} *tv = AZ_TC_ITER_GET_TV(iter, {1});\n\n".format(mkTVStruct(fn),mkTVType(fn)))
  insertTrace(f, fn, "")
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCEpilogHandler(f, fn):
  fname=mkfnHandler("epilog", fn)
  f.write(mkfunctionDescr(fname, "called after each test iteration", "test case info", AZTestCaseHandlerRetType))
  f.write("{0} {1}({2} *pTC)".format(AZTestCaseHandlerRetType, fname,
      AZTestCaseStruct))
  f.write("\n{\n")
  f.write("  {0} r = {1};\n".format(AZTestCaseHandlerRetType, "AZ_SUCCESS"))
  f.write("  {0} *iter = AZ_TC_CUR_ITER(pTC);\n".format(AZTestIterType))
  f.write("  {0} *tv = AZ_TC_ITER_GET_TV(iter, {1});\n\n".format(mkTVStruct(fn),mkTVType(fn)))
  insertTrace(f, fn, "AZ_TC_PRINT_END(pTC, iter)")
  f.write("  return r;\n");
  f.write("}\n\n");

def defTCTermHandler(f, fn):
  fname=mkfnHandler("term", fn)
  struct_name = mkfname(AZTestCaseVectorStruct, fn)
  array_name = mkfname(AZTestCaseVectorArray, fn)
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
  f.write("    AZ_TC_SET_TV(pTC, NULL);\n")
  f.write("  }\n\n")
  insertTrace(f, fn, "AZ_TC_PRINT_T(pTC)")
  f.write("  return r;\n");
  f.write("}\n\n");


def defSetReasonFunction(f, fname):
  declCurTestVector = AZTestCaseVectorCurDecl + "(" + fname + ");"
  curTestVector = AZTestCaseVectorCur + "(" + fname + ")"
  func = mkfname(AZTestCaseSetReason,fname)
  f.write(mkfunctionDescr(func, "called from macro AZ_TC_SET_REASON ", "file, line, reason", "void"))
  f.write("void {0}(char *file, int line, char *reason)\n".format(func))
  f.write("{\n");
  f.write("\textern "+declCurTestVector+"\n") 
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_reason", "reason"))
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_file", "file"))
  f.write("\t {0} -> {1} = {2};\n".format(curTestVector, "_line", "line"))
  f.write("}\n\n");

def parseFuncDecl(funcdecl):
  funcdecl = funcdecl.replace("*", "* ")
  token = funcdecl.split(" ")
  #print token
  if len(token) < 2: 
    print "\"{0}\" is invalid func decl".format(funcdecl)
    return False
  rettype = "" 
  func = ""
  found = True 
  #print token
  for arg in token[1:len(token)]:
    if arg == '*' and found:
      rettype = rettype + arg.strip()
    elif len(arg) == 0:
      continue
    else:
      func = func + ' ' + arg
      found = False
  rettype = token[0] + ' ' + rettype
  #print rettype
  #print func
  token = func.strip().split("(",1)
  funcname = token[0]
  if len(token) > 1: 
    token = token[1].split(",")
    #print token
    argcount = len(token)
    token[argcount-1] = token[argcount-1].replace(")", "").strip()
    token[argcount-1] = token[argcount-1].replace(";", "").strip()
  else:
    token = []
  funcstruct={'ret':rettype, 'name':funcname, 'args':token}
  return funcstruct

def argsToVector(rettype, args):
  argn = len(args)
  s = ""
  for arg in args:
    arg = arg.replace("*", "* ")
    token = arg.strip().split(' ')
    if len(token) > 1:
      if len(token) > 2:
        n = token[0]+' '+''.join(token[1:len(token)-1])
        s += "{0}=\"{1}\" ".format(token[len(token)-1].strip(), n.strip()) 
      else:
        s += "{0}=\"{1}\" ".format(token[len(token)-1].strip(), token[0].strip()) 

  s += "{0}=\"{1}\" ".format("samples", AZTestSampleType) 
  s += "{0}=\"{1}\" ".format("result", AZTestResultType) 
  return s

def main():
  if len(sys.argv) < 2:
    print "usage: {0} funcdel".format(sys.argv[0])
    return

  #print "usage: {0} {1} {2}".format(sys.argv[0], sys.argv[1], sys.argv[2])
  username = get_user_name()
  brief = "" 

  funcdecl = sys.argv[1]

  funcstruct = parseFuncDecl(funcdecl)

  if funcstruct == False:
    print "function declaration {0} parse error!".format(funcdecl)
    return
  
  #print funcstruct

  funcname = funcstruct['name'];
  funcret = funcstruct['ret'];
  funcargs = funcstruct['args'];

  if funcret == "void":
    func_has_return = False
  else:
    func_has_return = True

  hfilename = AZTestCaseVarPrefix + '_' + funcname + '.h' 
  cfilename = AZTestCaseVarPrefix + '_' + funcname + '.c' 

  filename = hfilename
  print "create {0}...\n".format(filename)

  filetag=filename.upper().replace('.', '_')
  fileinfo = mkfileinfo(filename, username, brief, cr_owner, cr_dates)

  f = open(filename, "wb")
  f.write(fileinfo)
  f.write("#ifndef {0}\n".format(filetag))
  f.write("#define {0}\n\n".format(filetag))
  f.write("#include \"{0}\"\n".format(AZTestCaseSysHeaderFile))
  f.write("#include \"{0}\"\n".format(AZTestHeaderFile))
  f.write("/* TODO: include function dependant header files */\n\n")
  if func_has_return == False:
    f.write("#define {0}  1\n".format(AZTestCaseFuncIsVoid))
    f.write("#undef {0}  \n".format(AZTestCaseFuncHasReturn))
  else:
    f.write("#define {0}  1\n".format(AZTestCaseFuncHasReturn))
    f.write("#undef {0}  \n".format(AZTestCaseFuncIsVoid))

  f.write("#include \"{0}\"\n\n".format(AZTestCaseHeaderFile))
  f.write("#ifdef __cplusplus\n")
  f.write("extern \"C\"\n{\n#endif\n\n")
  f.write("/* constants */\n\n")
  f.write("/* basic macros */\n\n")
  f.write("/* basic types */\n\n")
  f.write("/* structures */\n\n")
  defTestVectorStruct(f, funcname, funcret, funcargs)
  f.write("/* structured types */\n\n")
  f.write("/* macros */\n\n")
  f.write("/* variabls exposed */\n\n")
  f.write("extern int {0};\n\n".format(mkflags(funcname)))
  declCurTestCase(f, funcname)
  declTestVectorArray(f, funcname)
  f.write("/* inline functions */\n\n")
  f.write("/* function prototypes exposed */\n\n")
  mkDeclareAZTestCaseTargetFunction(f, funcdecl)
  f.write("/* declare global callback functions */\n\n")
  declTCInitHandler(f, funcname)
  declTCSyncHandler(f, funcname)
  declTCPrologHandler(f, funcname)
  declTCRunHandler(f, funcname)
  declTCEpilogHandler(f, funcname)
  declTCTermHandler(f, funcname)
  f.write("#ifdef __cplusplus\n")
  f.write("}\n#endif\n")
  f.write("#endif\n\n")
  f.write("/* === end of {0} === */\n".format(filetag))
  f.close()
  """
  """


  filename = cfilename
  print "create {0}...\n".format(filename)

  filetag=filename.upper().replace('.', '_')
  fileinfo = mkfileinfo(filename, username, brief, cr_owner, cr_dates)

  f = open(filename, "wb")
  f.write(fileinfo)
  f.write("/* include header files */\n")
  f.write("#include \"{0}\"\n".format(hfilename))
  f.write("/* TODO: include function dependant header files */\n\n")
  f.write("/* declare global variables */\n\n")
  f.write("int {0} = {1};\n\n".format(mkflags(funcname), AZTestCaseTraceEnable))
  
  defTestVectorArray(f, funcname)

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
  if filenames[0].find(AZTestCaseVarPrefix) < 0:
    tcname = AZTestCaseVarPrefix + '_' + filenames[0];
  else:
    tcname = filenames[0];
  #filename = AZTestCaseVarPrefix +'_cfg_'+filenames[0] + '.xml'
  filename = filenames[0] + '_cfg.xml'

  print "create {0}...\n".format(filename)
  if os.path.isfile(filename):
    os.rename(filename, filename+'.bk')

  f = open(filename, "wb")

  xmlheader="<? xml version=\"1.0\" encoding=\"iso-8859-1\" ?>"
  cdata="<![CDATA[ testCase version=\"1\" ]]>"
  f.write("{0}\n".format(xmlheader));
  f.write("{0}\n".format(cdata));
  f.write("<testCase name=\"{0}\" prototype=\"{1}\" >\n".format(tcname, funcdecl))
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
