#ifndef CD3GLOBALS_H
#define CD3GLOBALS_H

#ifdef _MSC_VER
#pragma warning( disable: 4251 )
#endif
#ifdef _WIN32
//#define CD3_PUBLIC __attribute__ ((visibility("default")))
#define CD3_PUBLIC __declspec(dllexport)
#else
#define CD3_PUBLIC
#endif

#endif // CD3GLOBALS_H
