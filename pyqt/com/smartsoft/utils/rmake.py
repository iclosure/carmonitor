'''
Created on Apr 6, 2015

@author: iclosure
'''

import os

class Config:
    import PyQt5
    pypath = PyQt5.__path__[0].split('/lib/')[0]
    pylupdate = pypath + '/bin/pylupdate5'
    pydoc = pypath + '/bin/pydoc3.4'
    pyuic = pypath + '/bin/pyuic5'
    pyrcc = pypath + '/bin/pyrcc5'
    ''' -------'''
    lupdate = 'lupdate'
    linguist = 'linguist'
    lrelease = 'lrelease'
    qmake = 'qmake'
    lconvert = 'lconvert'
    moc = 'moc'
    qdoc = 'qdoc'
    rcc = 'rcc'
    uic = 'uic'

def buildqrc(path = '.'):
    '''
    generate Qt source files from resources
    '''
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith('.ui'):
                os.system('%s -o ui_%s.py %s' % (Config.pyuic, file.rsplit('.', 1)[0], file))
            elif file.endswith('qrc'):
                os.system('%s -o %s_rc.py %s' % (Config.pyrcc, file.rsplit('.', 1)[0], file))


def buildlang(path, target):
    '''
    update translations and generate ts files
    @param path: the root of project
    @param target: the name of translator file
    '''
    def _buildlang(path, target):
        for root, dirs, files in os.walk(path):
            for file in files:
                if file.endswith('.py'):
                    os.system('%s %s -ts %s.ts' % (Config.pylupdate, root + '/' + file, file + '_' + target))
            for _dir in dirs:
                _buildlang(_dir, target)
    print('* starting building language %s...' % target)
    # update
    _buildlang(path, target)
    # linguist
    # os.system('if exist %s %s %s')
    # release
    print('* ending building language %s' % target)

if __name__ == '__main__':
    import PyQt5
    print('PyQt5.__path__[0]:', PyQt5.__path__[0])
    print('Config.pypath:', Config.pypath)
