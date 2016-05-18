//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

#ifndef __STORAGEEXCEPTION_H_
#define __STORAGEEXCEPTION_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Text;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class StorageException
//
// The generic exception thrown by the StructuredStorage objects.  Wraps up
// the underlying COM error into something slightly more useful than just
// an HRESULT error code
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StorageException sealed : public Exception
{
internal:

	//-----------------------------------------------------------------------
	// Constructor
	//
	// Arguments:
	//
	//	hr			- HRESULT error code from Structured Storage API

	StorageException(HRESULT hr) : Exception(GenerateMessage(hr, nullptr))
	{
		Exception::HResult = hr;			// Set the HRESULT value
	}

	//-----------------------------------------------------------------------
	// Constructor
	//
	// Arguments:
	//
	//	hr			- HRESULT error code from Structured Storage API
	//	insert		- Single insert string for the error message

	StorageException(HRESULT hr, String^ insert) : 
		Exception(GenerateMessage(hr, insert))
	{
		Exception::HResult = hr;			// Set the HRESULT value
	}

private:
	
	//-----------------------------------------------------------------------
	// Private Member Functions

	static String^ GenerateMessage(HRESULT hr, String^ insert);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGEEXCEPTION_H_
