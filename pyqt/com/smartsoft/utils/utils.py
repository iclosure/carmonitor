'''
Created on Apr 25, 2015

@author: iclosure
'''
import os, sys

# 获取脚本文件的当前路径
def applicationDirPath():
    # 获取脚本路径
    path = sys.path[0]
    # 判断为脚本文件还是py2exe编译后的文件，如果是脚本文件，则返回的是脚本的目录，如果是py2exe编译后的文件，则返回的是编译后的文件路径
    if os.path.isdir(path):
        return path
    elif os.path.isfile(path):
        return os.path.dirname(path)

# 生成平台苦文件后缀
def libSuffix():
    return ('.dylib' if sys.platform == 'darwin'
            else '.so' if sys.platform == 'linux'
            else '.dll' if sys.platform == 'win32'
            else '.x')

# TEST
if __name__ == '__main__':
    print(libSuffix())
