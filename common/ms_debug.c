#ifdef __KERNEL__
    #include <linux/module.h>
    #pragma GCC diagnostic ignored "-Wunused-value"
#else
    #include <stdio.h>
    #include <stdarg.h>
#endif
#include "ms_debug.h"


#ifdef __KERNEL__
int isprint(int c)
{
    return (c >= 0x20 && c<=0x7E);  
}
#endif

void trace(
    const char * format,
    char * filename,
    const char * funcname,
    unsigned int line,
    ...)
{
    char buffer[256];   
    va_list argptr;
    
#ifdef __KERNEL__
    (void *)filename;
#endif
    
    if ((format == NULL) || (funcname == NULL)
#ifndef __KERNEL__
    || (filename == NULL))
#else
    )
#endif
        
    {
        goto done;
    }
    
    va_start(argptr, line);
    vsnprintf(buffer, 256, format, argptr);
    va_end(argptr); 
    
#ifdef __KERNEL__
    PRINTF("Maplesyrup [%s] [%d]: %s", funcname, line, buffer);
#else
    PRINTF("Maplesyrup [%s] [%s] [%d]: %s", filename, funcname, line, buffer);
#endif
    
done:
    return; 
}

void
hex_dump(
    unsigned int * ptr,
    unsigned int len)
{
    unsigned int i = 0;
    char * pBuffer = NULL;
    
    if ((ptr == NULL) || (len == 0))
    {       
        goto done;      
    }
    
    PRINTF("=======================================\n");
    PRINTF("Dumping %d bytes @ %p\n", len, ptr);
    PRINTF("=======================================\n");

    
    pBuffer = (char *)ptr;
    for (i = 0; i < len; i++)
    {
        PRINTF("%02x", *pBuffer & 0xFF);
        
        if (((i + 1) % 4) == 0)
        {
            PRINTF(" ");
        }
        if (((i + 1) % 32) == 0)
        {
            PRINTF(" (+0x%x)", i - 0x1F);
            PRINTF("\n");
        }
        
        pBuffer++;
    }
    
    PRINTF("\n\n\n");
    pBuffer = (char *)ptr;
    for (i = 0; i < len; ++i)
    {
        if (isprint(*pBuffer & 0xFF))
        {
            PRINTF("%c ", *pBuffer);
        }
        else
        {
            PRINTF(". ");
        }
        if (((i + 1) % 4) == 0) { PRINTF(" "); }
        if (((i + 1) % 32) == 0) { PRINTF(" (+0x%x)", i - 0x1F); PRINTF("\n"); }
        
        pBuffer++;
        
    }
    
    PRINTF("\n");
    PRINTF("=======================================\n");
    
    
done:
    return;
}
