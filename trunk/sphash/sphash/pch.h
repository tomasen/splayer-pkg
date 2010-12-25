#ifndef PCH_H
#define PCH_H

#if defined(WIN32)

#ifndef WINVER                  // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE               // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <io.h>
#endif // defined(WIN32)

#include <vector>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>

#ifdef _MAC_

#define _DARWIN_FEATURE_64_BIT_INODE

#define UCHAR unsigned char
#define __int64 __int64_t
#define MAX_PATH 1024

#include <CoreFoundation/CoreFoundation.h>

class Utf8
{
public:
  Utf8(const wchar_t* wsz): m_utf8(NULL)
  {
    // OS X uses 32-bit wchar
    const int bytes = wcslen(wsz) * sizeof(wchar_t);
    // kCFStringEncodingUTF32BE for PowerPC
    CFStringEncoding encoding = kCFStringEncodingUTF32LE;
   
    CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
                                                    (const UInt8*)wsz, bytes, 
                                                    encoding, false, 
                                                    kCFAllocatorNull
                                                    );
    
    const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
    m_utf8 = new char[bytesUtf8];
    CFStringGetFileSystemRepresentation(str, m_utf8, bytesUtf8);
    CFRelease(str);
  }   
  
  ~Utf8() 
  { 
    if( m_utf8 )
    {
      delete[] m_utf8;
    }
  }
  
public:
  operator const char*() const { return m_utf8; }

private:
    char* m_utf8;
};

#endif // _MAC_

#endif // PCH_H