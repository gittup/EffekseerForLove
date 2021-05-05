CC = 'g++'
LD = 'g++'
LDFLAGS += '`pkg-config luajit --libs`'
LDFLAGS += '-shared'
