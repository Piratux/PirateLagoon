# What is this?
This is set of files for Visual Studio (though can be used with other IDEs) that let's you just jump straight into coding for Emscripten.

# Setup:
Instructions in the video:
https://www.youtube.com/watch?v=MrI5kkVY6zk

Manual steps:
- Go to https://emscripten.org/
- Download the emscripten
- Follow the steps install it (such as running bat or sh scripts)

# Compile:
pge2wasm.bat build
or
Emscripten Compile in `Tools`

# Run
pge2wasm.bat run
or
Emscripten Run in `Tools`

# Notes:
WASM folder already contains `index.html` which is modifies emscripten's html template to make it a fullscreen app.
`index.html` taken from here:
https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/tools/wasm/basic_template.html
