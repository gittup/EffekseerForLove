cross_root = '/home/marf/cross-win'
luajit_path = cross_root .. '/luajit'
CC = 'x86_64-w64-mingw32-g++-posix'
LD = 'x86_64-w64-mingw32-g++-posix'
LDFLAGS += '-L' .. luajit_path
LDFLAGS += '-shared'
LDFLAGS += '-lws2_32'
LDFLAGS += '-lglu32'
LDFLAGS += '-lopengl32'
LDFLAGS += '-lluajit-5.1'
LDEXT = 'dll'
