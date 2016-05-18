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

#ifndef __STORAGEOPENMODE_H_
#define __STORAGEOPENMODE_H_
#pragma once

#pragma warning(push, 4)					// Enable maximum compiler warnings

using namespace System::IO;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageOpenMode Enumeration
//
// The StorageOpenMode enumeration defines a subset of the System::IO::FileMode
// enumeration as they pertain to this library
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC enum struct StorageOpenMode
{
	Create			= FileMode::Create,			// Create w/overwrite
	CreateNew		= FileMode::CreateNew,		// Create fail if exists
	Open			= FileMode::Open,			// Open existing
	OpenOrCreate	= FileMode::OpenOrCreate,	// Open or create w/overwrite
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif		// __STORAGEOPENMODE_H_
