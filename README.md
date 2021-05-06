# EffekseerForLove

This project makes Effekseer (https://effekseer.github.io/en/) available for LÖVE (https://love2d.org/).

## About
The Effekseer OpenGL runtime is used as a base, with a simple wrapper for Lua
access to the Effekseer Manager object. The original is availble here:
https://effekseer.sakura.ne.jp/Releases/EffekseerRuntime160.zip

Note that Effekseer has its own internal OpenGL state that is separate from
LÖVE's. It may be possible to have Effekseer render directly using
love.graphics.\* instead.

## Build
### Linux
	$ tup
	# effekseer.so should be built, which can be used in your LÖVE application.

	# Try out the demo and click the mouse to create an effect:
	$ love .

### love.js
You can build an effekseer.wasm to link into love.js from https://github.com/Davidobot/love.js

	$ tup variant configs/wasm.config
	$ tup
	# build-wasm/effekseer.wasm is available to link into love.js with the included wasm.patch applied to megasource
