# Wrzeczak's Vector Language

See `THEORY.md` for practical specifications; writing languages has been something I've been trying my hand at since the 6th grade; was recently reminded of this and decided to try again.

WVLang is an interpreted language; run it using `./wvcomp <file.wvlang>`.

To compile `wvcomp`, run `./nob build`. To compile `nob`, run `gcc nob.c -o nob`, or an equivalent command in your C compiler. [The documentation for nob can be found here](https://github.com/tsoding/nob.h). Essentially, `nob.h` is the meat of the library, and `nob.c` is the build script. Modifying `nob.c` will cause it to automatically rebuild on running `./nob` again, so you can tinker with the way it's built for yourself, and no tools other than the C compiler are required.