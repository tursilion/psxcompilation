CLS
OPEN "cosin.h" FOR OUTPUT AS #1
pi = 3.1415
FOR a = 0 TO 2 * pi STEP (2 * pi) / 319
PRINT #1, INT(COS(a) * 80) + 120; ",";
NEXT a
FOR a = 0 TO 2 * pi STEP (2 * pi) / 319
PRINT #1, INT(COS(a) * 80) + 120; ",";
NEXT a
PRINT #1,
CLOSE #1



