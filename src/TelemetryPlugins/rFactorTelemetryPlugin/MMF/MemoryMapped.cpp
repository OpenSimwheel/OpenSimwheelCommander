
// //////////////////////////////////////////////////////////
// MemoryMapped.cpp
// Copyright (c) 2013 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

#include "MemoryMapped.h"

#include <stdexcept>
#include <cstdio>


#include <windows.h>


/// do nothing, must use open()
MemoryMapped::MemoryMapped()
: _filename   (),
  _filesize   (0),
  _hint       (Normal),
  _mappedBytes(0),
  _file       (0),
  _mappedFile (NULL),
  _mappedView (NULL)
{
}


/// open file, mappedBytes = 0 maps the whole file
MemoryMapped::MemoryMapped(const std::string& filename, size_t mappedBytes, CacheHint hint)
: _filename   (filename),
  _filesize   (0),
  _hint       (hint),
  _mappedBytes(mappedBytes),
  _file       (0),
  _mappedFile (NULL),
  _mappedView (NULL)
{
  open(filename, mappedBytes, hint);
}


/// close file (see close() )
MemoryMapped::~MemoryMapped()
{
  close();
}


/// open file
bool MemoryMapped::open(const std::string& filename, size_t mappedBytes, CacheHint hint)
{
  // already open ?
  if (isValid())
    return false;

  _file       = 0;
  _filesize   = 0;
  _hint       = hint;
  _mappedFile = NULL;
  _mappedView = NULL;



  DWORD winHint = 0;
  switch (_hint)
  {
  case Normal:         winHint = FILE_ATTRIBUTE_NORMAL;     break;
  case SequentialScan: winHint = FILE_FLAG_SEQUENTIAL_SCAN; break;
  case RandomAccess:   winHint = FILE_FLAG_RANDOM_ACCESS;   break;
  default: break;
  }

  // open file
  _file = ::CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, winHint, NULL);
  if (!_file)
    return false;

  // file size
  LARGE_INTEGER result;
  if (!GetFileSizeEx(_file, &result))
    return false;
  _filesize = static_cast<uint64_t>(result.QuadPart);

  // convert to mapped mode
  _mappedFile = ::CreateFileMapping(_file, NULL, PAGE_READONLY, 0, 0, NULL);
  if (!_mappedFile)
    return false;

  // initial mapping
  remap(0, mappedBytes);

  if (!_mappedView)
    return false;

  // everything's fine
  return true;
}


/// close file
void MemoryMapped::close()
{
  _filesize = 0;

  // kill pointer
  if (_mappedView)
  {

    ::UnmapViewOfFile(_mappedView);

    _mappedView = NULL;
  }


  if (_mappedFile)
  {
    ::CloseHandle(_mappedFile);
    _mappedFile = NULL;
  }


  // close underlying file
  if (_file)
  {

    ::CloseHandle(_file);

    _file = 0;
  }
}


/// access position, no range checking (faster)
unsigned char MemoryMapped::operator[](size_t offset) const
{
  return ((unsigned char*)_mappedView)[offset];
}


/// access position, including range checking
unsigned char MemoryMapped::at(size_t offset) const
{
  // checks
  if (!_mappedView)
    throw std::invalid_argument("No view mapped");
  if (offset >= _filesize)
    throw std::out_of_range("View is not large enough");

  return operator[](offset);
}


/// raw access
const unsigned char* MemoryMapped::getData() const
{
  return (const unsigned char*)_mappedView;
}


/// true, if file successfully opened
bool MemoryMapped::isValid() const
{
  return _mappedView != NULL;
}


/// get file size
uint64_t MemoryMapped::size() const
{
  return _filesize;
}


/// get number of actually mapped bytes
size_t MemoryMapped::mappedSize() const
{
  return _mappedBytes;
}


/// replace mapping by a new one of the same file, offset MUST be a multiple of the page size
bool MemoryMapped::remap(uint64_t offset, size_t mappedBytes)
{
  if (!_file)
    return false;

  if (mappedBytes == WholeFile)
    mappedBytes = _filesize;

  // close old mapping
  if (_mappedView)
  {

    ::UnmapViewOfFile(_mappedView);

    _mappedView = NULL;
  }

  // don't go further than end of file
  if (offset > _filesize)
    return false;
  if (offset + mappedBytes > _filesize)
    mappedBytes = size_t(_filesize - offset);


  // Windows

  DWORD offsetLow  = DWORD(offset & 0xFFFFFFFF);
  DWORD offsetHigh = DWORD(offset >> 32);
  _mappedBytes = mappedBytes;

  // get memory address
  _mappedView = ::MapViewOfFile(_mappedFile, FILE_MAP_READ, offsetHigh, offsetLow, mappedBytes);

  if (_mappedView == NULL)
  {
    _mappedBytes = 0;
    _mappedView  = NULL;
    return false;
  }

  return true;

}


/// get OS page size (for remap)
int MemoryMapped::getpagesize()
{

  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  return sysInfo.dwAllocationGranularity;

}
