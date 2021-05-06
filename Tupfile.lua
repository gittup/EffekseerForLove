if LD then
	tup.rule('<objs>', '^ LD %o^ $(LD) %<objs> -o %o $(LDFLAGS)', 'effekseer.' .. LDEXT)
elseif AR then
	tup.rule('<objs>', '^ AR %o^ $(AR) cr %o %<objs>', 'effekseer.wasm')
end
