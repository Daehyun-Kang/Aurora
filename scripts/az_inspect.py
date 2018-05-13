#!/usr/bin/env python

import asciitree
import sys
import os

sys.path.append(os.getcwd()+"/clang");

from clang.cindex import *
from util import get_cursor
from util import get_cursors
from util import get_tu

include_paths='-I ../aurora/inc -I../aurora/inc/sys'
def get_root(fname):
    tu = get_tu(fname, flags=[include_paths, '-D__CODE_GENERATOR__'], options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    #tu = get_tu(fname, flags=[include_paths, '-D__CODE_GENERATOR__'])
    return tu.cursor

def scan_dir(fname, action=None, arg=None, excludes=[]):
    for root, folders, files in os.walk(fname):
        for filename in files:
            path = os.sep.join([root, filename])
            ext = os.path.splitext(path)[-1].lower()
            if ext != ".c" and ext != '.h': 
                continue
            if path in excludes:
                continue
            if action is not None:
                if arg is not None:
                    result = action(path, arg)
                    if result is not None:
                        return result,path
                else:
                    action(path)
            else:
                print path 
    return None,''


def scan(source, kind):
    print 'scan:{}'.format(source)
    if isinstance(source, str):
        path = source
        source = get_root(source)
    else:
        path = sys.argv[1]
    l = []
    root_cursor = source if isinstance(source, Cursor) else source.cursor
    for cursor in root_cursor.walk_preorder():
        if cursor.location.file == None:
            continue
        if cursor.location.file.name != path: 
            continue
        if cursor.kind == kind: 
            l.append(cursor)
    return l;

def scan_func(source):
    return scan(source, CursorKind.FUNCTION_DECL)

def scan_struct(source):
    return scan(source, CursorKind.STRUCT_DECL)

def scan_typedef(source):
    return scan(source, CursorKind.TYPEDEF_DECL)

def find(source, fname, kind):
    print 'find:{} in {}'.format(fname, source)
    if isinstance(source, str):
        path = source
        source = get_root(source)
    else:
        path = sys.argv[1]
    found = None
    root_cursor = source if isinstance(source, Cursor) else source.cursor
    for cursor in root_cursor.walk_preorder():
        if cursor.location.file == None:
            continue
        if cursor.location.file.name != path: 
            continue
        if cursor.kind != kind: 
            continue
        if cursor.spelling == fname:
            found = cursor
            break
    return found;

def find_func(source, fname):
    return find(source, fname, CursorKind.FUNCTION_DECL)

def find_struct(source, fname):
    return find(source, fname, CursorKind.STRUCT_DECL)

def find_typedef(source, fname):
    return find(source, fname, CursorKind.TYPEDEF_DECL)

def get_decl(node):
    tokens=[]
    if node.kind != CursorKind.FUNCTION_DECL:
        return ''
    for token in node.get_tokens():
        tokens.append(token.spelling)
        if token.spelling == ')': 
            break
    return ' '.join(tokens)


def get_prototype(node):
    args=[]
    if node.kind != CursorKind.FUNCTION_DECL:
        return ''
    args.append(node.type.get_result().spelling)
    args.append(node.spelling)
    skip = True
    for token in node.get_tokens():
        if skip is True:
            if token.spelling == '(':
                skip = False
            else:
                continue
        args.append(token.spelling)
        if token.spelling == ')': 
            break
    return ' '.join(args)

def get_struct(node):
    args=[]
    if node.kind != CursorKind.STRUCT_DECL: 
        return ''
    for token in node.get_tokens():
        args.append(token.spelling)
    return ' '.join(args)

def get_typedef_struct(node):
    args=[]
    child_struct = None
    if node.kind != CursorKind.TYPEDEF_DECL:
        return ''
    for child in node.get_children():
        if child.kind == CursorKind.STRUCT_DECL:
            child_struct = child
            break
    if child_struct is None:
        return 'no struct'
    for token in child_struct.get_tokens():
        if token.spelling == '{':
            args.append(token.spelling+'\n')
            continue
        if token.spelling == ';':
            args.append(token.spelling+'\n')
            continue
        args.append(token.spelling)
    return ' '.join(args)

def find_func_next(fname, folder, curlist):
    return scan_dir(folder, action=find_func, arg=fname, excludes=curlist)


def test_scan_struct():
    node, path = scan_dir(sys.argv[1], action=find_struct, arg=sys.argv[2])
    if node is None:
        print '{} not found'.format(sys.argv[2])

def find_func_next(fname, folder, curlist):
    return scan_dir(folder, action=find_func, arg=fname, excludes=curlist)


def test_scan_struct():
    node, path = scan_dir(sys.argv[1], action=find_struct, arg=sys.argv[2])
    if node is None:
        print '{} not found'.format(sys.argv[2])
    else:
        print 'found: {} in {}'.format(get_struct(node), path)
    exit(0)

def test_find_struct():
    if len(sys.argv) > 2:
        node = find_struct(sys.argv[1], sys.argv[2])
        if node is None:
            print '{} NOT FOUND'.format(sys.argv[2])
        else:
            print 'found:' + get_struct(node)
    else:
        flist = scan_struct(sys.argv[1])
        for node in flist:
            print get_struct(node)
    exit(0)

def test_find_typedef():
    if len(sys.argv) > 2:
        node = find_typedef(sys.argv[1], sys.argv[2])
        if node is None:
            print '{} NOT FOUND'.format(sys.argv[2])
        else:
            print 'found:' + get_typedef_struct(node)
    else:
        flist = scan_typedef(sys.argv[1])
        for node in flist:
            print get_typedef_struct(node)
    exit(0)

def test_scan_func():
    #scan_dir(sys.argv[1], action=scan)
    node, path = scan_dir(sys.argv[1], action=find_func, arg=sys.argv[2])
    if node is None:
        print '{} not found'.format(sys.argv[2])
    else:
        print 'found: {} in {}'.format(get_prototype(node), path)
    exit(0)

def test_find_func():
    if len(sys.argv) > 2:
        node = find_func(sys.argv[1], sys.argv[2])
        if node is None:
            print '{} NOT FOUND'.format(sys.argv[2])
        else:
            print 'found:' + get_prototype(node)
    else:
        flist = scan_func(sys.argv[1])
        for node in flist:
            print get_decl(node)
    exit(0)
    
if __name__ == "__main__":
    #test_scan_func()
    #test_find_func()
    #test_scan_struct()
    #test_find_struct()
    test_find_typedef()
