#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include "InternalsPlugin.hpp"
#include <windows.h>
#include <conio.h>
#include <tchar.h>



// This is used for app to find out information about the plugin
class InternalsPluginInfo : public PluginObjectInfo
{
 public:

  // Constructor/destructor
  InternalsPluginInfo();
  ~InternalsPluginInfo() {}

  // Derived from base class PluginObjectInfo
  virtual const char*    GetName()     const;
  virtual const char*    GetFullName() const;
  virtual const char*    GetDesc()     const;
  virtual const unsigned GetType()     const;
  virtual const char*    GetSubType()  const;
  virtual const unsigned GetVersion()  const;
  virtual void*          Create() const;

 private:

  char m_szFullName[128];
};


// This is used for the app to use the plugin for its intended purpose
class rFactorOpenSimwheelPlugin : public InternalsPluginV3
{
 protected:
  
  const static char m_szName[];
  const static char m_szSubType[];
  const static unsigned m_uID;
  const static unsigned m_uVersion;

 public:

  // Constructor/destructor
  rFactorOpenSimwheelPlugin() {}
  ~rFactorOpenSimwheelPlugin() {}

  // Called from class InternalsPluginInfo to return specific information about plugin
  static const char *   GetName()                     { return m_szName; }
  static const unsigned GetType()                     { return PO_INTERNALS; }
  static const char *   GetSubType()                  { return m_szSubType; }
  static const unsigned GetVersion()                  { return m_uVersion; }

  // Derived from base class PluginObject
  void                  Destroy()                     { Shutdown(); }  // poorly named ... doesn't destroy anything
  PluginObjectInfo *    GetInfo();
  unsigned              GetPropertyCount() const      { return 0; }
  PluginObjectProperty *GetProperty( const char * )   { return 0; }
  PluginObjectProperty *GetProperty( const unsigned ) { return 0; }

  // These are the functions derived from base class InternalsPlugin
  // that can be implemented.
  void Startup();         // game startup
  void Shutdown();        // game shutdown

  void EnterRealtime();   // entering realtime
  void ExitRealtime();    // exiting realtime

  void StartSession();    // session has started
  void EndSession();      // session has ended

  // GAME OUTPUT
  bool WantsTelemetryUpdates() { return( true ); } // CHANGE TO TRUE TO ENABLE TELEMETRY EXAMPLE!
  void UpdateTelemetry( const TelemInfoV2 &info );

  bool WantsGraphicsUpdates() { return( false ); } // CHANGE TO TRUE TO ENABLE GRAPHICS EXAMPLE!
  void UpdateGraphics( const GraphicsInfoV2 &info );

  // GAME INPUT
  bool HasHardwareInputs() { return( false ); } // CHANGE TO TRUE TO ENABLE HARDWARE EXAMPLE!
  void UpdateHardware( const float fDT ) { mET += fDT; } // update the hardware with the time between frames
  void EnableHardware() { mEnabled = true; }             // message from game to enable hardware
  void DisableHardware() { mEnabled = false; }           // message from game to disable hardware

  // See if the plugin wants to take over a hardware control.  If the plugin takes over the
  // control, this method returns true and sets the value of the float pointed to by the
  // second arg.  Otherwise, it returns false and leaves the float unmodified.
  bool CheckHWControl( const char * const controlName, float &fRetVal );

  bool ForceFeedback( float &forceValue );  // SEE FUNCTION BODY TO ENABLE FORCE EXAMPLE

  // SCORING OUTPUT
  bool WantsScoringUpdates() { return( false ); } // CHANGE TO TRUE TO ENABLE SCORING EXAMPLE!
  void UpdateScoring( const ScoringInfoV2 &info );

  // COMMENTARY INPUT
  bool RequestCommentary( CommentaryRequestInfo &info );  // SEE FUNCTION BODY TO ENABLE COMMENTARY EXAMPLE

 private:

  float mET;  // needed for the hardware example
  bool mEnabled; // needed for the hardware example

  HANDLE m_hFileMMF;
  LPVOID m_pViewMMFFile;
};


#endif // _INTERNALS_EXAMPLE_H

