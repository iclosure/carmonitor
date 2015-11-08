'''
Created on Apr 6, 2015

@author: iclosure
'''

import os

if __name__ == '__main__':
    for root, dirs, files in os.walk('.'):
        for file in files:
            if file.endswith('.ui'):
                os.system('pyuic5 -o ui_%s.py %s' % (file.rsplit('.', 1)[0], file))
            elif file.endswith('qrc'):
                os.system('pyrcc5 -o %s_rc.py %s' % (file.rsplit('.', 1)[0], file))
                