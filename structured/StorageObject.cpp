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

#include "stdafx.h"							// Include project pre-compiled headers
#include "StorageObject.h"					// Include StorageObject declarations
#include "StructuredStorage.h"				// Include StructuredStorage decls
#include "StorageContainer.h"				// Include StorageContainer declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageObject Constructor (internal)
//
// Arguments:
//
//	root				- Reference to the parent Storage object
//	parent				- Parent ComStorage instance reference
//	stream				- Contained ComStream instance

StorageObject::StorageObject(StructuredStorage^ root, ComStorage^ parent, 
	ComStream^ stream) : m_root(root), m_parent(parent), m_stream(stream)
{
	if(m_root == nullptr) throw gcnew ArgumentNullException();
	if(m_parent == nullptr) throw gcnew ArgumentNullException();
	if(m_stream == nullptr) throw gcnew ArgumentNullException();

	// Get the read-only flag as well as the GUID associated with this
	// particular object stream ...

	m_readOnly = StorageUtil::IsStreamReadOnly(m_stream);
	m_objid = StorageUtil::GetObjectID(m_stream);
}

//---------------------------------------------------------------------------
// StorageObject::Data::get
//
// Returns the entire contents of the object stream as a byte array

array<Byte>^ StorageObject::Data::get(void)
{
	StorageObjectReader^		reader;			// Object stream reader
	__int64						length;			// Length of the stream
	array<Byte>^				data;			// The resultant array

	CHECK_DISPOSED(m_stream->IsDisposed());

	reader = GetReader();				// Acquire a new stream reader

	try {

		// If the object contains more data than we can define in a signed
		// 32-bit integer, it's WAY too big to turn into a byte array.  The
		// chances of this ever happening are absurdly minute, and bring into
		// question the sanity of the individual that tried to stick so much
		// stuff in there to begin with.  Crazy, crazy stuff.

		length = reader->Length;
		if(length > Int32::MaxValue) throw gcnew ObjectTooLargeException();

		// Allocate the array to return to the caller, and load it up

		data = gcnew array<Byte>(static_cast<int>(length));
		reader->Read(data, 0, static_cast<int>(length));
		
		return data;					// Hand the array back to the caller
	}

	finally { delete reader; }			// Always dispose of the reader
}

//---------------------------------------------------------------------------
// StorageObject::Data::set
//
// Replaces the contents of the objects stream with a byte array

void StorageObject::Data::set(array<Byte>^ value)
{
	StorageObjectWriter^		writer;			// Object stream writer

	CHECK_DISPOSED(m_stream->IsDisposed());

	if(m_readOnly) throw gcnew ObjectReadOnlyException();

	writer = GetWriter();				// Acquire a new stream writer

	try {

		// This isn't a very difficult operation.  Just pre-set the length
		// of the stream to shrink/grow as needed, and dump all the data
		// from the provided array in one shot ...

		writer->SetLength(value->Length);
		writer->Write(value, 0, value->Length);
	}

	finally { delete writer; }			// Always dispose of the writer
}

//---------------------------------------------------------------------------
// StorageObject::GetReader
//
// Creates and returns a new StorageObjectStreamReader against this stream
//
// Arguments:
//
//	NONE

StorageObjectReader^ StorageObject::GetReader(void)
{
	CHECK_DISPOSED(m_stream->IsDisposed());
	return gcnew StorageObjectReader(m_stream);
}

//---------------------------------------------------------------------------
// StorageObject::GetWriter
//
// Creates and returns a new StorageObjectStreamWriter against this stream
//
// Arguments:
//
//	NONE

StorageObjectWriter^ StorageObject::GetWriter(void)
{
	CHECK_DISPOSED(m_stream->IsDisposed());
	return gcnew StorageObjectWriter(m_stream);
}

//---------------------------------------------------------------------------
// StorageObject::Name::get
//
// Retrieves the current name of the object stream

String^ StorageObject::Name::get(void)
{
	CHECK_DISPOSED(m_stream->IsDisposed());
	return m_parent->ObjectNameMapper->MapGuidToName(m_objid);
}

//---------------------------------------------------------------------------
// StorageObject::Name::set
//
// Changes the name of the object stream

void StorageObject::Name::set(String^ value)
{
	CHECK_DISPOSED(m_stream->IsDisposed());

	if(value == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ObjectReadOnlyException();
	
	m_parent->ObjectNameMapper->RenameMapping(m_objid, value);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
