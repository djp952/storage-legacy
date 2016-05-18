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

#ifndef __STORAGEACCESSMODE_H_
#define __STORAGEACCESSMODE_H_
#pragma once

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageAccessMode Enumeration
//
// The StorageAccessMode enumeration defines the various options for access
// and sharing when opening a StructuredStorage file.
//
// For now, I'm only supporting DIRECT mode and very limited sharing.  At 
// some point in the future, this may change to allow transactional and/or
// DIRECT_SWMR support (after many changes to the code)
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC enum struct StorageAccessMode
{
	Exclusive			= STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
	ReadOnlyExclusive	= STGM_DIRECT | STGM_READ | STGM_SHARE_EXCLUSIVE,
	ReadOnlyShared		= STGM_DIRECT | STGM_READ | STGM_SHARE_DENY_WRITE,
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif		// __STORAGEACCESSMODE_H_
