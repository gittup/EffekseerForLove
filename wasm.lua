CC = 'em++'
AR = 'emar'
CFLAGS += '-s WASM=1'
CFLAGS += '-include ' .. EFFEKSEER_PLUGIN .. '/src/glad/gladwrap.h'
