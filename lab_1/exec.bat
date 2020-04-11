cd /D "C:\Program Files (x86)\GUI Turbo Assembler\BIN"
::gui64.exe -c "mount C: 'C:\Program Files (x86)\GUI Turbo Assembler\BIN"' -c C: -c tasm lab.asm
gui64.exe -c "mount C: 'C:\Program Files (x86)\GUI Turbo Assembler\BIN"' -c C: -c "mount D: 'D:\'" -c D: -c "cd ASM\lab_1" -c lab.exe
exit