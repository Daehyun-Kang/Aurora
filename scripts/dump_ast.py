#!/usr/bin/env python

import asciitree
import sys
import os

sys.path.append(os.getcwd()+"/clang");

from clang.cindex import *
from util import get_cursor
from util import get_cursors
from util import get_tu

def node_children(node):
    l = []
    #if hasattr(node.location.file, 'name') and node.location.file.name == sys.argv[1]:
    #    return list(node.get_children())
    for c in node.get_children():
        if c.location.file == None:
            continue
        if c.location.file.name == sys.argv[1]:
            l.append(c)
    return l 

def srcrangestr(x):
    return '%d:%d - %d:%d' % (x.start.line, x.start.column, x.end.line, x.end.column)

def print_node(node):
    #if node.location.file == None:
    #    return ''
    #if node.location.file.name != sys.argv[1]:
    #    return ''
    #file = node._tu.get_file(sys.argv[1])
    text = node.spelling or node.displayname
    if len(text) == 0 and hasattr(node, 'value'):
        text = node.value
    if len(text) == 0 and hasattr(node, 'name'):
        text = node.name
    #if len(text) == 0:
    #    for token in node.get_tokens():
    #        text += token.spelling + ' '
    if len(text) == 0:
        #j = len(list(node.get_children()))
        #children = node.get_children()
        #for ch in children: 
        #    text += print_node(ch) 
        #loc = node.location 
        #ext = node.extent
        #text = 'child:'+str(j)
        text = srcrangestr(node.extent)
    kind = str(node.kind)[str(node.kind).index('.')+1:]
    #print '{} {}'.format(kind, text)
    return '{} {}'.format(kind, text)

if len(sys.argv) != 2:
    print("Usage: dump_ast.py [header file name]")
    sys.exit()

#Config.set_library_file('/usr/lib/llvm-6.0/lib/libclang.so.1')
#index = Index.create()
#translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11', '-D__CODE_GENERATOR__'])
include_paths='-I ../aurora/inc -I../aurora/inc/sys'
#translation_unit = index.parse(sys.argv[1], [include_paths, '-D__CODE_GENERATOR__'])
#translation_unit = get_tu(sys.argv[1], flags=[include_paths, '-D__CODE_GENERATOR__'])
def get_root(fname):
    tu = get_tu(fname, flags=[include_paths, '-D__CODE_GENERATOR__'], options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    #tu = get_tu(fname, flags=[include_paths, '-D__CODE_GENERATOR__'])
    return tu.cursor
#print translation_unit.cursor.get_children()
#print(asciitree.draw_tree(translation_unit.cursor, node_children, print_node))
#traverse = asciitree.traversal.Traversal(get_children=node_children, get_text=print_node)
traverse = asciitree.traversal.Traversal(get_root=get_root, get_children=node_children, get_text=print_node)
#print(asciitree.LeftAligned(traverse=traverse)(translation_unit.cursor))
print(asciitree.LeftAligned(traverse=traverse)(sys.argv[1]))


