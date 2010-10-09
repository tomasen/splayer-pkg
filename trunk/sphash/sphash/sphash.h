#ifndef SPHASH_H
#define SPHASH_H

#if defined(WIN32)

#ifdef SPHASH_EXPORTS
#define SPHASH_API __declspec(dllexport)
#else
#define SPHASH_API __declspec(dllimport)
#endif

#else

#define SPHASH_API

#endif // defined(WIN32)

#define   HASH_ALGO_MD5         0


#define   HASH_MOD_FILE_STR     0
#define   HASH_MOD_FILE_BIN     1
#define   HASH_MOD_VIDEO_STR    2

extern "C"
{

// hash_file expects |file_name| to be double null terminated.
// to hash more than one file, use L"filename1\0filename2\0\0"
SPHASH_API void hash_file(int hash_mode, int hash_algorithm, 
                          const wchar_t* file_name, 
                          char* result_inout, int* result_len);

};

#endif // SPHASH_H