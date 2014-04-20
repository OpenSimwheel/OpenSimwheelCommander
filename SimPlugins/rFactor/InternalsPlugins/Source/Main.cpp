//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Source File                                   ﬁ
//›                                                                         ﬁ
//› Description: Declarations for the Internals Example Plugin              ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› This source code module, and all information, data, and algorithms      ﬁ
//› associated with it, are part of CUBE technology (tm).                   ﬁ
//›                 PROPRIETARY AND CONFIDENTIAL                            ﬁ
//› Copyright (c) 1996-2007 Image Space Incorporated.  All rights reserved. ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› Change history:                                                         ﬁ
//›   tag.2005.11.30: created                                               ﬁ
//›                                                                         ﬁ
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

#include "Main.hpp"          // corresponding header file
#include <math.h>               // for atan2, sqrt
#include <stdio.h>              // for sample output


struct MMFOpenSimwheel
{
    float Gear;
    float EngineRPM;
    float EngineMaxRPM;

	float Speed;

    float GLongitudinal;
    float GLateral;
    float GVertical;

    float Pitch;
    float Roll;
    float Yaw;

    float Brake;
    float Throttle;
    float Clutch;

    float Steering;
    float SteeringArmForce;
	float ForceFeedbackValue;

    bool InSession;
    bool InRealtime;

    char TrackName[64];
    char VehicleName[64];

    unsigned int Tick;
} static MMF;

static LPVOID lpvMem = NULL;      // pointer to shared memory
static HANDLE hMapObject = NULL;  // handle to file mapping
static unsigned int tick = 0;

// plugin information
unsigned g_uPluginID          = 0;
char     g_szPluginName[]     = "OpenSimwheel Feeder Plugin";
unsigned g_uPluginVersion     = 001;
unsigned g_uPluginObjectCount = 1;
InternalsPluginInfo g_PluginInfo;



// interface to plugin information
extern "C" __declspec(dllexport)
const char* __cdecl GetPluginName() { return g_szPluginName; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginVersion() { return g_uPluginVersion; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginObjectCount() { return g_uPluginObjectCount; }

// get the plugin-info object used to create the plugin.
extern "C" __declspec(dllexport)
PluginObjectInfo* __cdecl GetPluginObjectInfo( const unsigned uIndex )
{
  switch(uIndex)
  {
    case 0:
      return  &g_PluginInfo;
    default:
      return 0;
  }
}


// InternalsPluginInfo class

InternalsPluginInfo::InternalsPluginInfo()
{
  // put together a name for this plugin
  sprintf( m_szFullName, "%s - %s", g_szPluginName, InternalsPluginInfo::GetName() );
}

const char*    InternalsPluginInfo::GetName()     const { return rFactorOpenSimwheelPlugin::GetName(); }
const char*    InternalsPluginInfo::GetFullName() const { return m_szFullName; }
const char*    InternalsPluginInfo::GetDesc()     const { return "An MMF Feeder for the OpenSimwheel Commander"; }
const unsigned InternalsPluginInfo::GetType()     const { return rFactorOpenSimwheelPlugin::GetType(); }
const char*    InternalsPluginInfo::GetSubType()  const { return rFactorOpenSimwheelPlugin::GetSubType(); }
const unsigned InternalsPluginInfo::GetVersion()  const { return rFactorOpenSimwheelPlugin::GetVersion(); }
void*          InternalsPluginInfo::Create()      const { return new rFactorOpenSimwheelPlugin(); }


// InternalsPlugin class

const char rFactorOpenSimwheelPlugin::m_szName[] = "InternalsPlugin";
const char rFactorOpenSimwheelPlugin::m_szSubType[] = "Internals";
const unsigned rFactorOpenSimwheelPlugin::m_uID = 1;
const unsigned rFactorOpenSimwheelPlugin::m_uVersion = 3;  // set to 3 for InternalsPluginV3 functionality and added graphical and vehicle info


PluginObjectInfo *rFactorOpenSimwheelPlugin::GetInfo()
{
  return &g_PluginInfo;
}

static const wchar_t* RFACTOR_OSW_MEMMAPFILENAME     = L"Local\\rFactorOpenSimwheelMMF";

void rFactorOpenSimwheelPlugin::Startup()
{
	tick = tick + 1;
	MMF.Tick = tick;

	hMapObject = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MMF), TEXT("Local\\rFactorOpenSimwheelMMF"));

    lpvMem = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
}


void rFactorOpenSimwheelPlugin::Shutdown()
{
	tick = tick + 1;
	UnmapViewOfFile(lpvMem);
	CloseHandle(hMapObject);
}


void rFactorOpenSimwheelPlugin::StartSession()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InSession=true;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void rFactorOpenSimwheelPlugin::EndSession()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InSession=false;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void rFactorOpenSimwheelPlugin::EnterRealtime()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InRealtime=true;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void rFactorOpenSimwheelPlugin::ExitRealtime()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InRealtime=false;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void rFactorOpenSimwheelPlugin::UpdateTelemetry( const TelemInfoV2 &info )
{
	tick = tick + 1;
	MMF.Tick = tick;

	MMF.SteeringArmForce = info.mSteeringArmForce;
	MMF.Steering = info.mUnfilteredSteering;

	float metersPerSec = sqrtf( ( info.mLocalVel.x * info.mLocalVel.x ) +
                                     ( info.mLocalVel.y * info.mLocalVel.y ) +
                                     ( info.mLocalVel.z * info.mLocalVel.z ) );
	MMF.Speed = metersPerSec*3.6f;
	
	MMF.Brake = info.mUnfilteredBrake;
	MMF.Throttle = info.mUnfilteredThrottle;
	MMF.Clutch = info.mUnfilteredClutch;

	MMF.GLongitudinal = info.mLocalAccel.z;
	MMF.GLateral = info.mLocalAccel.x;
	MMF.GVertical = info.mLocalAccel.y;

	MMF.Gear = info.mGear;
	MMF.EngineRPM = info.mEngineRPM;
	MMF.EngineMaxRPM=info.mEngineMaxRPM;

	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void rFactorOpenSimwheelPlugin::UpdateGraphics( const GraphicsInfoV2 &info )
{
	tick = tick + 1;
	MMF.Tick = tick;
}


bool rFactorOpenSimwheelPlugin::CheckHWControl( const char * const controlName, float &fRetVal )
{
  return( false );
}


bool rFactorOpenSimwheelPlugin::ForceFeedback( float &forceValue )
{
  // Note that incoming value is the game's computation, in case you're interested.

  // CHANGE COMMENTS TO ENABLE FORCE EXAMPLE

	MMF.ForceFeedbackValue = forceValue;
	MMF.Tick = tick;
	memcpy(lpvMem, &MMF, sizeof(MMF));

	return( true );

  // I think the bounds are -11500 to 11500 ...
//  forceValue = 11500.0f * sinf( mET );
//  return( true );
}


void rFactorOpenSimwheelPlugin::UpdateScoring( const ScoringInfoV2 &info )
{
 
}


bool rFactorOpenSimwheelPlugin::RequestCommentary( CommentaryRequestInfo &info )
{
  return( false );
}

