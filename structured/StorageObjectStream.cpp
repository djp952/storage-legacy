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

#include "stdafx.h"						// Include project pre-compiled headers
#include "StorageObjectStream.h"		// Include StorageObjectStream declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings
#pragma warning(disable:4100)			// "unreferenced formal parameter"

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// StorageObjectStream Constructor (internal)
//
// Arguments:
//
//	stream			- Existing ComStream instance
//	mode			- ObjectStream mode (reader/writer)

StorageObjectStream::StorageObjectStream(ComStream^ stream, 
	StorageObjectStreamMode mode) : m_mode(mode)
{
	HRESULT					hResult;		// Result from function call

	if(stream == nullptr) throw gcnew ArgumentNullException();

	hResult = stream->CreateClone(m_stream);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageObjectStream::CanRead::get
//
// Determines if this stream supports READ operations

bool StorageObjectStream::CanRead::get(void)
{
	if(m_disposed) return false;
	return (m_mode == StorageObjectStreamMode::Reader);
}

//---------------------------------------------------------------------------
// StorageObjectStream::CanSeek::get
//
// Determines if this stream supports SEEK operations.  We do, but we don't
// support a known LENGTH

bool StorageObjectStream::CanSeek::get(void)
{
	return !m_disposed;
}

//---------------------------------------------------------------------------
// StorageObjectStream::CanWrite::get
//
// Determines if this stream supports WRITE operations

bool StorageObjectStream::CanWrite::get(void)
{
	if(m_disposed) return false;
	return (m_mode == StorageObjectStreamMode::Reader);
}

//---------------------------------------------------------------------------
// StorageObjectStream::Flush
//
// Flushes any changes to the stream ... or not in our case
//
// Arguments:
//
//	NONE

void StorageObjectStream::Flush(void)
{
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed);
	
	// This doesn't really do a whole lot for compound files that aren't
	// in transactional mode, but we can implement it, so we should

	hResult = m_stream->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageObjectStream::Length::get
//
// Determines the length of the stream by seeking to the end and returning
// that position back to the caller.

__int64 StorageObjectStream::Length::get(void)
{
	__int64				current;		// The current stream position

	CHECK_DISPOSED(m_disposed);

	current = Position;					// Get the current position
	
	// Attempt to seek to the end and return that value.  Ensure that
	// we put the stream pointer back where it started before leaving

	try { return Seek(0, SeekOrigin::End); }
	finally { Seek(current, SeekOrigin::Begin); }
}

//---------------------------------------------------------------------------
// StorageObjectStream::Read
//
// Reads bytes from the stream at the current position
//
// Arguments:
//
//	buffer		- Buffer to copy the data into
//	offset		- Offset into the buffer to start writing the data
//	count		- Maximum number of bytes to write into the buffer

int StorageObjectStream::Read(array<Byte>^ buffer, int offset, int count)
{
	ULONG				cbBytesToRead;				// Number of bytes to be read
	PinnedBytePtr		pinBuffer;					// Pinned buffer pointer
	ULONG				cbRead;						// Number of bytes read
	HRESULT				hResult;					// Result from function call

	CHECK_DISPOSED(m_disposed);

	if(buffer == nullptr) throw gcnew ArgumentNullException();
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("Offset cannot be a negative value");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("Count cannot be a negative value");
	if(m_mode != StorageObjectStreamMode::Reader) throw gcnew InvalidOperationException();

	cbBytesToRead = Math::Min(count, buffer->Length - offset);	// Calculate size
	pinBuffer = &buffer[offset];								// Pin the byte array

	// Attempt to load the data directly into the user supplied Byte[] array

	hResult = m_stream->Read(pinBuffer, cbBytesToRead, &cbRead);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return cbRead;						// Return number of bytes actually read
}

//---------------------------------------------------------------------------
// StorageObjectStream::Seek
//
// Seeks to a specific position in the underlying stream
//
// Arguments:
//
//	offset		- New offset to see to, based on origin
//	origin		- The origin of the seek operation

__int64 StorageObjectStream::Seek(__int64 offset, SeekOrigin origin)
{
	LARGE_INTEGER			liOffset;			// Offset as a LARGE_INTEGER
	ULARGE_INTEGER			uliPosition;		// The new position
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed);

	liOffset.QuadPart = offset;					// Convert into a LARGE_INTEGER

	hResult = m_stream->Seek(liOffset, static_cast<DWORD>(origin), &uliPosition);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return uliPosition.QuadPart;				// Return the new position
}

//---------------------------------------------------------------------------
// StorageObjectStream::SetLength
//
// Sets the length of the underlying stream
//
// Arguments:
//
//	value		- The new length of the stream

void StorageObjectStream::SetLength(__int64 value)
{
	ULARGE_INTEGER			uliNewSize;			// New size as ULARGE_INTEGER
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed);

	if(m_mode != StorageObjectStreamMode::Writer) throw gcnew NotSupportedException();

	uliNewSize.QuadPart = value;
	hResult = m_stream->SetSize(uliNewSize);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageObjectStream::Write
//
// Writes data into the stream at the current position if were writing a 
// stream class that wasn't read-only, that is
//
// Arguments:
//
//	buffer		- Buffer containing the bytes to be written
//	offset		- Offset into the buffer
//	count		- Maximum number of bytes to write into the stream

void StorageObjectStream::Write(array<Byte>^ buffer, int offset, int count)
{
	ULONG					cbBytesToWrite;		// Number of bytes to be written
	PinnedBytePtr			pinBuffer;			// Pinned buffer pointer
	ULONG					cbWritten;			// Number of bytes written
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed);

	if(buffer == nullptr) throw gcnew ArgumentNullException();
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("Offset cannot be a negative value");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("Count cannot be a negative value");
	if(m_mode != StorageObjectStreamMode::Writer) throw gcnew NotSupportedException();

	cbBytesToWrite = Math::Min(count, buffer->Length - offset);	// Calculate size
	pinBuffer = &buffer[offset];								// Pin the byte array

	// Attempt to load the data directly from the user buffer into the stream

	hResult = m_stream->Write(pinBuffer, cbBytesToWrite, &cbWritten);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)
