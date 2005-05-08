// Copyright (c) 1998 Mark Russinovich
// Systems Internals
// http://www.sysinternals.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define NTOS_MODE_USER
#include <ntos.h>
#include <fmifs.h>
#include <tchar.h>

// Globals
BOOL	Error = FALSE;

// switches
BOOL	QuickFormat = FALSE;
DWORD   ClusterSize = 0;
BOOL	CompressDrive = FALSE;
BOOL    GotALabel = FALSE;
LPTSTR  Label = _T("");
LPTSTR  Drive = NULL;
LPTSTR  Format = _T("FAT");

TCHAR  RootDirectory[MAX_PATH];
TCHAR  LabelString[12];

//
// Size array
//
typedef struct {
	TCHAR  SizeString[16];
	DWORD  ClusterSize;
} SIZEDEFINITION, *PSIZEDEFINITION;

SIZEDEFINITION LegalSizes[] = {
	{ _T("512"), 512 },
	{ _T("1024"), 1024 },
	{ _T("2048"), 2048 },
	{ _T("4096"), 4096 },
	{ _T("8192"), 8192 },
	{ _T("16K"), 16384 },
	{ _T("32K"), 32768 },
	{ _T("64K"), 65536 },
	{ _T("128K"), 65536 * 2 },
	{ _T("256K"), 65536 * 4 },
	{ _T(""), 0 },
};


//----------------------------------------------------------------------
//
// PrintWin32Error
//
// Takes the win32 error code and prints the text version.
//
//----------------------------------------------------------------------
static VOID PrintWin32Error( LPTSTR Message, DWORD ErrorCode )
{
	LPTSTR   lpMsgBuf;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, ErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&lpMsgBuf, 0, NULL );

	_tprintf(_T("%S: %S\n"), Message, lpMsgBuf );
	LocalFree( lpMsgBuf );
}


//----------------------------------------------------------------------
//
// Usage
//
// Tell the user how to use the program
//
//----------------------------------------------------------------------
static VOID Usage( LPTSTR ProgramName )
{
	_tprintf(_T("Usage: %S drive: [-FS:file-system] [-V:label] [-Q] [-A:size] [-C]\n\n"), ProgramName);
	_tprintf(_T("  [drive:]         Specifies the drive to format.\n"));
	_tprintf(_T("  -FS:file-system  Specifies the type of file system (e.g. FAT).\n"));
	_tprintf(_T("  -V:label         Specifies volume label.\n"));
	_tprintf(_T("  -Q               Performs a quick format.\n"));
	_tprintf(_T("  -A:size          Overrides the default allocation unit size. Default settings\n"));
	_tprintf(_T("                   are strongly recommended for general use\n"));
	_tprintf(_T("                   NTFS supports 512, 1024, 2048, 4096, 8192, 16K, 32K, 64K.\n"));
	_tprintf(_T("                   FAT supports 8192, 16K, 32K, 64K, 128K, 256K.\n"));
	_tprintf(_T("                   NTFS compression is not supported for allocation unit sizes\n"));
	_tprintf(_T("                   above 4096.\n"));
	_tprintf(_T("  -C               Files created on the new volume will be compressed by\n"));
	_tprintf(_T("                   default.\n"));
	_tprintf(_T("\n"));
}


//----------------------------------------------------------------------
//
// ParseCommandLine
//
// Get the switches.
//
//----------------------------------------------------------------------
static int ParseCommandLine( int argc, TCHAR *argv[] )
{
	int i, j;
	BOOLEAN gotFormat = FALSE;
	BOOLEAN gotQuick = FALSE;
	BOOLEAN gotSize = FALSE;
	BOOLEAN gotLabel = FALSE;
	BOOLEAN gotCompressed = FALSE;


	for( i = 1; i < argc; i++ ) {

		switch( argv[i][0] ) {

		case '-':
		case '/':

			if( !_tcsnicmp( &argv[i][1], _T("FS:"), 3 )) {

				if( gotFormat) return -1;
				Format = &argv[i][4];
				gotFormat = TRUE;


			} else if( !_tcsnicmp( &argv[i][1], _T("A:"), 2 )) {

				if( gotSize ) return -1;
				j = 0;
				while( LegalSizes[j].ClusterSize &&
					 _tcsicmp( LegalSizes[j].SizeString, &argv[i][3] )) j++;

				if( !LegalSizes[j].ClusterSize ) return i;
				ClusterSize = LegalSizes[j].ClusterSize;
				gotSize = TRUE;

			} else if( ! _tcsnicmp( &argv[i][1], _T("V:"), 2 )) {

				if( gotLabel ) return -1;
				Label = &argv[i][3];
				gotLabel = TRUE;
				GotALabel = TRUE;

			} else if( !_tcsicmp( &argv[i][1], _T("Q") )) {

				if( gotQuick ) return -1;
				QuickFormat = TRUE;
				gotQuick = TRUE;

			} else if( !_tcsicmp( &argv[i][1], _T("C") )) {

				if( gotCompressed ) return -1;
				CompressDrive = TRUE;
				gotCompressed = TRUE;

			} else return i;
			break;

		default:

			if( Drive ) return i;
			if( argv[i][1] != _T(':') ) return i;

			Drive = argv[i];
			break;
		}
	}
	return 0;
}

//----------------------------------------------------------------------
//
// FormatExCallback
//
// The file system library will call us back with commands that we
// can interpret. If we wanted to halt the chkdsk we could return FALSE.
//
//----------------------------------------------------------------------
BOOLEAN STDCALL
FormatExCallback (CALLBACKCOMMAND Command,
		  ULONG Modifier,
		  PVOID Argument)
{
	PDWORD percent;
	PTEXTOUTPUT output;
	PBOOLEAN status;

	//
	// We get other types of commands, but we don't have to pay attention to them
	//
	switch( Command ) {

	case PROGRESS:
		percent = (PDWORD) Argument;
		_tprintf(_T("%lu percent completed.\r"), *percent);
		break;

	case OUTPUT:
		output = (PTEXTOUTPUT) Argument;
		fprintf(stdout, "%s", output->Output);
		break;

	case DONE:
		status = (PBOOLEAN) Argument;
		if( *status == FALSE ) {

			_tprintf(_T("FormatEx was unable to complete successfully.\n\n"));
			Error = TRUE;
		}
		break;
	case DONEWITHSTRUCTURE:
	case UNKNOWN2:
	case UNKNOWN3:
	case UNKNOWN4:
	case UNKNOWN5:
	case INSUFFICIENTRIGHTS:
	case UNKNOWN7:
	case UNKNOWN8:
	case UNKNOWN9:
	case UNKNOWNA:
	case UNKNOWNC:
	case UNKNOWND:
	case STRUCTUREPROGRESS:
		_tprintf(_T("Operation Not Supported"));
		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------------------------
//
// LoadFMIFSEntryPoints
//
// Loads FMIFS.DLL and locates the entry point(s) we are going to use
//
//----------------------------------------------------------------------
BOOLEAN LoadFMIFSEntryPoints()
{
	HMODULE hFmifs = LoadLibrary( _T("fmifs.dll") );
	if( !(void*) GetProcAddress( hFmifs, "FormatEx" ) ) {

		return FALSE;
	}

	if( !((void *) GetProcAddress( hFmifs,
			"EnableVolumeCompression" )) ) {

		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------
//
// WMain
//
// Engine. Just get command line switches and fire off a format. This
// could also be done in a GUI like Explorer does when you select a
// drive and run a check on it.
//
// We do this in UNICODE because the chkdsk command expects PWCHAR
// arguments.
//
//----------------------------------------------------------------------
int
_tmain(int argc, TCHAR *argv[])
{
	int badArg;
	DWORD media = FMIFS_HARDDISK;
	DWORD driveType;
	TCHAR fileSystem[1024];
	TCHAR volumeName[1024];
	TCHAR input[1024];
	DWORD serialNumber;
	DWORD flags, maxComponent;
	ULARGE_INTEGER freeBytesAvailableToCaller, totalNumberOfBytes, totalNumberOfFreeBytes;
#ifndef UNICODE
	WCHAR  RootDirectoryW[MAX_PATH], FormatW[MAX_PATH], LabelW[MAX_PATH];
#endif

	//
	// Get function pointers
	//
	if( !LoadFMIFSEntryPoints()) {

		_tprintf(_T("Could not located FMIFS entry points.\n\n"));
		return -1;
	}

	//
	// Parse command line
	//
	if( (badArg = ParseCommandLine( argc, argv ))) {

		_tprintf(_T("Unknown argument: %S\n"), argv[badArg] );

		Usage(argv[0]);
		return -1;
	}

	//
	// Get the drive's format
	//
	if( !Drive ) {

		_tprintf(_T("Required drive parameter is missing.\n\n"));
		Usage( argv[0] );
		return -1;

	} else {

		_tcscpy( RootDirectory, Drive );
	}
	RootDirectory[2] = _T('\\');
	RootDirectory[3] = _T('\0');

	//
	// See if the drive is removable or not
	//
	driveType = GetDriveType( RootDirectory );

	if( driveType == 0 ) {
		PrintWin32Error( _T("Could not get drive type"), GetLastError());
		return -1;
	}

	if( driveType != DRIVE_FIXED ) {
		_tprintf(_T("Insert a new floppy in drive %C:\nand press Enter when ready..."),
			RootDirectory[0] );
		_fgetts( input, sizeof(input)/2, stdin );

		media = FMIFS_FLOPPY;
	}

	//
	// Determine the drive's file system format
	//
	if( !GetVolumeInformation( RootDirectory,
						volumeName, sizeof(volumeName)/2,
						&serialNumber, &maxComponent, &flags,
						fileSystem, sizeof(fileSystem)/2)) {

		PrintWin32Error( _T("Could not query volume"), GetLastError());
		return -1;
	}

	if( !GetDiskFreeSpaceEx( RootDirectory,
			&freeBytesAvailableToCaller,
			&totalNumberOfBytes,
			&totalNumberOfFreeBytes )) {

		PrintWin32Error( _T("Could not query volume size"), GetLastError());
		return -1;
	}
	_tprintf(_T("The type of the file system is %S.\n"), fileSystem );

	//
	// Make sure they want to do this
	//
	if( driveType == DRIVE_FIXED ) {

		if( volumeName[0] ) {

			while(1 ) {

				_tprintf(_T("Enter current volume label for drive %C: "), RootDirectory[0] );
				_fgetts( input, sizeof(input)/2, stdin );
				input[ _tcslen( input ) - 1] = 0;

				if( !_tcsicmp( input, volumeName )) {

					break;
				}
				_tprintf(_T("An incorrect volume label was entered for this drive.\n"));
			}
		}

		while( 1 ) {

			_tprintf(_T("\nWARNING, ALL DATA ON NON_REMOVABLE DISK\n"));
			_tprintf(_T("DRIVE %C: WILL BE LOST!\n"), RootDirectory[0] );
			_tprintf(_T("Proceed with Format (Y/N)? " ));
			_fgetts( input, sizeof(input)/2, stdin );

			if( input[0] == _T('Y') || input[0] == _T('y') ) break;

			if(	input[0] == _T('N') || input[0] == _T('n') ) {

				_tprintf(_T("\n"));
				return 0;
			}
		}
		media = FMIFS_HARDDISK;
	}

	//
	// Tell the user we're doing a long format if appropriate
	//
	if( !QuickFormat ) {

		if( totalNumberOfBytes.QuadPart > 1024*1024*10 ) {

			_tprintf(_T("Verifying %luM\n"), (DWORD) (totalNumberOfBytes.QuadPart/(1024*1024)));

		} else {

			_tprintf(_T("Verifying %.1fM\n"),
				((float)(LONGLONG)totalNumberOfBytes.QuadPart)/(float)(1024.0*1024.0));
		}
	} else  {

		if( totalNumberOfBytes.QuadPart > 1024*1024*10 ) {

			_tprintf(_T("QuickFormatting %luM\n"), (DWORD) (totalNumberOfBytes.QuadPart/(1024*1024)));

		} else {

			_tprintf(_T("QuickFormatting %.2fM\n"),
				((float)(LONGLONG)totalNumberOfBytes.QuadPart)/(float)(1024.0*1024.0));
		}
		_tprintf(_T("Creating file system structures.\n"));
	}

	//
	// Format away!
	//
#ifndef UNICODE
	MultiByteToWideChar(CP_ACP, 0, RootDirectory, -1, RootDirectoryW, MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, Format, -1, FormatW, MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, Label, -1, LabelW, MAX_PATH);
	FormatEx( RootDirectoryW, media, FormatW, LabelW, QuickFormat,
			ClusterSize, FormatExCallback );
#else
	FormatEx( RootDirectory, media, Format, Label, QuickFormat,
			ClusterSize, FormatExCallback );
#endif
	if( Error ) return -1;
	_tprintf(_T("Format complete.\n"));

	//
	// Enable compression if desired
	//
	if( CompressDrive ) {

#ifndef UNICODE
		MultiByteToWideChar(CP_ACP, 0, RootDirectory, -1, RootDirectoryW, MAX_PATH);
		if( !EnableVolumeCompression( RootDirectoryW, TRUE )) {
#else
		if( !EnableVolumeCompression( RootDirectory, TRUE )) {
#endif

			_tprintf(_T("Volume does not support compression.\n"));
		}
	}

	//
	// Get the label if we don't have it
	//
	if( !GotALabel ) {

		_tprintf(_T("Volume Label (11 characters, Enter for none)? " ));
		_fgetts( input, sizeof(LabelString)/2, stdin );

		input[ _tcslen(input)-1] = 0;
		if( !SetVolumeLabel( RootDirectory, input )) {

			PrintWin32Error(_T("Could not label volume"), GetLastError());
			return -1;
		}
	}

	if( !GetVolumeInformation( RootDirectory,
						volumeName, sizeof(volumeName)/2,
						&serialNumber, &maxComponent, &flags,
						fileSystem, sizeof(fileSystem)/2)) {

		PrintWin32Error( _T("Could not query volume"), GetLastError());
		return -1;
	}

	//
	// Print out some stuff including the formatted size
	//
	if( !GetDiskFreeSpaceEx( RootDirectory,
			&freeBytesAvailableToCaller,
			&totalNumberOfBytes,
			&totalNumberOfFreeBytes )) {

		PrintWin32Error( _T("Could not query volume size"), GetLastError());
		return -1;
	}

	_tprintf(_T("\n%I64d bytes total disk space.\n"), totalNumberOfBytes.QuadPart );
	_tprintf(_T("%I64d bytes available on disk.\n"), totalNumberOfFreeBytes.QuadPart );

	//
	// Get the drive's serial number
	//
	if( !GetVolumeInformation( RootDirectory,
						volumeName, sizeof(volumeName)/2,
						&serialNumber, &maxComponent, &flags,
						fileSystem, sizeof(fileSystem)/2)) {

		PrintWin32Error( _T("Could not query volume"), GetLastError());
		return -1;
	}
	_tprintf(_T("\nVolume Serial Number is %04X-%04X\n"), (unsigned int)(serialNumber >> 16),
					(unsigned int)(serialNumber & 0xFFFF) );

	return 0;
}

