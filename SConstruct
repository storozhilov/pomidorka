# iGreenHouse project SCons configuration file

import os

AddOption('--build-firmware',
		dest = 'build-firmware',
		action = 'store_true',
		help = 'Build firmware (the same as if \'GH_BUILD_FIRMWARE\' environment variable is set to \'yes\')')

#sconscriptTargets = ['src/SConscript']
sconscriptTargets = ['src/SConscript', 'mbclnt/SConscript', 'tests/SConscript']
if GetOption('build-firmware') or os.environ.get('GH_BUILD_FIRMWARE', '').upper() == 'YES':
	sconscriptTargets.append('firmware/SConscript')
SConscript(sconscriptTargets)

# Uninstall section
env = Environment()
uninstaller = env.Command('uninstall', None, Delete(env.FindInstalledFiles()))
env.Alias('uninstall', uninstaller)
