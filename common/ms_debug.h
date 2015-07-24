#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __KERNEL__
#else
    #include <ctype.h>
    #include <stdarg.h>
#endif

#ifdef _MS_DEBUG
    #define TRACE(fmt, ...) \
        do { trace(fmt, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#else
//#ifdef __KERNEL__ 
//  #define TRACE(fmt, ...) do { trace(fmt, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
//#else
    #define TRACE(fmt, ...)
//#endif //__KERNEL__
#endif // _MS_DEBUG

#ifdef __KERNEL__
    #define PRINTF printk
#else
    #define PRINTF printf
#endif
    

void trace(const char * format, char * filename, const char * funcname, unsigned int line, ...);
void hex_dump(unsigned int * ptr,unsigned int len);


#endif
