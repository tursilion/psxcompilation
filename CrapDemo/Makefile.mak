play.exe: play.c
	ccpsx -Xo$80010000 play.c hitmod.obj -oplay.psx -llibspu.lib -llibtap.lib
	cpe2x play.psx
