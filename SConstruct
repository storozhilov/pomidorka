import os

env = Environment(
CCFLAGS = '-g -Wall',
CPPPATH = './include',
LIBS = ['islScada', 'isl', 'pthread', 'rt']
)
env.ParseConfig('pkg-config --cflags --libs libmodbus')
env.Append(ENV = {'PATH' : os.environ['PATH']})

prefix = '/usr/local'
if 'PREFIX' in os.environ:
	prefix = os.environ['PREFIX']

programBuilder = env.Program('bin/ghd', Glob('src/*.cxx'))
Default(programBuilder)

env.Command('uninstall', None, Delete(FindInstalledFiles()))
