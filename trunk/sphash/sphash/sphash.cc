// SPHash.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "sphash.h"
#include "unrar.hpp"

void dummy_unrar()
{
  RAROpenArchiveEx(NULL);
}

void hash_file(int hash_mode, int hash_algorithm, 
               const wchar_t* file_name, 
               char* result_inout, int* result_len)
{
  dummy_unrar();
}