/* $Id: fname.c,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */

#include <string.h>
#include <stdio.h>
#include "jjudge.h"
#include "fname.h"

char *fname_as_file(char *buf)
{
  char *mark = NULL;
  char *p = buf;

  jjudge_t judger;
  jjudge_init(&judger);

  while (*p) {
    unsigned char c = *p;
    if (jjudge_equal(&judger, c, FNAME_FS_CHAR)) { if (!mark) mark = p; }
    else mark = NULL;
    p++;
  }

  if (mark) *mark = '\0';
  return buf;
}

char *fname_as_dir(char *buf)
{ 
  return strcat(fname_as_file(buf), FNAME_FS_CSTR);
}

char *fname_get_parent(char *buf)
{
  char *mark = NULL;
  char *p = buf;
  int conti = 0;

  jjudge_t judger;
  jjudge_init(&judger);

  fname_as_file(buf);

  while (*p) {
    unsigned char c = *p;
    if (jjudge_equal(&judger, c, FNAME_FS_CHAR)) {
      if (!conti) { mark = p; conti++; }
    } else conti = 0;
    p++;
  }

  if (mark) { *mark = '\0'; return buf; }
  return NULL;
}

char *fname_get_file(char *buf)
{
  char *mark = NULL;
  char *p = buf;

  jjudge_t judger;
  jjudge_init(&judger);

  fname_as_file(buf);

  while (*p) {
    unsigned char c = *p;
    if (jjudge_equal(&judger, c, FNAME_FS_CHAR)) { mark = p; }
    p++;
  }

  if (mark) {
    mark++;
    p = buf;
    while ((*p++ = *mark++));
  }

  return buf;
}

#ifdef DEBUG_fname_pgm

#if defined(WIN32)
int main(void)
{
  char buf[100] = "表\\\\\\";
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_file(buf));

  strcpy(buf, "c:\\foo\\bar\\baz\\");
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_file(buf));
  printf("\n");

  strcpy(buf, "c:\\foo\\bar\\表\\\\");
  printf("%s\n", fname_as_dir(buf));
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_dir(buf));
  printf("\n");

  strcpy(buf, "c:\\表\\表示\\表\\\\");
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("\n");

  strcpy(buf, "表\\\\");
  printf("%s\n", (fname_get_parent(buf) == NULL) ? "" : buf);

  strcpy(buf, "aaaa");
  printf("%s\n", (fname_get_parent(buf) == NULL) ? "" : buf);

  printf("\n");

  strcpy(buf, "c:\\表\\表示\\表\\\\");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "c:\\表\\表示");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "表");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "abc");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "");
  printf("%s\n", fname_get_file(buf));
  
  return 0;
}
#else
int main(void)
{
  char buf[100] = "aa//";
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_file(buf));

  strcpy(buf, "/usr/local/bin//");
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_file(buf));
  printf("\n");

  strcpy(buf, "/usr/local/bin/");
  printf("%s\n", fname_as_dir(buf));
  printf("%s\n", fname_as_file(buf));
  printf("%s\n", fname_as_dir(buf));
  printf("\n");

  strcpy(buf, "/usr/bin////");
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_get_parent(buf));
  printf("%s\n", fname_as_dir(buf));
  printf("\n");

  strcpy(buf, "/usr/bin//");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "/usr/bin");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "aaa");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "abc");
  printf("%s\n", fname_get_file(buf));

  strcpy(buf, "");
  printf("%s\n", fname_get_file(buf));
  printf("%s\n", fname_as_dir(buf));
  
  return 0;
}
#endif
#endif

