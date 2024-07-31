Step-by-step guide on how to install and configure OpenGL in Code: Blocks for Windows operating system.
•	Download codeblocks from http://www.codeblocks.org/downloads/binaries/#imagesoswindows48p nglogo- microsoft- windows
•	Choose codeblocks=20.03mingw-setup.exe from FossHUB for64-bit windows machine
•	Download Freeglut and unzip https://www.transmissionzero.co.uk/software/freeglu t-devel/.
•	Changes to glut.cpb
•	Open notepad as Administrator.
•	Navigate to C:\Program Files\CodeBlocks\share\CodeBlocks\templates
•	Show all files
•	Open glut.cpb
•	Edit->Replace all “glut32” with “freeglut”
•	Changes to wizard.Script
•	Open notepad as Administrator.
•	Navigate to C:\ProgramFiles\CodeBlocks\share\CodeBlocks\templates\wiza rd\glut
•	Show all files
•	Open wizard.Script
•	Edit->Replace all “glut32” with “freeglut”
•	Include,lib,bin
•	Copy GL folder in freeglut-MinGW-3.0.0-1.mp\freeglut\include
•	Paste it here: C:\Program Files\CodeBlocks\MinGW\include
•	Copy the two lib files in freeglut-MinGW-3.0.0-1.mp\freeglut\lib\x64
•	Paste here : C:\Program Files\CodeBlocks\MinGW\lib
•	Copy freeglut.dll file freeglut-MinGW-3.0.0-1.mp\freeglut\bin\x64
•	Paste it here :C:\Windows\SysWOW64
•	Also paste it here : C:\Windows\System32 “freeglut”
