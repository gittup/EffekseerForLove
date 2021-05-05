tup.creategitignore()
EFFEKSEER_PLUGIN = tup.getcwd()

tup.include(tup.getconfig('TUP_PLATFORM') .. '.lua')

CFLAGS += '`pkg-config luajit --cflags`'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/include/'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/Effekseer/'
CFLAGS += '-fPIC'
CFLAGS += '-g'
CFLAGS += '-O2'

LDFLAGS += '-fPIC'
LDFLAGS += '-g'
