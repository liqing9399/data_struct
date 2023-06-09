#######p############################################
#filename      : __init__.py
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2023-05-18 10:58:41
#last modified : 2023-05-18 10:58:41
####################################################
#!/usr/bin/env python

from . import oob1
from . import oob2

def pymoduler():
    print("I'm pymoduler ")

if __name__ == '__main__':
    print ("主程序运行")
else:
    print ("pymodule 初始化")

