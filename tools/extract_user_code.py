#!/usr/bin/env python3
import os
import numpy as np
import sys

def explore(rootdir, ext):
    for subdir, dirs, files in os.walk(rootdir):
        for file in files:
            filepath = subdir + os.sep + file
            if file.endswith((ext)):
                print(' ')
                print("#"*80)
                print('#')
                print('# ', filepath)
                print('#')
                print("#"*80)
                extract_user_code(filepath)

def extract_user_code(filename):
    try:
        with open(filename) as reader:
            lines = reader.readlines()
            user_code = False
            for nl, line in enumerate(lines):
                if "USER CODE BEGIN" in line:
                    user_code = True
                if "USER CODE END" in line:
                    print('/* ',nl+1, '*/ ',line, end='')
                    print("="*80)
                    user_code = False       
                if user_code and len(line)>1:
                    print('/* ', nl+1, '*/ ',line, end='')
    except:
        return

if __name__ == '__main__':
#    print("Argument List:", str(sys.argv))

    explore('.', '.h')
    explore('.', '.c')






