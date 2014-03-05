/*
 * Created by okamoto on 2011-03-25
 */

#ifndef fname_h
#define fname_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined(unix)
#define FNAME_FS_CHAR    '/'
#elif defined(WIN32)
#define FNAME_FS_CHAR    '\\'
#else
#define FNAME_FS_CHAR    '\0'
#endif

#if defined(unix)
#define FNAME_FS_CSTR    "/"
#elif defined(WIN32)
#define FNAME_FS_CSTR    "\\"
#else
#define FNAME_FS_CSTR    ""
#endif

extern char *fname_as_file(char *buf);
extern char *fname_as_dir(char *buf);

extern char *fname_get_parent(char *buf);
extern char *fname_get_file(char *buf);

#ifdef __cplusplus
}
#endif

#endif /* fname_h */
