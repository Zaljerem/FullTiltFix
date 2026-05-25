# FullTiltFix

An injectable DLL that fixes the bug Win95 "Full Tilt!" has with saving strings to the registry.



Both the 1.0 and 1.1 versions of the Full Tilt table executables (CADET.EXE, PIRATES.EXE, DRAGON.EXE) have a bug under any Windows newer than Win98 ... they truncate the final character of any string they attempt to write to the registry. This causes a few issues, including being unable to save high scores.



You'll need a tool like Xenos to inject this DLL into the .exe at runtime (at some point I plan to create an .EXE patch).



Pre-compiled DLL and source is included (has a dependency on MinHook - https://github.com/TsudaKageyu/minhook License: https://opensource.org/license/BSD-2-Clause)



I would also like to address the windows positioning issue they have as well, at some point.

