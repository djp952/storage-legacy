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

#ifndef __STORAGEOBJECTSTREAM_H_
#define __STORAGEOBJECTSTREAM_H_
#pragma once

#include "ComStream.h"					// Include ComStream declarations
#include "StorageObjectStreamMode.h"	// Include stream mode enumeration
#include "StorageException.h"			// Include StorageException declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class StorageObjectStream
//
// StorageObjectStream wraps an existing IStream pointer from a storage object
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StorageObjectStream abstract : public Stream
{
public:

	//-----------------------------------------------------------------------
	// Stream Overrides

	//
	// NOTE: Do not override .Close() ... just implement IDisposable property
	//

	virtual void	Flush() override;
	virtual	int		Read(array<Byte>^ buffer, int offset, int count) override;
	virtual __int64	Seek(__int64 offset, SeekOrigin origin) override;
	virtual void	SetLength(__int64 value) override;
	virtual void	Write(array<Byte>^ buffer, int offset, int count) override;

	virtual property bool		CanRead { bool get(void) override; }
	virtual property bool		CanSeek { bool get(void) override; }
	virtual property bool		CanWrite { bool get(void) override; }
	virtual property __int64	Length { __int64 get(void) override; }

	virtual property __int64 Position 
	{ 
		__int64 get(void) override { return Seek(0, SeekOrigin::Current); } 
		void set(__int64 pos) override { Seek(pos, SeekOrigin::Begin); }
	}

internal:

	// INTERNAL CONSTRUCTOR
	StorageObjectStream(ComStream^ stream, StorageObjectStreamMode mode);

	//-----------------------------------------------------------------------
	// Internal Properties

	property bool IsDisposed { bool get(void) { return m_disposed; } } 

private:

	// DESTRUCTOR / FINALIZER
	~StorageObjectStream() { m_stream = nullptr; m_disposed = true; }
	//!StorageObjectStream();

	//-----------------------------------------------------------------------
	// Member Variables

	bool							m_disposed;		// Object disposal flag
	StorageObjectStreamMode			m_mode;			// Object stream mode
	ComStream^						m_stream;		// Parent stream instance
	//IStream*						m_pStream;		// Contained COM stream
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGEOBJECTSTREAM_H_
