#!/usr/bin/env python
import sys
import os
import logging
import datetime

from mkcommon import *

sys.path.append(os.getcwd()+"/clang");

from clang.cindex import *
from util import get_cursor
from util import get_cursors
from util import get_tu

depth = 0
def traverse(node):
    global depth
    #i = 0
    #while i < depth:
    #    print ' '
    #    i = i + 1
    #print '{0} {1}'.format(depth, node.spelling) 
    if node is None:
        return
    if node.kind == CursorKind.FUNCTION_DECL:
        if node.spelling != sys.argv[2]:
            return
        #text = node.spelling or node.displayname
        ftype = node.type.spelling.split(' ')[0]
        #ftype = node.type.get_canonical()
        text = node.displayname
        kind = str(node.kind)[str(node.kind).index('.')+1:]
        print '{} {} {}'.format(depth, ftype, text)
        for c in node.get_children():
            print c.spelling
        return
    for c in node.get_children():
        depth = depth + 1
        traverse(c)
    depth = depth - 1

def find_typerefs(node, typename):
    if node.kind == CursorKind.CALL_EXPR:
        if node.spelling == typename:
            print 'Found %s [line=%s, col=%s]' % (typename, node.location.line, node.location.column)
        else:
            print 'not Found %s [line=%s, col=%s]' % (typename, node.location.line, node.location.column)
    else:
        print '{0}'.format(node.spelling)

    for c in node.get_children():
        find_typerefs(c, typename)

index = Index.create()
tu = index.parse(sys.argv[1])
include_paths='-I../aurora/inc'
#tu = index.parse(sys.argv[1], [include_paths, '-D__CODE_GENERATOR__'])
tu = get_tu(sys.argv[1], flags=[include_paths, '-D__CODE_GENERATOR__'])
print 'Translation unit:', tu.spelling
#find_typerefs(tu.cursor, sys.argv[2])
traverse(tu.cursor)

#if __name__ == "__main__":
#  main()
