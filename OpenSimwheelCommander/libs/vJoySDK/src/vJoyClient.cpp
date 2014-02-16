// vJoyClient.cpp : Simple feeder application
//
// Supports both types of POV Hats

#include "stdafx.h"
#include "public.h"
#include "vjoyinterface.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#pragma comment( lib, "VJOYINTERFACE" )
#define  _CRT_SECURE_NO_WARNINGS

// Use either ROBUST or EFFICIENT functions
#define ROBUST
//#define EFFICIENT

int
__cdecl
_tmain(__in int argc, __in PZPWSTR argv)
{

	USHORT X, Y, Z, ZR, XR;							// Position of several axes
	JOYSTICK_POSITION	iReport;					// The structure that holds the full position data
	BYTE id=1;										// ID of the target vjoy device (Default is 1)
	UINT iInterface=1;								// Default target vJoy device
	BOOL ContinuousPOV=FALSE;						// Continuous POV hat (or 4-direction POV Hat)
	int count=0;


	// Get the ID of the target vJoy device
	if (argc>1 && wcslen(argv[1]))
		sscanf_s((char *)(argv[1]), "%d", &iInterface);


	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf("vJoy driver not enabled: Failed Getting vJoy attributes.\n");
		return -2;
	}
	else
	{
		_tprintf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", TEXT(GetvJoyManufacturerString()),  TEXT(GetvJoyProductString()), TEXT(GetvJoySerialNumberString()));
	};

	// Get the state of the requested device
	VjdStat status = GetVJDStatus(iInterface);
	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf("vJoy Device %d is already owned by this feeder\n", iInterface);
		break;
	case VJD_STAT_FREE:
		_tprintf("vJoy Device %d is free\n", iInterface);
		break;
	case VJD_STAT_BUSY:
		_tprintf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface);
		return -3;
	case VJD_STAT_MISS:
		_tprintf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface);
		return -4;
	default:
		_tprintf("vJoy Device %d general error\nCannot continue\n", iInterface);
		return -1;
	};

	// Check which axes are supported
	BOOL AxisX  = GetVJDAxisExist(iInterface, HID_USAGE_X);
	BOOL AxisY  = GetVJDAxisExist(iInterface, HID_USAGE_Y);
	BOOL AxisZ  = GetVJDAxisExist(iInterface, HID_USAGE_Z);
	BOOL AxisRX = GetVJDAxisExist(iInterface, HID_USAGE_RX);
	BOOL AxisRZ = GetVJDAxisExist(iInterface, HID_USAGE_RZ);
	// Get the number of buttons and POV Hat switchessupported by this vJoy device
	int nButtons  = GetVJDButtonNumber(iInterface);
	int ContPovNumber = GetVJDContPovNumber(iInterface);
	int DiscPovNumber = GetVJDDiscPovNumber(iInterface);

	// Print results
	_tprintf("\nvJoy Device %d capabilities:\n", iInterface);
	_tprintf("Numner of buttons\t\t%d\n", nButtons);
	_tprintf("Numner of Continuous POVs\t%d\n", ContPovNumber);
	_tprintf("Numner of Descrete POVs\t\t%d\n", DiscPovNumber);
	_tprintf("Axis X\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Y\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Z\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Rx\t\t%s\n", AxisRX?"Yes":"No");
	_tprintf("Axis Rz\t\t%s\n", AxisRZ?"Yes":"No");



	// Acquire the target
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(iInterface))))
	{
		_tprintf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	}
	else
	{
		_tprintf("Acquired: vJoy device number %d.\n", iInterface);
	}



	_tprintf("\npress enter to stat feeding");
	getchar();

	X = 20;
	Y = 30;
	Z = 40;
	XR = 60;
	ZR = 80;

	long value = 0;
	BOOL res = FALSE;

#ifdef ROBUST
	// Reset this device to default values
	ResetVJD(iInterface);

	// Feed the device in endless loop
	while(1)
	{
		// Set position of 4 axes
		res = SetAxis(X, iInterface, HID_USAGE_X);
		res = SetAxis(Y, iInterface, HID_USAGE_Y);
		res = SetAxis(Z, iInterface, HID_USAGE_Z);
		res = SetAxis(XR, iInterface, HID_USAGE_RX);
		res = SetAxis(ZR, iInterface, HID_USAGE_RZ);

		// Press/Release Buttons
		res = SetBtn(TRUE, iInterface, count/50);
		res = SetBtn(FALSE, iInterface, 1+count/50);

		// If Continuous POV hat switches installed - make them go round
		// For high values - put the switches in neutral state
		if (ContPovNumber)
		{
			if ((count*70) < 30000)
			{
				res = SetContPov((DWORD)(count*70)		, iInterface, 1);
				res = SetContPov((DWORD)(count*70)+2000	, iInterface, 2);
				res = SetContPov((DWORD)(count*70)+4000	, iInterface, 3);
				res = SetContPov((DWORD)(count*70)+6000	, iInterface, 4);
			}
			else
			{
				res = SetContPov(-1 , iInterface, 1);
				res = SetContPov(-1 , iInterface, 2);
				res = SetContPov(-1 , iInterface, 3);
				res = SetContPov(-1 , iInterface, 4);
			};
		};

		// If Discrete POV hat switches installed - make them go round
		// From time to time - put the switches in neutral state
		if (DiscPovNumber)
		{
			if (count < 550)
			{
				SetDiscPov(((count/20) + 0)%4, iInterface, 1);
				SetDiscPov(((count/20) + 1)%4, iInterface, 2);
				SetDiscPov(((count/20) + 2)%4, iInterface, 3);
				SetDiscPov(((count/20) + 3)%4, iInterface, 4);
			}
			else
			{
				SetDiscPov(-1, iInterface, 1);
				SetDiscPov(-1, iInterface, 2);
				SetDiscPov(-1, iInterface, 3);
				SetDiscPov(-1, iInterface, 4);
			};
		};

		Sleep(20);
		X+=150;
		Y+=250;
		Z+=350;
		ZR-=200;
		count++;
		if (count > 640) count=0;
	} // While
#endif

#ifdef EFFICIENT
	// Start feeding in an endless loop
	while (1)
	{

		/*** Create the data packet that holds the entire position info ***/
		id = (BYTE)iInterface;
		iReport.bDevice = id;

		iReport.wAxisX=X;
		iReport.wAxisY=Y;
		iReport.wAxisZ=Z;
		iReport.wAxisZRot=ZR;
		iReport.wAxisXRot=XR;

		// Set buttons one by one
		iReport.lButtons = 1<<count/20;

		if (ContPovNumber)
		{
			// Make Continuous POV Hat spin
			iReport.bHats		= (DWORD)(count*70);
			iReport.bHatsEx1	= (DWORD)(count*70)+3000;
			iReport.bHatsEx2	= (DWORD)(count*70)+5000;
			iReport.bHatsEx3	= 15000 - (DWORD)(count*70);
			if ((count*70) > 36000)
			{
				iReport.bHats = -1; // Neutral state
				iReport.bHatsEx1 = -1; // Neutral state
				iReport.bHatsEx2 = -1; // Neutral state
				iReport.bHatsEx3 = -1; // Neutral state
			};
		}
		else
		{
			// Make 5-position POV Hat spin
			unsigned char pov[4];
			pov[0] = ((count/20) + 0)%4;
			pov[1] = ((count/20) + 1)%4;
			pov[2] = ((count/20) + 2)%4;
			pov[3] = ((count/20) + 3)%4;

			iReport.bHats		= (pov[3]<<12) | (pov[2]<<8) | (pov[1]<<4) | pov[0];
			if ((count) > 550)
				iReport.bHats = -1; // Neutral state
		};

		/*** Feed the driver with the position packet - is fails then wait for input then try to re-acquire device ***/
		if (!UpdateVJD(iInterface, (PVOID)&iReport))
		{
			_tprintf("Feeding vJoy device number %d failed - try to enable device then press enter\n", iInterface);
			getchar();
			AcquireVJD(iInterface);
			ContinuousPOV = (BOOL)GetVJDContPovNumber(iInterface);
		}

				Sleep(20);
		count++;
		if (count > 640) count=0;

		X+=150;
		Y+=250;
		Z+=350;
		ZR-=200;

	};
#endif

	_tprintf("OK\n");

	return 0;
}
