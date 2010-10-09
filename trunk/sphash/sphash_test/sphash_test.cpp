#include "pch.h"
#include "../sphash/sphash.h"
#include <string>

#define TEST_ENTER(testcase) \
{ \
  printf("\n--------------------------------------------\n");\
  printf(#testcase "\n");\
}

#define TEST_RESULT(testcase, condition, variable) \
{ \
  if (##condition)\
  printf ("\ntest [%s] is [PASSED]\n", testcase);\
    else\
    printf ("\ntest [%s] [FAILED], condition requires (" #condition ") and its value is: %d\n", testcase, variable);\
    \
}


int main(int argc, char* argv[])
{
  TEST_ENTER("video file and RAR file");
  wchar_t file[MAX_PATH] = L"SAMPLE.mkv\0\0";
  char out[300], out2[300];
  int len, len2;
  hash_file(HASH_MOD_VIDEO_STR, HASH_ALGO_MD5, file, out, &len);
  printf("[file hash]: %s\n", out);

  wchar_t file2[MAX_PATH] = L"rar://SAMPLE.rar?SAMPLE.mkv\0\0";
  hash_file(HASH_MOD_VIDEO_STR, HASH_ALGO_MD5, file2, out2, &len2);
  printf("[RAR file hash]: %s\n", out2);

  TEST_RESULT("video file and RAR file", strcmp(out, out2)==0, strcmp(out, out2));


  TEST_ENTER("sub file");
  wchar_t file3[MAX_PATH] = L"Piranha.2010-r5.xvid-vision.srt\0\0";
  char out3[300];
  int len3;
  hash_file(HASH_MOD_FILE_STR, HASH_ALGO_MD5, file3, out3, &len3);
  printf("[file hash]: %s\n", out3);
  TEST_RESULT("sub file", strcmp(out3, "aae217e9af85b58fe1f96c923511359b")==0, strcmp(out3, "aae217e9af85b58fe1f96c923511359b"));

	return 0;
}

