import sys
import os
import logging
import datetime

reload(sys)
sys.setdefaultencoding('utf-8')

if os.name == 'nt':
  import getpass
  def get_user_name():
    return getpass.getuser()
else:
  import pwd
  def get_user_name():
    return pwd.getpwuid(os.getuid()).pw_name

def mkCopyrightInfo(cr_owner, cr_dates):
  cr_info = " * @copyright \n"
  cr_info += " * Copyright (c) {0}, {1} - All Rights Reserved\n".format(cr_owner, cr_dates)
  cr_info += " * You may use, distribute and modify this code under the \n"
  cr_info += " * terms of the \"Aurora Source Code License\".\n"
  cr_info += " * See the file LICENSE for full license details.\\n\\n\n"
  cr_info += " * \n"
  cr_info += " * You should have received a copy of  the LICENSE with this file.\\n\\n\n"
  cr_info += " * \n"
  cr_info += " * If not, please contact to {0} with the information in the file CONTACTS.\n".format(cr_owner)
  return cr_info

def mkfileinfo(filename, author, brief, cr_owner, cr_dates):
  date=datetime.date.today()
  date=datetime.date(date.year, date.month, date.day)
  datestr = date.strftime("%d/%m/%y")
  datestr1 = date.isoformat()
  fileInfo = "/**\n"
  fileInfo += " * @file   {0}\n".format(filename)
  fileInfo += " * @brief  {0}\n".format(brief)
  fileInfo += " * @date   {0}\n".format(datestr)
  fileInfo += " * @author {0}\n".format(author)
  fileInfo += " *\n"
  fileInfo += mkCopyrightInfo(cr_owner, cr_dates)
  fileInfo += " *\n"
  fileInfo += " * History:\n"
  fileInfo += " *   - {0} {1} initial create\n".format(datestr1, author)
  fileInfo += " */\n\n"
  return fileInfo
