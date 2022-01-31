sand.exe: sand.c sand.h sand.txt
        ccpsx -Xo$80100000 -O3 sand.c hitmod.obj -osand.psx -llibsnd.lib -llibspu.lib
        cpe2x sand.psx
