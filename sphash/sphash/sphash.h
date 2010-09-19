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

extern "C"
{

SPHASH_API void hash_file(int hash_mode, int hash_algorithm, 
                          const wchar_t* file_name, 
                          char* result_inout, int* result_len);

};

#endif // SPHASH_H