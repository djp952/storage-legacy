//-----------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef __VIRTUALDISKATTACHPARAMETERS_H_
#define __VIRTUALDISKATTACHPARAMETERS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class VirtualDiskAttachParameters
//
// Contains virtual hard disk (VHD) attach parameters
//---------------------------------------------------------------------------

public ref class VirtualDiskAttachParameters sealed
{
public:

	// Instance Constructor
	VirtualDiskAttachParameters() { Construct(); }

	//-----------------------------------------------------------------------
	// Properties

	// AutoDetach
	//
	// Flag indicating that the disk should be automatically detached when closed
	property bool AutoDetach
	{
		bool get(void) { return m_autoDetach; }
		void set(bool value) { m_autoDetach = value; }
	}

	// DoNotAssignDriveLetter
	//
	// Flag indicating that a drive letter should not be assigned during attach
	property bool DoNotAssignDriveLetter
	{
		bool get(void) { return m_noDriveLetter; }
		void set(bool value) { m_noDriveLetter = value; }
	}

	// ReadOnly
	//
	// Flag indicating if the backing store will be opened in read-only mode
	property bool ReadOnly
	{
		bool get(void) { return m_readOnly; }
		void set(bool value) { m_readOnly = value; }
	}

internal:

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// Releases strcuture allocated by ToATTACH_VIRTUAL_DISK_PARAMETERS()
	static void FreeATTACH_VIRTUAL_DISK_PARAMETERS(PATTACH_VIRTUAL_DISK_PARAMETERS pParams);

	// Gets the ATTACH_VIRTUAL_DISK_FLAG bitmask
	ATTACH_VIRTUAL_DISK_FLAG ToATTACH_VIRTUAL_DISK_FLAG(void);

	// Converts the state of this object into an unmanaged ATTACH_VIRTUAL_DISK_PARAMETERS
	ATTACH_VIRTUAL_DISK_VERSION ToATTACH_VIRTUAL_DISK_PARAMETERS(PATTACH_VIRTUAL_DISK_PARAMETERS* ppParams);

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	// Pseudo-constructor
	void Construct(void);

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_readOnly;
	bool					m_noDriveLetter;
	bool					m_autoDetach;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKATTACHPARAMETERS_H_
