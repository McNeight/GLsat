*      DPPER DSPACE PERIODICS  (STANDALONE SGP4 DC/DPPER4)  20APR90

      SUBROUTINE DPPER(XNP,EP,XIP,XNODEP,OMEGAP,XMAP,INITDS)

*-----------------------------------------------------------------------
*     THIS SUBROUTINE PROVIDES DEEP SPACE LONG PERIOD PERIODIC CONTRIBU-
*     TIONS TO THE MEAN ELEMENTS. BY DESIGN THESE PERIODICS ARE ZERO
*     AT EPOCH.
*
*     Note by Rob Matson:  The last sentence is false.  I believe
*     they SHOULD be zero at epoch, but the way the program is coded
*     by Goddard, they are not.  See below.                       
*-----------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      save  !required for IRIX compilers

      INTEGER YR
      INTEGER*4 SATN

      COMMON/CONST/C1,C1P2P,PI,PIO2,QZMS2T,RPTIM,SS,THGR70,TWOPI,X2O3,
     1       X3PIO2,XJ2,XJ3,XJ4,XKE,XKMPER,XJ3OJ2
      COMMON/ELTM/AM,EM,XINCM,XNODEM,OMEGAM,XMAM,CNODM,CON49,COSIM,
     1       COSOMM,EMSQ,RTEMSQ,SINIM,SINOMM,SNODM,XN
      COMMON/EPELM/AO,EO,XINCO,XNODEO,OMEGAO,XMAO,AINV,BSTAR,CNDOT,
     1       CON41,CON42,COSIO,COSIO2,EINV,EOSQ,EPOCH,OMEOSQ,
     2       OMGDT,POSQ,QO,RTEOSQ,SINIO,T,THGRO,XMDOT,XNDOT,
     3       XNO,XNODOT,XPIDOT,SATN,YR,init,method
************************************************************************

      COMMON/LSCOM/S1,S2,S3,S4,S5,S6,S7,Z1,Z2,Z3,Z11,Z12,Z13,Z21,Z22,
     1             Z23,Z31,Z32,Z33,SS1,SS2,SS3,SS4,SS5,SS6,SS7,SZ1,SZ2,
     2             SZ3,SZ11,SZ12,SZ13,SZ21,SZ22,SZ23,SZ31,SZ32,SZ33,
     3             DAY,GAM

      DATA              ZNS,             ZES/
     1                  1.19459D-5,      .01675D0/
      DATA              ZNL,            ZEL/
     1                  1.5835218D-4,    .05490D0/

      IF(INITDS .GT. 0) GO TO 40

*     DEEP SPACE PERIODICS INITIALIZATION

      XN=XNP
      EM=EP
      SNODM=SIN(XNODEP)
      CNODM=COS(XNODEP)
      SINOMM=SIN(OMEGAP)
      COSOMM=COS(OMEGAP)
      SINIM=SIN(XIP)
      COSIM=COS(XIP)

*     INITIALIZE LUNAR SOLAR TERMS

      CALL DSCOM
      ZMOL=FMOD2P(4.7199672D0+.22997150D0*DAY-GAM)
      ZMOS=FMOD2P(6.2565837D0+.017201977D0*DAY)

*     DO SOLAR TERMS

      SE2=2.D0*SS1*SS6
      SE3=2.D0*SS1*SS7
      SI2=2.D0*SS2*SZ12
      SI3=2.D0*SS2*(SZ13-SZ11)
      SL2=-2.D0*SS3*SZ2
      SL3=-2.D0*SS3*(SZ3-SZ1)
      SL4=-2.D0*SS3*(-21.D0-9.D0*EMSQ)*ZES
      SGH2=2.D0*SS4*SZ32
      SGH3=2.D0*SS4*(SZ33-SZ31)
      SGH4=-18.D0*SS4*ZES
      SH2=-2.D0*SS2*SZ22
      SH3=-2.D0*SS2*(SZ23-SZ21)

*     DO LUNAR TERMS

      EE2=2.D0*S1*S6
      E3=2.D0*S1*S7
      XI2=2.D0*S2*Z12
      XI3=2.D0*S2*(Z13-Z11)
      XL2=-2.D0*S3*Z2
      XL3=-2.D0*S3*(Z3-Z1)
      XL4=-2.D0*S3*(-21.D0-9.D0*EMSQ)*ZEL
      XGH2=2.D0*S4*Z32
      XGH3=2.D0*S4*(Z33-Z31)
      XGH4=-18.D0*S4*ZEL
      XH2=-2.D0*S2*Z22
      XH3=-2.D0*S2*(Z23-Z21)

*     CALCULATE TIME VARYING PERIODICS

   40 ZM=ZMOS+ZNS*T
      IF(INIT .NE. 0) ZM=ZMOS      ! For calculating offset at epoch
      ZF=ZM+2.D0*ZES*SIN (ZM)
      SINZF=SIN (ZF)
      F2=.5D0*SINZF*SINZF-.25D0
      F3=-.5D0*SINZF*COS (ZF)
      SES=SE2*F2+SE3*F3
      SIS=SI2*F2+SI3*F3
      SLS=SL2*F2+SL3*F3+SL4*SINZF
      SGHS=SGH2*F2+SGH3*F3+SGH4*SINZF
      SHS=SH2*F2+SH3*F3
      ZM=ZMOL+ZNL*T
      IF(INIT .NE. 0) ZM=ZMOL      ! For calculating offset at epoch
      ZF=ZM+2.D0*ZEL*SIN (ZM)
      SINZF=SIN (ZF)
      F2=.5D0*SINZF*SINZF-.25D0
      F3=-.5D0*SINZF*COS (ZF)
      SEL=EE2*F2+E3*F3
      SIL=XI2*F2+XI3*F3
      SLL=XL2*F2+XL3*F3+XL4*SINZF
      SGHL=XGH2*F2+XGH3*F3+XGH4*SINZF
      SHL=XH2*F2+XH3*F3
      PE=SES+SEL
      PINC=SIS+SIL
      PL=SLS+SLL
      PGH=SGHS+SGHL
      PH=SHS+SHL
      IF(INIT .EQ. 0) GO TO 200
CRM   Note:  in the following 5 lines, Goddard is essentially
CRM   setting the solar/lunar offsets to zero at epoch.  This
CRM   will cause a discrepancy between SGP4 and SDP4 at epoch,
CRM   by an amount equal to whatever the solar/lunar perturbation
CRM   happens to be at epoch.  I can't imagine why they would go
CRM   to all the trouble to calculate what the offsets are at
CRM   epoch, and then ignore them!  You can trace the program
CRM   logic for yourself.  In the five lines starting at line
CRM   200, the epoch offsets are subtracted.  Since Goddard has
CRM   set them all to zero, the variables PEO, PINCO, PLO, PGHO
CRM   and PHO do nothing.  So the five lines at 200 do nothing.
CRM   You can test it yourself.  If you completely take out the
CRM   five lines starting at line 200, there will be no change
CRM   in the results.
CRM 
CRM   It is for this reason that I believe the next five lines
CRM   should read, instead,
CRM     PEO = PE
CRM     PINCO = PINC
CRM     PLO = PL
CRM     PGHO = PGH
CRM     PHO = PH
CRM   You'll also need to get rid of the RETURN statement, and
CRM   replace it with a conditional RETURN as indicated below.
      PEO=0.D0
      PINCO=0.D0
      PLO=0.D0
      PGHO=0.D0
      PHO=0.D0
      RETURN
  200 PE=PE-PEO
      PINC=PINC-PINCO
      PL=PL-PLO
      PGH=PGH-PGHO
      PH=PH-PHO
CRM   IF (INIT .EQ. 0) RETURN
      XIP = XIP+PINC
      EP = EP+PE
      SINIP=SIN(XIP)
      COSIP=COS(XIP)
      IF(XIP.LT.(.2D0)) GO TO 220

*     APPLY PERIODICS DIRECTLY

      PH=PH/SINIP

CRM   Note:  the following two lines can be combined into one:
CRM       OMEGAP = OMEGAP + PGH - COSIP*PH
CRM   since PGH is not used anywhere else.
      PGH=PGH-COSIP*PH
      OMEGAP=OMEGAP+PGH
      XNODEP=XNODEP+PH
      XMAP=XMAP+PL
      RETURN

*     APPLY PERIODICS WITH LYDDANE MODIFICATION

  220 SINOP=SIN(XNODEP)
      COSOP=COS(XNODEP)
      ALFDP=SINIP*SINOP
      BETDP=SINIP*COSOP
      DALF=PH*COSOP+PINC*COSIP*SINOP
      DBET=-PH*SINOP+PINC*COSIP*COSOP
      ALFDP=ALFDP+DALF
      BETDP=BETDP+DBET
      XNODEP=FMOD2P(XNODEP)
      XLS = XMAP+OMEGAP+COSIP*XNODEP
      DLS=PL+PGH-PINC*XNODEP*SINIP
      XLS=XLS+DLS
      XNOH=XNODEP
      XNODEP=ACTAN(ALFDP,BETDP)
      IF(DABS(XNOH-XNODEP) .GT. PI) THEN
         IF(XNODEP .LT. XNOH) THEN
             XNODEP=XNODEP+TWOPI
         ELSE
             XNODEP=XNODEP-TWOPI
         ENDIF
      ENDIF
      XMAP=XMAP+PL
      OMEGAP = XLS-XMAP-COSIP*XNODEP
      RETURN
      END
