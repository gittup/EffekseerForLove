tup.creategitignore()
EFFEKSEER_PLUGIN = tup.getcwd()

CFLAGS += '`pkg-config luajit --cflags`'
CFLAGS += '-I' .. EFFEKSEER_PLUGIN .. '/src/effekseer/src/Effekseer/'
CFLAGS += '-fPIC'

LDFLAGS += '`pkg-config luajit --libs`'
LDFLAGS += '-fPIC'
