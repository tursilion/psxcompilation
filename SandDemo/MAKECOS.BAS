OPEN "cosin.h" FOR OUTPUT AS #1
FOR i = 0 TO 6.283 STEP (6.283 / 640)
  PRINT #1, INT((COS(i) * 50) + 120); ", ";
NEXT i



