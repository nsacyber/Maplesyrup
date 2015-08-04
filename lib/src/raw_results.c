#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include "parse.h"
#include "ms_debug.h"
#include "ms_error.h"
#include "open_device.h"
#include "raw_results.h"


/******************************************************
 * Passes the results to the caller
 *  
 * @param [in] cmd IOCTL code 
 * @param [out] Results from the processor
 * 
 * ***************************************************/
int
driver_results(
    unsigned int cmd,
    ms_data_cpu * results)
{
    long result = -1;
    unsigned int i = 0;
    
    if (results == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    if (ms_fd == -1)
    {
        TRACE("Device not opened\n", 0);
        goto done;
    }
    
    /* Initialize status for all CPUs */
    for (i = 0; i < MS_MAX_CPUS; ++i)
    {
        results->cpu[i].status = MS_ERROR_NORESULT;
    }
        
    if (ioctl(ms_fd, cmd, results) == -1)
    {
        TRACE("Failed to send cmd 0x%x: errno x%x\n", cmd, errno);
        goto done;
    }
        
    result = 0;
done:
    return result;
}
