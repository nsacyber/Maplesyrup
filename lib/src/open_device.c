#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "open_device.h"
#include "cmd_handler.h"
#include "ms_debug.h"

int ms_fd = -1;

int
start_test()
{
    int result = -1;    
    
    /* Handle signals */    
    if (register_signal_handlers() != 0)
    {
        printf("Failed to register signal handlers");
        goto done;
    }   
    TRACE("Registered signal handlers\n", 0);

    /* Open device */
    ms_fd = open("/dev/maplesyrup", O_RDWR);
    if (ms_fd == -1)
    {
        printf("Failed to open device\n");
        goto done;
    }
    TRACE("Device opened: fd: 0x%x\n", ms_fd);  

    result = 0; 
    
done:
    return result;
}

void
stop_test()
{   
    if (ms_fd == -1)
    {
        TRACE("No valid device fd found\n", 0);     
        goto done;
    }
    TRACE("Closing device: 0x%x\n", ms_fd);
    close(ms_fd);   
    ms_fd = -1; 
    
    
    TRACE("Restoring signal handlers\n", 0);
    restore_signal_handlers();

done:
    return;
}


