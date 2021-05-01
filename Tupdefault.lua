outputs = {'%B.o'}
outputs.extra_outputs = EFFEKSEER_PLUGIN .. '/<objs>'
tup.foreach_rule('*.cpp', '^ C++ %f^ g++ $(CFLAGS) -c %f -o %o', outputs)
