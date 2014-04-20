//ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ
//İ                                                                         Ş
//İ Module: Header file for internals plugin                                Ş
//İ                                                                         Ş
//İ Description: Interface declarations for internals plugin                Ş
//İ                                                                         Ş
//İ This source code module, and all information, data, and algorithms      Ş
//İ associated with it, are part of isiMotor Technology (tm).               Ş
//İ                 PROPRIETARY AND CONFIDENTIAL                            Ş
//İ Copyright (c) 1996-2013 Image Space Incorporated.  All rights reserved. Ş
//İ                                                                         Ş
//İ Change history:                                                         Ş
//İ   tag.2005.11.29: created                                               Ş
//İ                                                                         Ş
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß

#ifndef _INTERNALS_PLUGIN_HPP_
#define _INTERNALS_PLUGIN_HPP_

#include "PluginObjects.hpp"     // base class for plugin objects to derive from
#include <cmath>                // for sqrt()
#include <windows.h>             // for HWND


// rF currently uses 4-byte packing ... whatever the current packing is will
// be restored at the end of this include with another #pragma.
#pragma pack( push, 4 )


//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
//³ Version01 Structures                                                   ³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

struct TelemVect3
{
  double x, y, z;

  void Set( const double a, const double b, const double c )  { x = a; y = b; z = c; }

  // Allowed to reference as [0], [1], or [2], instead of .x, .y, or .z, respectively
        double &operator[]( long i )               { return( ( &x )[ i ] ); }
  const double &operator[]( long i ) const         { return( ( &x )[ i ] ); }
};


struct TelemQuat
{
  double w, x, y, z;

  // Convert this quaternion to a matrix
  void ConvertQuatToMat( TelemVect3 ori[3] ) const
  {
    const double x2 = x + x;
    const double xx = x * x2;
    const double y2 = y + y;
    const double yy = y * y2;
    const double z2 = z + z;
    const double zz = z * z2;
    const double xz = x * z2;
    const double xy = x * y2;
    const double wy = w * y2;
    const double wx = w * x2;
    const double wz = w * z2;
    const double yz = y * z2;
    ori[0][0] = (double) 1.0 - ( yy + zz );
    ori[0][1] = xy - wz;
    ori[0][2] = xz + wy;
    ori[1][0] = xy + wz;
    ori[1][1] = (double) 1.0 - ( xx + zz );
    ori[1][2] = yz - wx;
    ori[2][0] = xz - wy;
    ori[2][1] = yz + wx;
    ori[2][2] = (double) 1.0 - ( xx + yy );
  }

  // Convert a matrix to this quaternion
  void ConvertMatToQuat( const TelemVect3 ori[3] )
  {
    const double trace = ori[0][0] + ori[1][1] + ori[2][2] + (double) 1.0;
    if( trace > 0.0625f )
    {
      const double sqrtTrace = sqrt( trace );
      const double s = (double) 0.5 / sqrtTrace;
      w = (double) 0.5 * sqrtTrace;
      x = ( ori[2][1] - ori[1][2] ) * s;
      y = ( ori[0][2] - ori[2][0] ) * s;
      z = ( ori[1][0] - ori[0][1] ) * s;
    }
    else if( ( ori[0][0] > ori[1][1] ) && ( ori[0][0] > ori[2][2] ) )
    {
      const double sqrtTrace = sqrt( (double) 1.0 + ori[0][0] - ori[1][1] - ori[2][2] );
      const double s = (double) 0.5 / sqrtTrace;
      w = ( ori[2][1] - ori[1][2] ) * s;
      x = (double) 0.5 * sqrtTrace;
      y = ( ori[0][1] + ori[1][0] ) * s;
      z = ( ori[0][2] + ori[2][0] ) * s;
    }
    else if( ori[1][1] > ori[2][2] )
    {
      const double sqrtTrace = sqrt( (double) 1.0 + ori[1][1] - ori[0][0] - ori[2][2] );
      const double s = (double) 0.5 / sqrtTrace;
      w = ( ori[0][2] - ori[2][0] ) * s;
      x = ( ori[0][1] + ori[1][0] ) * s;
      y = (double) 0.5 * sqrtTrace;
      z = ( ori[1][2] + ori[2][1] ) * s;
    }
    else
    {
      const double sqrtTrace = sqrt( (double) 1.0 + ori[2][2] - ori[0][0] - ori[1][1] );
      const double s = (double) 0.5 / sqrtTrace;
      w = ( ori[1][0] - ori[0][1] ) * s;
      x = ( ori[0][2] + ori[2][0] ) * s;
      y = ( ori[1][2] + ori[2][1] ) * s;
      z = (double) 0.5 * sqrtTrace;
    }
  }
};


struct TelemWheelV01
{
  double mSuspensionDeflection;  // meters
  double mRideHeight;            // meters
  double mSuspForce;             // pushrod load in Newtons
  double mBrakeTemp;             // Celsius
  double mBrakePressure;         // currently 0.0-1.0, depending on driver input and brake balance; will convert to true brake pressure (kPa) in future

  double mRotation;              // radians/sec
  double mLateralPatchVel;       // lateral velocity at contact patch
  double mLongitudinalPatchVel;  // longitudinal velocity at contact patch
  double mLateralGroundVel;      // lateral velocity at contact patch
  double mLongitudinalGroundVel; // longitudinal velocity at contact patch
  double mCamber;                // radians (positive is left for left-side wheels, right for right-side wheels)
  double mLateralForce;          // Newtons
  double mLongitudinalForce;     // Newtons
  double mTireLoad;              // Newtons

  double mGripFract;             // an approximation of what fraction of the contact patch is sliding
  double mPressure;              // kPa (tire pressure)
  double mTemperature[3];        // Kelvin (subtract 273.16 to get Celsius), left/center/right (not to be confused with inside/center/outside!)
  double mWear;                  // wear (0.0-1.0, fraction of maximum) ... this is not necessarily proportional with grip loss
  char mTerrainName[16];         // the material prefixes from the TDF file
  unsigned char mSurfaceType;    // 0=dry, 1=wet, 2=grass, 3=dirt, 4=gravel, 5=rumblestrip
  bool mFlat;                    // whether tire is flat
  bool mDetached;                // whether wheel is detached

  double mVerticalTireDeflection;// how much is tire deflected from its (speed-sensitive) radius
  double mWheelYLocation;        // wheel's y location relative to vehicle y location
  double mToe;                   // current toe angle w.r.t. the vehicle

  unsigned char mExpansion[56];  // for future use
};


// Our world coordinate system is left-handed, with +y pointing up.
// The local vehicle coordinate system is as follows:
//   +x points out the left side of the car (from the driver's perspective)
//   +y points out the roof
//   +z points out the back of the car
// Rotations are as follows:
//   +x pitches up
//   +y yaws to the right
//   +z rolls to the right
// Note that ISO vehicle coordinates (+x forward, +y right, +z upward) are
// right-handed.  If you are using that system, be sure to negate any rotation
// or torque data because things rotate in the opposite direction.  In other
// words, a -z velocity in rFactor is a +x velocity in ISO, but a -z rotation
// in rFactor is a -x rotation in ISO!!!

struct TelemInfoV01
{
  // Time
  long mID;                      // slot ID (note that it can be re-used in multiplayer after someone leaves)
  double mDeltaTime;             // time since last update (seconds)
  double mElapsedTime;           // game session time
  long mLapNumber;               // current lap number
  double mLapStartET;            // time this lap was started
  char mVehicleName[64];         // current vehicle name
  char mTrackName[64];           // current track name

  // Position and derivatives
  TelemVect3 mPos;               // world position in meters
  TelemVect3 mLocalVel;          // velocity (meters/sec) in local vehicle coordinates
  TelemVect3 mLocalAccel;        // acceleration (meters/sec^2) in local vehicle coordinates

  // Orientation and derivatives
  TelemVect3 mOri[3];            // rows of orientation matrix (use TelemQuat conversions if desired), also converts local
                                 // vehicle vectors into world X, Y, or Z using dot product of rows 0, 1, or 2 respectively
  TelemVect3 mLocalRot;          // rotation (radians/sec) in local vehicle coordinates
  TelemVect3 mLocalRotAccel;     // rotational acceleration (radians/sec^2) in local vehicle coordinates

  // Vehicle status
  long mGear;                    // -1=reverse, 0=neutral, 1+=forward gears
  double mEngineRPM;             // engine RPM
  double mEngineWaterTemp;       // Celsius
  double mEngineOilTemp;         // Celsius
  double mClutchRPM;             // clutch RPM

  // Driver input
  double mUnfilteredThrottle;    // ranges  0.0-1.0
  double mUnfilteredBrake;       // ranges  0.0-1.0
  double mUnfilteredSteering;    // ranges -1.0-1.0 (left to right)
  double mUnfilteredClutch;      // ranges  0.0-1.0

  // Filtered input (various adjustments for rev or speed limiting, TC, ABS?, speed sensitive steering, clutch work for semi-automatic shifting, etc.)
  double mFilteredThrottle;      // ranges  0.0-1.0
  double mFilteredBrake;         // ranges  0.0-1.0
  double mFilteredSteering;      // ranges -1.0-1.0 (left to right)
  double mFilteredClutch;        // ranges  0.0-1.0

  // Misc
  double mSteeringArmForce;      // force on steering arms
  double mFront3rdDeflection;    // deflection at front 3rd spring
  double mRear3rdDeflection;     // deflection at rear 3rd spring

  // Aerodynamics
  double mFrontWingHeight;       // front wing height
  double mFrontRideHeight;       // front ride height
  double mRearRideHeight;        // rear ride height
  double mDrag;                  // drag
  double mFrontDownforce;        // front downforce
  double mRearDownforce;         // rear downforce

  // State/damage info
  double mFuel;                  // amount of fuel (liters)
  double mEngineMaxRPM;          // rev limit
  unsigned char mScheduledStops; // number of scheduled pitstops
  bool  mOverheating;            // whether overheating icon is shown
  bool  mDetached;               // whether any parts (besides wheels) have been detached
  bool  mHeadlights;             // whether headlights are on
  unsigned char mDentSeverity[8];// dent severity at 8 locations around the car (0=none, 1=some, 2=more)
  double mLastImpactET;          // time of last impact
  double mLastImpactMagnitude;   // magnitude of last impact
  TelemVect3 mLastImpactPos;     // location of last impact

  // Expanded
  double mEngineTq;              // current engine torque (including additive torque)
  long mCurrentSector;           // the current sector (zero-based) with the pitlane stored in the sign bit (example: entering pits from third sector gives 0x80000002)
  unsigned char mSpeedLimiter;   // whether speed limiter is on
  unsigned char mMaxGears;       // maximum forward gears
  unsigned char mFrontTireCompoundIndex;   // index within brand
  unsigned char mRearTireCompoundIndex;    // index within brand
  double mFuelCapacity;          // capacity in liters
  unsigned char mFrontFlapActivated;       // whether front flap is activated
  unsigned char mRearFlapActivated;        // whether rear flap is activated
  unsigned char mRearFlapLegalStatus;      // 0=disallowed, 1=criteria detected but not allowed quite yet, 2=allowed
  unsigned char mIgnitionStarter;          // 0=off 1=ignition 2=ignition+starter

  // Future use
  unsigned char mExpansion[228]; // for future use (note that the slot ID has been moved to mID above)

  // keeping this at the end of the structure to make it easier to replace in future versions
  TelemWheelV01 mWheel[4];       // wheel info (front left, front right, rear left, rear right)
};


struct GraphicsInfoV01
{
  TelemVect3 mCamPos;            // camera position
  TelemVect3 mCamOri[3];         // rows of orientation matrix (use TelemQuat conversions if desired), also converts local
  HWND mHWND;                    // app handle

  double mAmbientRed;
  double mAmbientGreen;
  double mAmbientBlue;
};


struct GraphicsInfoV02 : public GraphicsInfoV01
{
  long mID;                      // slot ID being viewed (-1 if invalid)

  // Camera types (some of these may only be used for *setting* the camera type in WantsToViewVehicle())
  //    0  = TV cockpit
  //    1  = cockpit
  //    2  = nosecam
  //    3  = swingman
  //    4  = trackside (nearest)
  //    5  = onboard000
  //       :
  //       :
  // 1004  = onboard999
  // 1005+ = (currently unsupported, in the future may be able to set/get specific trackside camera)
  long mCameraType;              // see above comments for possible values

  unsigned char mExpansion[128]; // for future use (possibly camera name)
};


struct CameraControlInfoV01
{
  long mID;                      // slot ID to view
  long mCameraType;              // see GraphicsInfoV02 comments for values

  unsigned char mExpansion[128]; // for future use (possibly camera name & positions/orientations)
};


struct MessageInfoV01
{
  char mText[128];               // message to display

  unsigned char mDestination;    // 0 = message center, 1 = chat (can be used for multiplayer chat commands)
  unsigned char mTranslate;      // 0 = do not attempt to translate, 1 = attempt to translate

  unsigned char mExpansion[126]; // for future use (possibly what color, what font, and seconds to display)
};


struct VehicleScoringInfoV01
{
  long mID;                      // slot ID (note that it can be re-used in multiplayer after someone leaves)
  char mDriverName[32];          // driver name
  char mVehicleName[64];         // vehicle name
  short mTotalLaps;              // laps completed
  signed char mSector;           // 0=sector3, 1=sector1, 2=sector2 (don't ask why)
  signed char mFinishStatus;     // 0=none, 1=finished, 2=dnf, 3=dq
  double mLapDist;               // current distance around track
  double mPathLateral;           // lateral position with respect to *very approximate* "center" path
  double mTrackEdge;             // track edge (w.r.t. "center" path) on same side of track as vehicle

  double mBestSector1;           // best sector 1
  double mBestSector2;           // best sector 2 (plus sector 1)
  double mBestLapTime;           // best lap time
  double mLastSector1;           // last sector 1
  double mLastSector2;           // last sector 2 (plus sector 1)
  double mLastLapTime;           // last lap time
  double mCurSector1;            // current sector 1 if valid
  double mCurSector2;            // current sector 2 (plus sector 1) if valid
  // no current laptime because it instantly becomes "last"

  short mNumPitstops;            // number of pitstops made
  short mNumPenalties;           // number of outstanding penalties
  bool mIsPlayer;                // is this the player's vehicle

  signed char mControl;          // who's in control: -1=nobody (shouldn't get this), 0=local player, 1=local AI, 2=remote, 3=replay (shouldn't get this)
  bool mInPits;                  // between pit entrance and pit exit (not always accurate for remote vehicles)
  unsigned char mPlace;          // 1-based position
  char mVehicleClass[32];        // vehicle class

  // Dash Indicators
  double mTimeBehindNext;        // time behind vehicle in next higher place
  long mLapsBehindNext;          // laps behind vehicle in next higher place
  double mTimeBehindLeader;      // time behind leader
  long mLapsBehindLeader;        // laps behind leader
  double mLapStartET;            // time this lap was started

  // Position and derivatives
  TelemVect3 mPos;               // world position in meters
  TelemVect3 mLocalVel;          // velocity (meters/sec) in local vehicle coordinates
  TelemVect3 mLocalAccel;        // acceleration (meters/sec^2) in local vehicle coordinates

  // Orientation and derivatives
  TelemVect3 mOri[3];            // rows of orientation matrix (use TelemQuat conversions if desired), also converts local
                                 // vehicle vectors into world X, Y, or Z using dot product of rows 0, 1, or 2 respectively
  TelemVect3 mLocalRot;          // rotation (radians/sec) in local vehicle coordinates
  TelemVect3 mLocalRotAccel;     // rotational acceleration (radians/sec^2) in local vehicle coordinates

  // tag.2012.03.01 - stopped casting some of these so variables now have names and mExpansion has shrunk, overall size and old data locations should be same
  unsigned char mHeadlights;     // status of headlights
  unsigned char mPitState;       // 0=none, 1=request, 2=entering, 3=stopped, 4=exiting
  unsigned char mServerScored;   // whether this vehicle is being scored by server (could be off in qualifying or racing heats)
  unsigned char mIndividualPhase;// game phases (described below) plus 9=after formation, 10=under yellow, 11=under blue (not used)

  long mQualification;           // 1-based, can be -1 when invalid

  double mTimeIntoLap;           // estimated time into lap
  double mEstimatedLapTime;      // estimated laptime used for 'time behind' and 'time into lap' (note: this may changed based on vehicle and setup!?)

  char mPitGroup[24];            // pit group (same as team name unless pit is shared)
  unsigned char mFlag;           // primary flag being shown to vehicle (currently only 0=green or 6=blue)
  unsigned char mUnused1;
  unsigned char mUnused2;
  unsigned char mUnused3;

  // Future use
  // tag.2012.04.06 - SEE ABOVE!
  unsigned char mExpansion[76];  // for future use
};


struct ScoringInfoV01
{
  char mTrackName[64];           // current track name
  long mSession;                 // current session (0=testday 1-4=practice 5-8=qual 9=warmup 10-13=race)
  double mCurrentET;             // current time
  double mEndET;                 // ending time
  long  mMaxLaps;                // maximum laps
  double mLapDist;               // distance around track
  char *mResultsStream;          // results stream additions since last update (newline-delimited and NULL-terminated)

  long mNumVehicles;             // current number of vehicles

  // Game phases:
  // 0 Before session has begun
  // 1 Reconnaissance laps (race only)
  // 2 Grid walk-through (race only)
  // 3 Formation lap (race only)
  // 4 Starting-light countdown has begun (race only)
  // 5 Green flag
  // 6 Full course yellow / safety car
  // 7 Session stopped
  // 8 Session over
  unsigned char mGamePhase;   

  // Yellow flag states (applies to full-course only)
  // -1 Invalid
  //  0 None
  //  1 Pending
  //  2 Pits closed
  //  3 Pit lead lap
  //  4 Pits open
  //  5 Last lap
  //  6 Resume
  //  7 Race halt (not currently used)
  signed char mYellowFlagState;

  signed char mSectorFlag[3];      // whether there are any local yellows at the moment in each sector (not sure if sector 0 is first or last, so test)
  unsigned char mStartLight;       // start light frame (number depends on track)
  unsigned char mNumRedLights;     // number of red lights in start sequence
  bool mInRealtime;                // in realtime as opposed to at the monitor
  char mPlayerName[32];            // player name (including possible multiplayer override)
  char mPlrFileName[64];           // may be encoded to be a legal filename

  // weather
  double mDarkCloud;               // cloud darkness? 0.0-1.0
  double mRaining;                 // raining severity 0.0-1.0
  double mAmbientTemp;             // temperature (Celsius)
  double mTrackTemp;               // temperature (Celsius)
  TelemVect3 mWind;                // wind speed
  double mOnPathWetness;           // on main path 0.0-1.0
  double mOffPathWetness;          // on main path 0.0-1.0

  // Future use
  unsigned char mExpansion[256];

  // keeping this at the end of the structure to make it easier to replace in future versions
  VehicleScoringInfoV01 *mVehicle; // array of vehicle scoring info's
};


struct CommentaryRequestInfoV01
{
  char mName[32];                  // one of the event names in the commentary INI file
  double mInput1;                  // first value to pass in (if any)
  double mInput2;                  // first value to pass in (if any)
  double mInput3;                  // first value to pass in (if any)
  bool mSkipChecks;                // ignores commentary detail and random probability of event

  // constructor (for noobs, this just helps make sure everything is initialized to something reasonable)
  CommentaryRequestInfoV01()       { mName[0] = 0; mInput1 = 0.0; mInput2 = 0.0; mInput3 = 0.0; mSkipChecks = false; }
};


//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
//³ Version02 Structures                                                   ³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

struct PhysicsOptionsV01
{
  unsigned char mTractionControl;  // 0 (off) - 3 (high)
  unsigned char mAntiLockBrakes;   // 0 (off) - 2 (high)
  unsigned char mStabilityControl; // 0 (off) - 2 (high)
  unsigned char mAutoShift;        // 0 (off), 1 (upshifts), 2 (downshifts), 3 (all)
  unsigned char mAutoClutch;       // 0 (off), 1 (on)
  unsigned char mInvulnerable;     // 0 (off), 1 (on)
  unsigned char mOppositeLock;     // 0 (off), 1 (on)
  unsigned char mSteeringHelp;     // 0 (off) - 3 (high)
  unsigned char mBrakingHelp;      // 0 (off) - 2 (high)
  unsigned char mSpinRecovery;     // 0 (off), 1 (on)
  unsigned char mAutoPit;          // 0 (off), 1 (on)
  unsigned char mAutoLift;         // 0 (off), 1 (on)
  unsigned char mAutoBlip;         // 0 (off), 1 (on)

  unsigned char mFuelMult;         // fuel multiplier (0x-7x)
  unsigned char mTireMult;         // tire wear multiplier (0x-7x)
  unsigned char mMechFail;         // mechanical failure setting; 0 (off), 1 (normal), 2 (timescaled)
  unsigned char mAllowPitcrewPush; // 0 (off), 1 (on)
  unsigned char mRepeatShifts;     // accidental repeat shift prevention (0-5; see PLR file)
  unsigned char mHoldClutch;       // for auto-shifters at start of race: 0 (off), 1 (on)
  unsigned char mAutoReverse;      // 0 (off), 1 (on)
  unsigned char mAlternateNeutral; // Whether shifting up and down simultaneously equals neutral
  float mManualShiftOverrideTime;  // time before auto-shifting can resume after recent manual shift
  float mAutoShiftOverrideTime;    // time before manual shifting can resume after recent auto shift
  float mSpeedSensitiveSteering;   // 0.0 (off) - 1.0
  float mSteerRatioSpeed;          // speed (m/s) under which lock gets expanded to full
};


struct EnvironmentInfoV01
{
  // TEMPORARY buffers (you should copy them if needed for later use) containing various paths that may be needed.  Each of these
  // could be relative ("UserData\") or full ("C:\BlahBlah\rFactorProduct\UserData\").
  // mPath[ 0 ] points to the UserData directory.
  // mPath[ 1 ] points to the CustomPluginOptions.ini filename.
  // mPath[ 2 ] points to the latest results file
  // (in the future, we may add paths for the current garage setup, fully upgraded physics files, etc., any other requests?)
  const char *mPath[ 16 ];
  unsigned char mExpansion[256];   // future use
};


struct ScreenInfoV01
{
  HWND mAppWindow;                      // Application window handle
  void *mDevice;                        // Cast type to LPDIRECT3DDEVICE9
  void *mRenderTarget;                  // Cast type to LPDIRECT3DTEXTURE9
  long mDriver;                         // Current video driver index

  long mWidth;                          // Screen width
  long mHeight;                         // Screen height
  long mPixelFormat;                    // Pixel format
  long mRefreshRate;                    // Refresh rate
  long mWindowed;                       // Really just a boolean whether we are in windowed mode

  long mOptionsWidth;                   // Width dimension of screen portion used by UI
  long mOptionsHeight;                  // Height dimension of screen portion used by UI
  long mOptionsLeft;                    // Horizontal starting coordinate of screen portion used by UI
  long mOptionsUpper;                   // Vertical starting coordinate of screen portion used by UI

  unsigned char mOptionsLocation;       // 0=main UI, 1=track loading, 2=monitor, 3=on track
  char mOptionsPage[ 31 ];              // the name of the options page

  unsigned char mExpansion[ 224 ];      // future use
};


struct CustomControlInfoV01
{
  // The name passed through CheckHWControl() will be the mUntranslatedName prepended with an underscore (e.g. "Track Map Toggle" -> "_Track Map Toggle")
  char mUntranslatedName[ 64 ];         // name of the control that will show up in UI (but translated if available)
  long mRepeat;                         // 0=registers once per hit, 1=registers once, waits briefly, then starts repeating quickly, 2=registers as long as key is down
  unsigned char mExpansion[ 64 ];       // future use
};


//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
//³ Plugin classes used to access internals                                ³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// Note: use class InternalsPluginV01 and have exported function GetPluginVersion() return 1, or
//       use class InternalsPluginV02 and have exported function GetPluginVersion() return 2, etc.
class InternalsPlugin : public PluginObject
{
 public:

  // General internals methods
  InternalsPlugin() {}
  virtual ~InternalsPlugin() {}

  // GAME FLOW NOTIFICATIONS
  virtual void Startup( long version ) {}                      // sim startup with version * 1000
  virtual void Shutdown() {}                                   // sim shutdown

  virtual void Load() {}                                       // scene/track load
  virtual void Unload() {}                                     // scene/track unload

  virtual void StartSession() {}                               // session started
  virtual void EndSession() {}                                 // session ended

  virtual void EnterRealtime() {}                              // entering realtime (where the vehicle can be driven)
  virtual void ExitRealtime() {}                               // exiting realtime

  // SCORING OUTPUT
  virtual bool WantsScoringUpdates() { return( false ); }      // whether we want scoring updates
  virtual void UpdateScoring( const ScoringInfoV01 &info ) {}  // update plugin with scoring info (approximately once per second)

  // GAME OUTPUT
  virtual long WantsTelemetryUpdates() { return( 1 ); }        // whether we want telemetry updates (0=no 1=player-only 2=all vehicles)
  virtual void UpdateTelemetry( const TelemInfoV01 &info ) {}  // update plugin with telemetry info

  virtual bool WantsGraphicsUpdates() { return( false ); }     // whether we want graphics updates
  virtual void UpdateGraphics( const GraphicsInfoV01 &info ) {}// update plugin with graphics info

  // COMMENTARY INPUT
  virtual bool RequestCommentary( CommentaryRequestInfoV01 &info ) { return( false ); } // to use our commentary event system, fill in data and return true

  // GAME INPUT
  virtual bool HasHardwareInputs() { return( false ); }        // whether plugin has hardware plugins
  virtual void UpdateHardware( const double fDT ) {}           // update the hardware with the time between frames
  virtual void EnableHardware() {}                             // message from game to enable hardware
  virtual void DisableHardware() {}                            // message from game to disable hardware

  // See if the plugin wants to take over a hardware control.  If the plugin takes over the
  // control, this method returns true and sets the value of the double pointed to by the
  // second arg.  Otherwise, it returns false and leaves the double unmodified.
  virtual bool CheckHWControl( const char * const controlName, double &fRetVal ) { return true; }

  virtual bool ForceFeedback( float &forceValue ) { return( true ); } // alternate force feedback computation - return true if editing the value

  // ERROR FEEDBACK
  virtual void Error( const char * const msg ) {} // Called with explanation message if there was some sort of error in a plugin callback
};


class InternalsPluginV01 : public InternalsPlugin  // Version 01 is the exact same as the original
{
  // REMINDER: exported function GetPluginVersion() should return 1 if you are deriving from this InternalsPluginV01!
};


class InternalsPluginV02 : public InternalsPluginV01  // V02 contains everything from V01 plus the following:
{
  // REMINDER: exported function GetPluginVersion() should return 2 if you are deriving from this InternalsPluginV02!

 public:

  // This function is called occasionally
  virtual void SetPhysicsOptions( PhysicsOptionsV01 &options ) {}
};


class InternalsPluginV03 : public InternalsPluginV02  // V03 contains everything from V02 plus the following:
{
  // REMINDER: exported function GetPluginVersion() should return 3 if you are deriving from this InternalsPluginV03!

 public:

  virtual bool WantsToViewVehicle( CameraControlInfoV01 &camControl ) { return( false ); } // set ID and camera type and return true

  // EXTENDED GAME OUTPUT
  virtual void UpdateGraphics( const GraphicsInfoV02 &info )          {} // update plugin with extended graphics info

  // MESSAGE BOX INPUT
  virtual bool WantsToDisplayMessage( MessageInfoV01 &msgInfo )       { return( false ); } // set message and return true
};


class InternalsPluginV04 : public InternalsPluginV03  // V04 contains everything from V03 plus the following:
{
  // REMINDER: exported function GetPluginVersion() should return 4 if you are deriving from this InternalsPluginV04!

 public:

  // EXTENDED GAME FLOW NOTIFICATIONS
  virtual void SetEnvironment( const EnvironmentInfoV01 &info )       {} // may be called whenever the environment changes
};


class InternalsPluginV05 : public InternalsPluginV04  // V05 contains everything from V04 plus the following:
{
  // REMINDER: exported function GetPluginVersion() should return 5 if you are deriving from this InternalsPluginV05!

 public:

  // SCREEN INFO NOTIFICATIONS
  virtual void InitScreen( const ScreenInfoV01 &info )                {} // Now happens right after graphics device initialization
  virtual void UninitScreen( const ScreenInfoV01 &info )              {} // Now happens right before graphics device uninitialization

  virtual void DeactivateScreen( const ScreenInfoV01 &info )          {} // Window deactivation
  virtual void ReactivateScreen( const ScreenInfoV01 &info )          {} // Window reactivation

  virtual void RenderScreenBeforeOverlays( const ScreenInfoV01 &info ){} // before rFactor overlays
  virtual void RenderScreenAfterOverlays( const ScreenInfoV01 &info ) {} // after rFactor overlays

  virtual void PreReset( const ScreenInfoV01 &info )                  {} // after detecting device lost but before resetting (note: placeholder not currently called)
  virtual void PostReset( const ScreenInfoV01 &info )                 {} // after resetting

  // CUSTOM CONTROLS
  virtual bool InitCustomControl( CustomControlInfoV01 &info )        { return( false ); } // called repeatedly at startup until false is returned
};


//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// See #pragma at top of file
#pragma pack( pop )

#endif // _INTERNALS_PLUGIN_HPP_

