tup.creategitignore()
EFFEKSEER_PLUGIN = tup.getcwd()

tup.include(tup.getconfig('TUP_PLATFORM') .. '.lua')

CFLAGS += '`pkg-config luajit --cflags`'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/Effekseer/'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/EffekseerRendererGL/'
CFLAGS += '-fPIC'

if tup.getconfig('DEBUG') == 'y' then
	CFLAGS += '-g'
	CFLAGS += '-Og'
else
	CFLAGS += '-O2'
end
