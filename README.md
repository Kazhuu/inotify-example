# Linux inotify example

Simple example program to test Linux [inotify](http://man7.org/linux/man-pages/man7/inotify.7.html) api. Watches provided `sample` file for metadata and modify changes.

## Usage

Build using CMake
```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```
After build run with
```
./inotify
```
While running try to touch `sample` file or modify it and save. You should see program print events to the terminal.

## License

MIT license
