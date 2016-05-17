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

#ifndef __STDAFX_H_
#define __STDAFX_H_
#pragma once

//---------------------------------------------------------------------------
// BEGIN_NAMESPACE / END_NAMESPACE
//
// Used to define namespaces, since C++/CLI still doesn't let you combine them
// into one statement

#define BEGIN_NAMESPACE(__x) namespace __x {
#define END_NAMESPACE(__x) }

//---------------------------------------------------------------------------
// CHECK_DISPOSED
//
// Used throughout to make object disposed exceptions easier to read and not
// require hard-coding a class name into the statement.  This will throw the
// function name, but that's actually better in my opinion

#define CHECK_DISPOSED(__flag) \
	if(__flag) throw gcnew ObjectDisposedException(gcnew String(__FUNCTION__));

//---------------------------------------------------------------------------
// Win32 Declarations

#define	_WIN32_WINNT		0x0500			// Windows 2000
#define	_WIN32_IE			0x0500			// IE 5.0 / ShellAPI 5.0

#include <windows.h>				// Include main Windows declarations
#include <vcclr.h>					// Include VC CLR extensions

#pragma warning(disable:4091)
#include <msclr\lock.h>				// Include lock class declarations
#pragma warning(default:4091)

//---------------------------------------------------------------------------
// Pinned Pointer Types

typedef pin_ptr<const wchar_t>		PinnedStringPtr;
typedef pin_ptr<unsigned __int8>	PinnedBytePtr;

//---------------------------------------------------------------------------
// STRUCTURED_STORAGE_PUBLIC
//
// Used in all public class declarations to allow other projects to
// embed the code without making it public

#define STRUCTURED_STORAGE_PUBLIC	public

//---------------------------------------------------------------------------

#endif	// __STDAFX_H_
