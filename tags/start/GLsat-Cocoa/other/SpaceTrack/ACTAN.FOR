\begin{verbatim}
      FUNCTION  ACTAN(SINX,COSX)
      COMMON/C2/DE2RA,PI,PIO2,TWOPI,X3PIO2
      ACTAN=0.
      IF (COSX.EQ.0.  ) GO TO 5
      IF (COSX.GT.0.  ) GO TO 1
      ACTAN=PI
      GO TO 7
    1 IF (SINX.EQ.0.  ) GO TO 8
      IF (SINX.GT.0.  ) GO TO 7
      ACTAN=TWOPI
      GO TO 7
    5 IF (SINX.EQ.0.  ) GO TO 8
       IF (SINX.GT.0.  ) GO TO 6
      ACTAN=X3PIO2
      GO TO 8
    6 ACTAN=PIO2
      GO TO 8
    7 TEMP=SINX/COSX
      ACTAN=ACTAN+ATAN(TEMP)
    8 RETURN
      END
\end{verbatim}
