# FullTiltFix

A launcher/DLL that fixes some of the bugs Win95 "Full Tilt! Pinball" has in modern Windows:



* Issue writing strings to the registry



Both the 1.0 and 1.1 versions of the Full Tilt! Pinball table executables (CADET.EXE, PIRATES.EXE, DRAGON.EXE) have a bug under any Windows newer than Win98 ... they truncate the final character of any string they attempt to write to the registry. This causes a few issues, including being unable to save high scores.



* Initial window spawns out of position



The table executables launch their window out of position - this fix centers the window nearly immediately. (There is still some fullscreen weirdness, but it is playable)



Notes and usage:



Put the launcher .exe and fix .dll into the table folder; on launch it will automatically detect CADET.EXE, PIRATES.EXE, or DRAGON.EXE, launch it, and inject the DLL ... 



Pre-compiled DLL, EXE and source is included (has a dependency on MinHook, included - https://github.com/TsudaKageyu/minhook License: https://opensource.org/license/BSD-2-Clause)

