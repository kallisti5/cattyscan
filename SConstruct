env = Environment()

def CheckPKGConfig(context, version):
	context.Message( 'Checking for pkg-config... ' )
	ret = context.TryAction('pkg-config --atleast-pkgconfig-version=%s' % version)[0]
	context.Result( ret )
	return ret
def CheckPKG(context, name):
	context.Message( 'Checking for %s... ' % name )
	ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
	context.Result( ret )
	return ret

conf = Configure(env, custom_tests = { 'CheckPKGConfig' : CheckPKGConfig,
	'CheckPKG' : CheckPKG })

if not conf.CheckPKGConfig('0.15.0'):
	print('pkg-config >= 0.15.0 not found.')
	Exit(1)
if not conf.CheckPKG('libcurl'):
	print('libcurl not found.')
	Exit(1)
if not conf.CheckPKG('Qt5Gui'):
	print('Qt not found.')
	Exit(1)
if not conf.CheckPKG('Qt5Widgets'):
	print('Qt not found.')
	Exit(1)

env = conf.Finish()

env.Append(CPPPATH='#src/libcatty/includes/')
env.Append(CPPFLAGS='-g -fPIC')

Export("env")
SConscript('src/SConscript')
