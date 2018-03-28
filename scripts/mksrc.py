#!/usr/bin/env python
import sys
import os
import logging
import datetime

from mkcommon import *

#sys.path.append(os.getcwd()+"/utils");

reload(sys)
sys.setdefaultencoding('utf-8')

cr_owner ='Fware'
cr_dates='2013-2018'


def mkfunctionDescrTemplate():
  fileInfo = "/**\n"
  fileInfo += " * @fn        {0}\n".format("function name")
  fileInfo += " * @brief     {0}\n".format("function-description")
  fileInfo += " * @param     {0}\n".format("input-output-parameters")
  fileInfo += " * @return    {0}\n".format("return-value")
  fileInfo += " * @warning   {0}\n".format("warnings")
  fileInfo += " * @exception {0}\n".format("none")
  fileInfo += " */\n\n\n"
  return fileInfo


def main():
  if len(sys.argv) < 2:
    print "usage: {0} filename author brief".format(sys.argv[0])
    return

  filename=sys.argv[1]

  if len(sys.argv) > 2:
    username=sys.argv[2]
  else:
    username = get_user_name()

  if len(sys.argv) > 3:
    brief=sys.argv[3]
  else:
    brief = "" 
  
  filetag=filename.upper().replace('.', '_')
  fileinfo = mkfileinfo(filename, username, brief, cr_owner, cr_dates)

  f = open(sys.argv[1], "wb")
  f.write(fileinfo)
  f.write("/* include header files */\n\n")
  f.write("/* declare global variables */\n\n")
  f.write("/* declare static variables */\n\n\n")
  f.write("/* implement static functions */\n\n")
  functionInfo = mkfunctionDescrTemplate()
  f.write(functionInfo)
  f.write("/* implement global functions */\n\n")
  f.write(functionInfo)
  f.write("/* === end of {0} === */\n".format(filetag))
  f.close()

if __name__ == "__main__":
  main()
