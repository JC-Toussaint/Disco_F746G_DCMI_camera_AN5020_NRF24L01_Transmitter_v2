#!/usr/bin/env python3
import os
import numpy as np
import sys

def explore(rootdir, string):
    for subdir, dirs, files in os.walk(rootdir):
        for file in files:
            filepath = subdir + os.sep + file
            if file.endswith('.h') or file.endswith('.c'):
                extract_user_code(filepath, string)

def extract_user_code(filename, string):
    try:
        with open(filename) as reader:
            lines = reader.readlines()
            first = True
            for nl, line in enumerate(lines):
                if string in line:
                    if first:
                        print(' ')
                        print("#"*80)
                        print('#')
                        print('# ', filename)
                        print('#')
                        print("#"*80)
                        first = False
                    print('/* ', nl+1, '*/ ',line, end='')
    except:
        return

if __name__ == '__main__':
#    print("Argument List:", str(sys.argv))
    string =sys.argv[1]
    explore('.', string)






