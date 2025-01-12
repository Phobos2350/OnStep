//--------------------------------------------------------------------------------------------------
// Goto, commands to move the telescope to an location or to report the current location

// check if goto/sync is valid
GotoErrors validateGoto() {
  // Check state
  if (faultAxis1 || faultAxis2)                return GOTO_ERR_HARDWARE_FAULT;
  if (!axis1Enabled)                           return GOTO_ERR_STANDBY;
  if (parkStatus != NotParked)                 return GOTO_ERR_PARK;
  if (guideDirAxis1 || guideDirAxis2)          return GOTO_ERR_IN_MOTION;
  if (trackingSyncInProgress())                return GOTO_ERR_GOTO;
  if (trackingState == TrackingMoveTo)         return GOTO_ERR_GOTO;
  return GOTO_ERR_NONE;
}

GotoErrors validateGotoCoords(double HA, double Dec, double Alt) {
  // Check coordinates
  if (Alt < minAlt)                            return GOTO_ERR_BELOW_HORIZON;
  if (Alt > maxAlt)                            return GOTO_ERR_ABOVE_OVERHEAD;
  if (Dec < AXIS2_LIMIT_MIN)                   return GOTO_ERR_OUTSIDE_LIMITS;
  if (Dec > AXIS2_LIMIT_MAX)                   return GOTO_ERR_OUTSIDE_LIMITS;
  if ((fabs(HA) > AXIS1_LIMIT_UNDER_POLE))     return GOTO_ERR_OUTSIDE_LIMITS;
  return GOTO_ERR_NONE;
}

GotoErrors validateGoToEqu(double RA, double Dec) {
  double a,z;
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);
  GotoErrors r=validateGoto(); if (r != GOTO_ERR_NONE) return r;
  r=validateGotoCoords(HA,Dec,a);
  return r;
}

void setLastErrorForGoto(GotoErrors r) {
  // check to see if lastError was a goto error and clear it if so
  if ((lastError > ERR_GOTO_ERR_NONE) && (lastError <= ERR_GOTO_ERR_UNSPECIFIED)) lastError=ERR_NONE;
  // if an goto error exists log it
  if (r != GOTO_ERR_NONE) lastError=(Errors)((int)ERR_GOTO_ERR_NONE+(int)r);
}

// syncs the telescope/mount to the sky
GotoErrors syncEqu(double RA, double Dec) {
  double a,z;

  // Convert RA into hour angle, get altitude
  // hour angleTrackingMoveTo
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  GotoErrors r=validateGoto(); setLastErrorForGoto(r);
  if ((r != GOTO_ERR_NONE) && (r != GOTO_ERR_STANDBY)) return r;
  r=validateGotoCoords(HA,Dec,a); setLastErrorForGoto(r);
  if (r != GOTO_ERR_NONE) return r;

  double Axis1,Axis2;
#if MOUNT_TYPE == ALTAZM
  equToHor(HA,Dec,&Axis2,&Axis1);
  Align.horToInstr(Axis2,Axis1,&Axis2,&Axis1,getInstrPierSide());
  Axis1=haRange(Axis1);
#else
  Align.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());
#endif

  // just turn on tracking
  if (atHome) { trackingState=TrackingSidereal; enableStepperDrivers(); }

  // west side of pier - we're in the eastern sky and the HA's are negative
  // east side of pier - we're in the western sky and the HA's are positive
  int newPierSide=getInstrPierSide();
  if (meridianFlip != MeridianFlipNever) {
    if (atHome) { if (Axis1 < 0) newPierSide=PierSideWest; else newPierSide=PierSideEast; } else // best side of pier decided based on meridian
#if SYNC_CURRENT_PIER_SIDE_ONLY == OFF
    if (preferredPierSide == PPS_WEST) { newPierSide=PierSideWest; if (haRange(Axis1) > degreesPastMeridianW) newPierSide=PierSideEast; } else
    if (preferredPierSide == PPS_EAST) { newPierSide=PierSideEast; if (haRange(Axis1) < -degreesPastMeridianE) newPierSide=PierSideWest; } else
#endif
    {
      if ((getInstrPierSide() == PierSideWest) && (haRange(Axis1) > degreesPastMeridianW)) newPierSide=PierSideEast;
      if ((getInstrPierSide() == PierSideEast) && (haRange(Axis1) < -degreesPastMeridianE)) newPierSide=PierSideWest;
    }

#if SYNC_CURRENT_PIER_SIDE_ONLY == ON
    if ((!atHome) && (newPierSide != getInstrPierSide())) return GOTO_ERR_OUTSIDE_LIMITS;
#endif

  } else {
    // always on the "east" side of pier - we're in the western sky and the HA's are positive
    // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
    newPierSide=PierSideEast;
  }

  setIndexAxis1(Axis1,newPierSide);
  setIndexAxis2(Axis2,newPierSide);

  return GOTO_ERR_NONE;
}

// syncs internal counts to shaft encoder position (in degrees)
GotoErrors syncEnc(double EncAxis1, double EncAxis2) {
  // validate
  GotoErrors f=validateGoto(); if (f != GOTO_ERR_NONE) return f;

  // no sync from encoders during an alignment!
  if (alignActive()) return GOTO_ERR_NONE;

  long e1=EncAxis1*(double)AXIS1_STEPS_PER_DEGREE;
  long e2=EncAxis2*(double)AXIS2_STEPS_PER_DEGREE;

  long a1,a2;
  cli();
  a1=posAxis1;
  a2=posAxis2;
  sei();
  a1+=indexAxis1Steps;
  a2+=indexAxis2Steps;

  long delta1=a1-e1;
  long delta2=a2-e2;

  indexAxis1Steps-=delta1;
  indexAxis1=(double)indexAxis1Steps/(double)AXIS1_STEPS_PER_DEGREE;
  indexAxis2Steps-=delta2;
  indexAxis2=(double)indexAxis2Steps/(double)AXIS2_STEPS_PER_DEGREE;

  return GOTO_ERR_NONE;
}

// get internal counts as shaft encoder position (in degrees)
int getEnc(double *EncAxis1, double *EncAxis2) {
  long a1,a2;
  cli();
  a1=posAxis1;
  a2=posAxis2;
  sei();
  a1+=indexAxis1Steps;
  a2+=indexAxis2Steps;
  
  *EncAxis1=(double)a1/(double)AXIS1_STEPS_PER_DEGREE;
  *EncAxis2=(double)a2/(double)AXIS2_STEPS_PER_DEGREE;

  return 0;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
 
#if MOUNT_TYPE != ALTAZM
  HA=getInstrAxis1();
  *Dec=getInstrAxis2();
  // apply pointing model
  Align.instrToEqu(HA,*Dec,&HA,Dec,getInstrPierSide());
#else
  double Z=getInstrAxis1();
  double A=getInstrAxis2();
  // apply pointing model
  Align.instrToHor(A,Z,&A,&Z,getInstrPierSide());
  horToEqu(A,Z,&HA,Dec);
#endif

  // return either the RA or the HA depending on returnHA
  if (!returnHA) {
    *RA=(LST()*15.0-HA);
    while (*RA >= 360.0) *RA-=360.0;
    while (*RA < 0.0) *RA+=360.0;
  } else *RA=HA;
  
  return true;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getApproxEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
#if MOUNT_TYPE != ALTAZM
  HA=getInstrAxis1();
  *Dec=getInstrAxis2();
#else
  double Z=getInstrAxis1();
  double A=getInstrAxis2();
  horToEqu(A,Z,&HA,Dec);
#endif

  HA=haRange(HA);
  if (*Dec > 90.0) *Dec=+90.0;
  if (*Dec < -90.0) *Dec=-90.0;
  
  // return either the RA or the HA depending on returnHA
  if (!returnHA) {
    *RA=degRange(LST()*15.0-HA);
  } else *RA=HA;
  return true;
}

// gets the telescopes current Alt and Azm
boolean getHor(double *Alt, double *Azm) {
  double h,d;
  getEqu(&h,&d,true);
  equToHor(h,d,Alt,Azm);
  return true;
}

// causes a goto to the same RA/Dec on the opposite pier side if possible
GotoErrors goToHere(bool toEastOnly) {
  bool verified=false;
  PreferredPierSide p=preferredPierSide;
  if (meridianFlip == MeridianFlipNever) return GOTO_ERR_OUTSIDE_LIMITS;
  cli(); long h=posAxis1+indexAxis1Steps; sei();
  if ((!toEastOnly) && (getInstrPierSide() == PierSideEast) && (h < (degreesPastMeridianW*(long)AXIS1_STEPS_PER_DEGREE))) { verified=true; preferredPierSide=PPS_WEST; }
  if ((getInstrPierSide() == PierSideWest) && (h > (-degreesPastMeridianE*(long)AXIS1_STEPS_PER_DEGREE))) { verified=true; preferredPierSide=PPS_EAST; }
  if (verified) {
    double newRA,newDec;
    getEqu(&newRA,&newDec,false);
    GotoErrors r=goToEqu(newRA,newDec);
    preferredPierSide=p;
    return r;
  } else return GOTO_ERR_OUTSIDE_LIMITS;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
GotoErrors goToEqu(double RA, double Dec) {
  double a,z;
  double Axis1,Axis2;
  double Axis1Alt,Axis2Alt;

  // Convert RA into hour angle, get altitude
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  GotoErrors r=validateGoto(); setLastErrorForGoto(r);
#ifndef GOTO_ERR_GOTO_OFF
  if (r == GOTO_ERR_GOTO) { if (!abortSlew) abortSlew=StartAbortSlew; } 
#endif
  if (r != GOTO_ERR_NONE) return r;
  r=validateGotoCoords(HA,Dec,a); setLastErrorForGoto(r);
  if (r != GOTO_ERR_NONE) return r;

#if MOUNT_TYPE == ALTAZM
  equToHor(HA,Dec,&a,&z);
  Align.horToInstr(a,z,&a,&z,getInstrPierSide());
  z=haRange(z);

  if ((AXIS1_LIMIT_MAXAZM > 180.0) && (AXIS1_LIMIT_MAXAZM <= 360.0)) {
    // adjust coordinate range to allow going past 180 deg.
    // position a1 is 0..180
    double a1=getInstrAxis1();
    if (a1 >= 0) {
      // and goto z is in -0..-180
      if (z < 0) {
        // the alternate z1 is in 180..360
        double z1=z+360.0;
        if ((z1 < AXIS1_LIMIT_MAXAZM) && (dist(a1,z) > dist(a1,z1))) z=z1;
      }
    }
    // position a1 -0..-180
    if (a1 < 0) { 
      // and goto z is in 0..180
      if (z > 0) {
        // the alternate z1 is in -360..-180
        double z1=z-360.0;
        if ((z1 > -AXIS1_LIMIT_MAXAZM) && (dist(a1,z) > dist(a1,z1))) z=z1;
      }
    }
  }
  
  Axis1=z;
  Axis2=a;
  Axis1Alt=z;
  Axis2Alt=a;
#else
  // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
  Align.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());

  // as above... for the opposite pier side just incase we need to do a meridian flip
  int p=PierSideNone; if (getInstrPierSide() == PierSideEast) p=PierSideWest; else if (getInstrPierSide() == PierSideWest) p=PierSideEast;
  Align.equToInstr(HA,Dec,&Axis1Alt,&Axis2Alt,p);
#endif

  // goto function takes HA and Dec in steps
  // when in align mode, force pier side
  byte thisPierSide = PierSideBest;
  if (meridianFlip != MeridianFlipNever) {
    if (preferredPierSide == PPS_WEST) thisPierSide=PierSideWest;
    if (preferredPierSide == PPS_EAST) thisPierSide=PierSideEast;
  }

  return goTo(Axis1,Axis2,Axis1Alt,Axis2Alt,thisPierSide);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
GotoErrors goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  horToEqu(*Alt,*Azm,&HA,&Dec);
  double RA=degRange(LST()*15.0-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination - both are in steps.  Alternate targets are used when a meridian flip occurs
GotoErrors goTo(double thisTargetAxis1, double thisTargetAxis2, double altTargetAxis1, double altTargetAxis2, int gotoPierSide) {
  atHome=false;
  int thisPierSide=getInstrPierSide();

  if (meridianFlip != MeridianFlipNever) {
    // where the allowable hour angles are
    double eastOfPierMaxHA= 180.0;
    double eastOfPierMinHA=-degreesPastMeridianE;
    double westOfPierMaxHA= degreesPastMeridianW;
    double westOfPierMinHA=-180.0;

    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((gotoPierSide != PierSideBest) && (thisPierSide != gotoPierSide)) {
      if (thisPierSide == PierSideEast) eastOfPierMinHA= degreesPastMeridianW;
      if (thisPierSide == PierSideWest) westOfPierMaxHA=-degreesPastMeridianE;
    }
    
    // if doing a meridian flip, use the opposite pier side coordinates
    if (thisPierSide == PierSideEast) {
      if ((thisTargetAxis1 > eastOfPierMaxHA) || (thisTargetAxis1 < eastOfPierMinHA)) {
        thisPierSide=PierSideFlipEW1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 > westOfPierMaxHA) return GOTO_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide == PierSideWest) {
      if ((thisTargetAxis1 > westOfPierMaxHA) || (thisTargetAxis1 < westOfPierMinHA)) {
        thisPierSide=PierSideFlipWE1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 < eastOfPierMinHA) return GOTO_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide == PierSideNone) {
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetAxis1 < 0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        thisPierSide=PierSideWest;
      } else {
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        thisPierSide=PierSideEast;
      }
    }
  } else {
    if (getInstrPierSide() == PierSideNone) {
        // always on the "east" side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
        thisPierSide=PierSideEast;
    }
  }
  
  // final validation
#if MOUNT_TYPE == ALTAZM
  // allow +/- 360 in Az
  if (((thisTargetAxis1 > AXIS1_LIMIT_MAXAZM) || (thisTargetAxis1 < -AXIS1_LIMIT_MAXAZM)) || ((thisTargetAxis2 > 180.0) || (thisTargetAxis2 < -180.0))) return GOTO_ERR_UNSPECIFIED;
#else
  if (((thisTargetAxis1 > 180.0) || (thisTargetAxis1 < -180.0)) || ((thisTargetAxis2 > 180.0) || (thisTargetAxis2 < -180.0))) return GOTO_ERR_UNSPECIFIED;
#endif
  lastTrackingState=trackingState;

  cli();
  trackingState=TrackingMoveTo;
  SiderealClockSetInterval(siderealInterval);

  startAxis1=posAxis1;
  startAxis2=posAxis2;

  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;
  sei();

  int p=PierSideEast; switch (thisPierSide) { case PierSideWest: case PierSideFlipEW1: p=PierSideWest; break; }
  setTargetAxis1(thisTargetAxis1,p);
  setTargetAxis2(thisTargetAxis2,p);

#if (MFLIP_SKIP_HOME == ON)
  boolean gotoDirect=true;
#else
  boolean gotoDirect=false;
#endif

  if (!pauseHome && gotoDirect) {
    if (thisPierSide == PierSideFlipWE1) pierSideControl=PierSideEast; else
    if (thisPierSide == PierSideFlipEW1) pierSideControl=PierSideWest; else pierSideControl=thisPierSide;
  } else pierSideControl=thisPierSide;

  reactivateBacklashComp();
#if MOUNT_TYPE != ALTAZM
  disablePec();
#endif
  soundAlert();
  stepperModeGoto();

  return GOTO_ERR_NONE;
}
