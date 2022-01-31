demo.exe: demo.c story.txt font.inc tursi.inc wpw.inc filters.inc freak.inc
        ccpsx -Xo$80100000 -O1 demo.c hitmod.obj -odemo.psx -llibsnd.lib -llibspu.lib -llibmath.lib
        cpe2x demo.psx
