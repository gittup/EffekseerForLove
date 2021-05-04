tup.creategitignore()
EFFEKSEER_PLUGIN = tup.getcwd()

CFLAGS += '`pkg-config luajit --cflags`'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/include/'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/Effekseer/'
CFLAGS += '-fPIC'
CFLAGS += '-g'
CFLAGS += '-O2'

LDFLAGS += '`pkg-config luajit --libs`'
LDFLAGS += '-fPIC'
LDFLAGS += '-g'
