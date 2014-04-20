

#include "Example.hpp"          // corresponding header file
#include <math.h>               // for atan2, sqrt
#include <stdio.h>              // for sample output

#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <memory.h>

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

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "OpenSimwheel Feeder Plugin" ); }


extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 1 ); } // InternalsPluginV01 functionality

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( (PluginObject *) new ExampleInternalsPlugin ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( (ExampleInternalsPlugin *) obj ); }


// ExampleInternalsPlugin class

void ExampleInternalsPlugin::WriteToAllExampleOutputFiles( const char * const openStr, const char * const msg )
{

}

static const wchar_t* RFACTOR_OSW_MEMMAPFILENAME     = L"Local\\rFactor2OpenSimwheelMMF";

void ExampleInternalsPlugin::Startup( long version )
{
	tick = tick + 1;
	MMF.Tick = tick;

	hMapObject = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MMF), TEXT("Local\\rFactor2OpenSimwheelMMF"));

    lpvMem = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
}


void ExampleInternalsPlugin::Shutdown()
{
	tick = tick + 1;
	UnmapViewOfFile(lpvMem);
	CloseHandle(hMapObject);
}


void ExampleInternalsPlugin::StartSession()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InSession=true;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void ExampleInternalsPlugin::EndSession()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InSession=false;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void ExampleInternalsPlugin::EnterRealtime()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InRealtime=true;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void ExampleInternalsPlugin::ExitRealtime()
{
	tick = tick + 1;
	MMF.Tick = tick;
	MMF.InRealtime=false;
	memcpy(lpvMem, &MMF, sizeof(MMF));
}


void ExampleInternalsPlugin::UpdateTelemetry( const TelemInfoV01 &info )
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


void ExampleInternalsPlugin::UpdateGraphics( const GraphicsInfoV01 &info )
{
		tick = tick + 1;
	MMF.Tick = tick;
}


bool ExampleInternalsPlugin::CheckHWControl( const char * const controlName, float &fRetVal )
{
 return( false );
}


bool ExampleInternalsPlugin::ForceFeedback( float &forceValue )
{
	MMF.ForceFeedbackValue = forceValue;
	MMF.Tick = tick;
	memcpy(lpvMem, &MMF, sizeof(MMF));

	return( true );
}


void ExampleInternalsPlugin::UpdateScoring( const ScoringInfoV01 &info )
{

}


bool ExampleInternalsPlugin::RequestCommentary( CommentaryRequestInfoV01 &info )
{
 return( false );
}
