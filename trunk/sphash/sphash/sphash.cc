// SPHash.cc : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "sphash.h"
#include "unrar.hpp"
#include "MD5Checksum.h"

using namespace std;

// files format:
// d:\\a\\a1.ext\0d:\\a\\a2.ext\0\0
vector<wstring> get_filelist(const wchar_t* file)
{
  vector<wstring> filelist;
  wstring filepath;

  int num_null = 0;
  int char_index = 0;
  int last_char_index = 0;
  while (true)
  {
    switch (file[char_index])
    {
    case 0:
      num_null++;
      if (num_null >= 2)
        break;
      filepath.assign(&file[last_char_index], char_index - last_char_index);
      filelist.push_back(filepath);
      last_char_index = char_index+1;
      break;
    default:
      num_null = 0;
      break;
    }
    char_index++;
    if (num_null >= 2)
      break;
  }

  return filelist;
}

// rar file format
// rar://C:\\Project\\SAMPLE.rar?SAMPLE.mkv
int rarfile_handle(wstring &path, wstring &rarpath)
{
  wstring prefix = path.substr(0, 6);
  transform(prefix.begin(), prefix.end(), prefix.begin(), tolower);

  if (prefix != L"rar://")
    return 0;

  int pos = path.find('?');
  if (pos == path.npos)
    return 0;

  rarpath = path.substr(pos+1, path.length()-pos-1);
  path = path.substr(6, pos-6);

  return 1;
}

int file_exist(wstring &path, wstring &newpath)
{
  rarfile_handle(path, newpath);

  struct _stat sbuf;
  return (!_wstat(path.c_str(), &sbuf) && sbuf.st_mode & _S_IFREG);
}

string bintotext(unsigned char *buf)
{
  string str;

  for (int i = 0; i < 16; i++)
  {
    stringstream textstream;
    textstream << setw(2) << setfill('0') << hex << (unsigned int)buf[i];
    str += textstream.str().c_str();
  }

  return str;
}

int modhash_file(unsigned char md5buf[16], const wchar_t* file_name, char* result_inout, int* result_len)
{

  vector<wstring> filelist = get_filelist(file_name);
  for (vector<wstring>::iterator it = filelist.begin(); it != filelist.end(); it++)
  {
    MD5Checksum md5;
    wstring strbuf;
    wstring newpath, path = *it;

    if (!file_exist(path, newpath))
      return 1;

    strbuf = md5.GetMD5(path);
    if (strbuf.empty())
      continue;

    if (it == filelist.begin())
      memcpy(md5buf, md5.lpszMD5, 16);
    else 
      for (int j = 0; j < 16; j++)
        md5buf[j] ^= md5.lpszMD5[j];
  } // for filelist

  return 0;
}

int modhash_video(vector<vector<unsigned char> > &strbuf, const wchar_t* file_name, char* result_inout, int* result_len)
{
  wstring path(file_name);
  wstring rarpath;

  __int64 offset[4];

  if (rarfile_handle(path, rarpath))
  {
    struct RAROpenArchiveDataEx rarst;
    memset(&rarst, 0, sizeof(rarst));

    rarst.ArcNameW  = (LPWSTR)path.c_str();
    rarst.OpenMode  = RAR_OM_EXTRACT;
    rarst.CmtBuf    = 0;

    HANDLE hrar = RAROpenArchiveEx(&rarst);
    if (hrar)
    {
      struct RARHeaderDataEx HeaderDataEx;
      HeaderDataEx.CmtBuf = NULL;

      while (RARReadHeaderEx(hrar, &HeaderDataEx) == 0)
      {
        if (HeaderDataEx.Method != 0x30) // only support stored file
          break;

        wstring subfn = HeaderDataEx.FileNameW;
        if (subfn.compare(rarpath.c_str()) == 0)
        {
          int errRar = RARExtractChunkInit(hrar, HeaderDataEx.FileName);
          if (errRar != 0)
            break;

          __int64 ftotallen = HeaderDataEx.UnpSize;

          if (ftotallen >= 8192)
          {
            offset[3] = ftotallen - 8192;
            offset[2] = ftotallen / 3;
            offset[1] = ftotallen / 3 * 2;
            offset[0] = 4096;
            MD5Checksum md5;
            char buf[4096];
            for (int i = 0; i < 4; i++)
            {
              RARExtractChunkSeek(hrar, offset[i], SEEK_SET);
              //hash 4k block
              RARExtractChunk(hrar, (char*)buf, 4096);
              md5.GetMD5((unsigned char*)buf, 4096);
              vector<unsigned char> str(16);
              memcpy(&str[0], md5.lpszMD5, 16);
              strbuf.push_back(str);
            }
          }
          RARExtractChunkClose(hrar);
          break;
        }
        RARProcessFile(hrar, RAR_SKIP, NULL, NULL);
      } // while
      RARCloseArchive(hrar);
    }
  } // if (rarfile_handle(path))

  if (strbuf.empty())
  {
    int stream;
    errno_t err;
    err =  _wsopen_s(&stream, path.c_str(), _O_BINARY|_O_RDONLY, _SH_DENYNO, _S_IREAD);
    if (!err)
    {
      __int64 ftotallen  = _filelengthi64(stream);
      if (ftotallen >= 8192)
      {
        offset[3] = ftotallen - 8192;
        offset[2] = ftotallen / 3;
        offset[1] = ftotallen / 3 * 2;
        offset[0] = 4096;
        MD5Checksum md5;
        unsigned char Buf[4096];
        for (int i = 0; i < 4; i++)
        {
          _lseeki64(stream, offset[i], 0);
          //hash 4k block
          int readlen = _read( stream, Buf, 4096);
          md5.GetMD5(Buf, readlen);
          vector<unsigned char> str(16);
          memcpy(&str[0], md5.lpszMD5, 16);
          strbuf.push_back(str);
        }
      }
      _close(stream);
    }
  }

  return 0;
}

void algo_md5(int hash_mode, const wchar_t* file_name, 
                  char* result_inout, int* result_len)
{
  switch (hash_mode)
  {
  case HASH_MOD_FILE_BIN:
  case HASH_MOD_FILE_STR:
    {
      unsigned char md5buf[16];
      if (modhash_file(md5buf, file_name, result_inout, result_len) != 0)
        return;

      if (hash_mode == HASH_MOD_FILE_BIN)
      {
        *result_len = 16;
        memcpy(result_inout, md5buf, *result_len);
      }
      else
      {
        string text = bintotext(md5buf);
        text.push_back(0);
        *result_len = text.size();
        memcpy(result_inout, text.c_str(), *result_len);
      }
    }
    break;
  case HASH_MOD_VIDEO_STR:
    {
      vector<vector<unsigned char> > strbuf;
      if (modhash_video(strbuf, file_name, result_inout, result_len) != 0)
      return;

      string text;
      for (vector<vector<unsigned char> >::iterator it = strbuf.begin(); it != strbuf.end(); it++)
      {
        if (it != strbuf.begin())
          text += ';';

        text += bintotext(&(*it)[0]);
      }
      text.push_back(0);
      *result_len = text.size();
      memcpy(result_inout, text.c_str(), *result_len);
    }
    break;
  default:
    break;
  }
}

void hash_file(int hash_mode, int hash_algorithm, 
               const wchar_t* file_name, 
               char* result_inout, int* result_len)
{
  *result_len = 0;

  switch (hash_algorithm)
  {
  case HASH_ALGO_MD5:
    algo_md5(hash_mode, file_name, result_inout, result_len);
    break;
  default:
    break;
  }
}