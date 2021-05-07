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
### Linux/MacOSX
	$ tup
	# effekseer.so should be built, which can be used in your LÖVE application.

	# Try out the demo and click the mouse to create an effect:
	$ love .

### love.js
You can build an effekseer.wasm to link into love.js from https://github.com/Davidobot/love.js

	$ tup variant configs/wasm.config
	$ tup
	# build-wasm/effekseer.wasm is available to link into love.js with the included wasm.patch applied to megasource

### Linux->MacOSX cross compile
If you are feeling adventurous, you can cross-compile from Linux to MacOSX.
First you'll need to setup a cross-build environment with:

 - A MacOSX10.15.sdk (other versions may work as well)
 - A copy of /Library/Developer/CommandLineTools/usr/include from a Mac
 - ld64 from cctools (https://github.com/darlinghq/cctools-port) built with libtapi (https://github.com/tpoechtrager/apple-libtapi)

Then you'll need to update cross-macosx.lua for your setup before building.

	$ vi cross-macosx.lua
	# Edit cross-macosx.lua with your cross-build environment paths
	$ tup variant configs/cross-macosx.config
	$ tup
	# build-cross-macosx/effekseer.so is available to be copied to a Mac

### Linux->Windows cross compile
To cross compile from Linux to Windows, you'll need:

 - A cross compiler (cross-win.lua is setup with `x86_64-w64-mingw32-g++-posix` from the `g++-mingw-w64-x86-64` package on Ubuntu)
 - The SDL2 development library (https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz)
 - luajit (https://luajit.org/git/luajit.git) built for Windows (eg: `make HOST_CC="gcc" CROSS=x86_64-w64-mingw32- TARGET_SYS=Windows`).

You'll need to edit cross-win.lua for your setup before building

	$ vi cross-win.lua
	# Edit cross-win.lua with your cross-build environment paths
	$ tup variant configs/cross-win.config
	$ tup
	# build-cross-win/effekseer.dll is available to be copied to a Windows machine
