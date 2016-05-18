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
#include "ComStream.h"						// Include ComStream declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// ComStream Constructor
//
// Arguments:
//
//	pStorage	- IStorage pointer to be wrapped up

ComStream::ComStream(IStream* pStream) : m_pStream(pStream)
{
	if(!m_pStream) throw gcnew ArgumentNullException();

	m_clones = gcnew List<WeakReference^>();
	m_pStream->AddRef();
}

//---------------------------------------------------------------------------
// ComStream Constructor (private)
//
// Arguments:
//
//	pStorage	- IStorage pointer to be wrapped up
//	parent		- Reference to the parent ComStream instance

ComStream::ComStream(IStream* pStream, ComStream^ parent) : m_pStream(pStream),
	m_parent(parent)
{
	// This is the clone constructor.  There is absolutely zero chance that
	// I'm going to remember the semantics of this next week, so you'll have
	// to take it on faith that this is a good thing and that it allows us
	// to open completely independent streams against a single StorageObject.

	if(!m_pStream) throw gcnew ArgumentNullException();
	if(m_parent == nullptr) throw gcnew ArgumentNullException();
	
	m_pStream->AddRef();				// AddRef() our local pointer
}

//---------------------------------------------------------------------------
// ComStream Destructor

ComStream::~ComStream()
{
	ComStream^				clone;		// Cloned stream object instance

	if(m_disposed) return;				// Already been disposed of

	// If there is a cloned stream collection (meaning this is a parent
	// stream), dispose of all the clones before killing ourselves off

	if(m_clones != nullptr) {

		for each(WeakReference^ ref in m_clones) {

			if(!ref->IsAlive) continue;		// Already dead, keep moving

			// Yank out a strong reference to the cloned ComStream and
			// dispose of it as necessary

			clone = safe_cast<ComStream^>(ref->Target);
			if(!clone->IsDisposed()) delete clone;
		}

		m_clones->Clear();				// Remove all cached clones
	}
	
	this->!ComStream();					// Invoke the finalizer
	m_disposed = true;					// Object has been disposed of
}

//---------------------------------------------------------------------------
// ComStream Finalizer

ComStream::!ComStream()
{
	if(m_pStream) m_pStream->Release();
	m_pStream = NULL;
}

//---------------------------------------------------------------------------
// ComStream::Commit
//
// Ensures that any changes made to a stream object are persisted

HRESULT ComStream::Commit(DWORD grfCommitFlags)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Commit(grfCommitFlags);
}

//---------------------------------------------------------------------------
// ComStream::CopyTo
//
// Copies a specified number of bytes into another stream

HRESULT ComStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead,
	ULARGE_INTEGER* pcbWritten)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->CopyTo(pstm, cb, pcbRead, pcbWritten);
}

//---------------------------------------------------------------------------
// ComStream::CreateClone
//
// Creates a cloned instance of this parent stream
//
// Arguments:
//
//	clone		- On success, contains the cloned stream

HRESULT ComStream::CreateClone(ComStream^% clone)
{
	IStream*			pClone;				// Raw cloned interface pointer
	LARGE_INTEGER		liOffset;			// Offset as a LARGE_INTEGER
	HRESULT				hResult;			// Result from function call

	clone = nullptr;						// Initialize [out] reference

	CHECK_DISPOSED(m_disposed);
	if(m_parent != nullptr) throw gcnew InvalidOperationException();
	if(m_clones == nullptr) throw gcnew InvalidOperationException();

	// Attempt to physically clone the stream instance to get a new IStream
	// that has it's own seek pointer.  Also reset that seek pointer to zero.

	hResult = m_pStream->Clone(&pClone);
	if(FAILED(hResult)) return hResult;

	liOffset.QuadPart = 0;
	pClone->Seek(liOffset, STREAM_SEEK_SET, NULL);

	// Now attempt to create a cloned ComStream instance and add it into the
	// member collection for tracking purposes ... see destructor

	try { 
	
		clone = gcnew ComStream(pClone, this);
		m_clones->Add(gcnew WeakReference(clone, false)); 
	}

	finally { pClone->Release(); }			// Release local interface ptr
	
	return S_OK;							// Clone generation successful
}

//---------------------------------------------------------------------------
// ComStream::LockRegion
//
// Restricts access to a specified range of bytes in the stream

HRESULT ComStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->LockRegion(libOffset, cb, dwLockType);
}

//---------------------------------------------------------------------------
// ComStream::Read
//
// Reads a specified number of bytes from the stream 

HRESULT ComStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Read(pv, cb, pcbRead);
}

//---------------------------------------------------------------------------
// ComStream::Revert
//
// Discards all changes that have been made to a transacted stream 

HRESULT ComStream::Revert(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Revert();
}

//---------------------------------------------------------------------------
// ComStream::Seek
//
// Changes the seek pointer to a new location

HRESULT ComStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Seek(dlibMove, dwOrigin, plibNewPosition);
}

//---------------------------------------------------------------------------
// ComStream::SetSize
//
// Changes the size of the stream object

HRESULT ComStream::SetSize(ULARGE_INTEGER libNewSize)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->SetSize(libNewSize);
}

//---------------------------------------------------------------------------
// ComStream::Stat
//
// Retrieves the STATSTG structure for this stream

HRESULT ComStream::Stat(::STATSTG* pstatstg, DWORD grfStatFlag)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Stat(pstatstg, grfStatFlag);
}

//---------------------------------------------------------------------------
// ComStream::UnlockRegion
//
// Removes the access restriction on a range of bytes previously restricted

HRESULT ComStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->UnlockRegion(libOffset, cb, dwLockType);
}

//---------------------------------------------------------------------------
// ComStream::Write
//
// Writes a specified number of bytes into the stream object

HRESULT ComStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStream->Write(pv, cb, pcbWritten);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
