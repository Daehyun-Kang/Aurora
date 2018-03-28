#!/usr/bin/env python
import sys
import os
import logging
import datetime

reload(sys)
sys.setdefaultencoding('utf-8')

from mkcommon import *

#sys.path.append(os.getcwd()+"/utils");

cr_owner ='Fware'
cr_dates='2013-2017'

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
  f.write("#ifndef {0}\n".format(filetag))
  f.write("#define {0}\n\n".format(filetag))
  f.write("#ifdef __cplusplus\n")
  f.write("extern \"C\"\n{\n#endif\n\n")
  f.write("/* constants */\n\n")
  f.write("/* basic macros */\n\n")
  f.write("/* basic types */\n\n")
  f.write("/* structures */\n\n")
  f.write("/* structured types */\n\n")
  f.write("/* macros */\n\n")
  f.write("/* variabls exposed */\n\n")
  f.write("/* inline functions */\n\n")
  f.write("/* function prototypes exposed */\n\n")
  f.write("#ifdef __cplusplus\n")
  f.write("}\n#endif\n")
  f.write("#endif\n\n")
  f.write("/* === end of {0} === */\n".format(filetag))
  f.close()

if __name__ == "__main__":
  main()
