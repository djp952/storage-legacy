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
#include "StructuredStorage.h"			// Include StructuredStorage declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StructuredStorage Constructor (private)
//
// Arguments:
//
//	fileName		- File name for the structured storage file
//	storage			- Root ComStorage object from Open()

StructuredStorage::StructuredStorage(String^ fileName, ComStorage^ storage) : 
	StorageContainer(nullptr, nullptr, storage), m_fileName(fileName), m_storage(storage)
{
	// NOTE: If anything goes wrong in this constructor, Open() will automatically
	// dispose of the COM pointer, so there is no need to self-dispose on error

	if(m_fileName == nullptr) throw gcnew ArgumentNullException();
	if(m_storage == nullptr) throw gcnew ArgumentNullException();

	m_pstgCache = gcnew ComCache<ComPropertyStorage^>();	// Create the cache
	m_stgCache = gcnew ComCache<ComStorage^>();				// Create the cache
	m_stmCache = gcnew ComCache<ComStream^>();				// Create the cache

	m_summaryInfo = gcnew StorageSummaryInformation(m_storage);
}

//---------------------------------------------------------------------------
// StructuredStorage Destructor
//
// Arguments:
//
//	NONE

StructuredStorage::~StructuredStorage()
{
	if(m_pstgCache != nullptr) delete m_pstgCache;	// Dispose of ComCache
	if(m_stgCache != nullptr) delete m_stgCache;	// Dispose of ComCache
	if(m_stmCache != nullptr) delete m_stmCache;	// Dispose of ComCache

	if(m_summaryInfo != nullptr) m_summaryInfo->InternalDispose();
	m_summaryInfo = nullptr;

	delete m_storage;							// Dispose of pointer
	m_disposed = true;							// Object is now disposed
}

//---------------------------------------------------------------------------
// StructuredStorage::ComPropStorageCache::get (internal)
//
// Exposes the ComStorage cache for this instance

ComCache<ComPropertyStorage^>^ StructuredStorage::ComPropStorageCache::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_pstgCache;
}

//---------------------------------------------------------------------------
// StructuredStorage::ComStorageCache::get (internal)
//
// Exposes the ComStorage cache for this instance

ComCache<ComStorage^>^ StructuredStorage::ComStorageCache::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stgCache;
}

//---------------------------------------------------------------------------
// StructuredStorage::ComStreamCache::get (internal)
//
// Exposes the ComStream cache for this instance

ComCache<ComStream^>^ StructuredStorage::ComStreamCache::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stmCache;
}

//---------------------------------------------------------------------------
// StructuredStorage::FileName::get
//
// Returns the file name used to create/access the structured storage

String^ StructuredStorage::FileName::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_fileName;
}

//---------------------------------------------------------------------------
// StructuredStorage::Flush
//
// Ensures that all data has been flushed to disk to prevent corruption
// and other such bad things
//
// Arguments:
//
//	NONE

void StructuredStorage::Flush(void)
{
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed);

	hResult = m_storage->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StructuredStorage::Open (static)
//
// Attempts to open/create a structured storage compound file and wrap a new
// StructuredStorage instance around it for the caller
//
// Arguments:
//
//	path		- Path to the structured storage file, or NULL for temp
//	mode		- Storage creation mode
//	access		- Storage access mode

StructuredStorage^ StructuredStorage::Open(String^ path, StorageOpenMode mode, 
	StorageAccessMode access)
{
	PinnedStringPtr		pinPath;					// Pinned path string
	DWORD				flags;						// Structured Storage flags
	STGOPTIONS			stgOptions;					// Storage options
	IStorage*			pRootStorage = NULL;		// Pointer to root IStorage
	ComStorage^			rootStorage;				// Wrapped root storage
	HRESULT				hResult = E_UNEXPECTED;		// Result from function call

	flags = static_cast<DWORD>(access);				// Cast out the flags

	memset(&stgOptions, 0, sizeof(STGOPTIONS));		// Initialize to all NULLs
	stgOptions.usVersion = 1;						// Windows 2000 base = version 1
	stgOptions.ulSectorSize = 4096;					// Use version 1 / 4K sectors

	if(path != nullptr) {
		
		path = Path::GetFullPath(path);			// Get the full path to the file
		pinPath = PtrToStringChars(path);		// Pin it down in memory
	}

	// Convert a mode of OpenOrCreate into one of the other values, depending
	// on if the file exists, OR the caller is looking for a temporary file

	if(mode == StorageOpenMode::OpenOrCreate) {

		if(path == nullptr) mode = StorageOpenMode::Create;
		else mode = (File::Exists(path)) ? StorageOpenMode::Open : StorageOpenMode::Create;
	}

	// If the determined file mode is CREATE or CREATENEW, the only allowed
	// access flag is ReadWriteExclusive

	if((mode == StorageOpenMode::Create) || (mode == StorageOpenMode::CreateNew))
		if(access != StorageAccessMode::Exclusive) throw gcnew InvalidOperationException();

	try {

		// Show time.  Depending on the specified FileMode value, either open
		// an existing structured storage file, or try to create a new one

		switch(mode) {

			// StorageOpenMode::Open
			// - Cannot be used with a temporary file (path != NULL)
			// - Uses StgOpenStorage to open an existing compound file

			case StorageOpenMode::Open:

				if(path == nullptr) throw gcnew ArgumentNullException();
				hResult = StgOpenStorageEx(pinPath, flags, STGFMT_DOCFILE, 0, &stgOptions, 
					0, __uuidof(IStorage), reinterpret_cast<void**>(&pRootStorage));
				break;

			// StorageOpenMode::Create
			// - Adds the STGM_CREATE flag to allow for an overwrite condition
			// - Falls through to the CreateNew handler below

			case StorageOpenMode::Create: flags |= STGM_CREATE;

			// FileMode::CreateNew
			// - Temporary files will be auto-deleted on release
			// - Uses StgCreateDocfile to construct the new compound file

			case StorageOpenMode::CreateNew:
				
				if(path == nullptr) flags |= STGM_DELETEONRELEASE;
				hResult = StgCreateStorageEx(pinPath, flags, STGFMT_DOCFILE, 0, &stgOptions, 
					NULL, __uuidof(IStorage), reinterpret_cast<void**>(&pRootStorage));
				break;
			
			default: throw gcnew ArgumentOutOfRangeException();
		}

		// If we actually make it this far, we have an HRESULT to work with.
		// If either call failed, the root IStorage pointer will still be
		// NULL and does not need to be released

		if(FAILED(hResult)) throw gcnew StorageException(hResult, Path::GetFileName(path));

		// In the new ComXXXX model, create a ComStorage wrapper around the raw interface
		// pointer, and if we cannot construct the instance, dispose of it manually to
		// ensure that the underlying storage file doesn't hang open on the application

		rootStorage = gcnew ComStorage(pRootStorage);
		try { return gcnew StructuredStorage(path, rootStorage); }
		catch(Exception^) { delete rootStorage; throw; }
	}

	finally { if(pRootStorage) pRootStorage->Release(); }
}

//---------------------------------------------------------------------------
// StructuredStorage::SummaryInformation::get
//
// Retrieves a reference to the contained StorageSummaryInfo class

StorageSummaryInformation^ StructuredStorage::SummaryInformation::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_summaryInfo;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
