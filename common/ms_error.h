#ifndef _MS_ERROR_H
#define _MS_ERROR_H

enum
{
    MS_ERROR_SUCCESS = 0,   // success
    MS_ERROR_ILLINST,       // illegal instruction
    MS_ERROR_UNDEFINST,     // undefined instruction
    MS_ERROR_SMP,           // generic smp error
    MS_ERROR_SMP_SINGLE,    // smp thread schedule fail
    MS_ERROR_NORESULT,      // cpu info not requested
    MS_ERROR_TABLE,         // problem reading table
    
};

#endif
