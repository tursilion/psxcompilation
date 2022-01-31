cap.exe: cap.c cap.h
	ccpsx -Xo$80100000 -O1 cap.c -ocap.psx -llibsnd.lib -llibspu.lib
	cpe2x cap.psx
	copy cap.exe cd
	cd cd
