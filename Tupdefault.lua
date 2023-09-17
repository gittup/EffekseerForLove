outputs = {'%B.o'}
outputs.extra_outputs = {'^/cache.lock', EFFEKSEER_PLUGIN .. '/<objs>'}
tup.foreach_rule('*.cpp', '^ C++ %f^ $(CC) $(CFLAGS) -c %f -o %o', outputs)
