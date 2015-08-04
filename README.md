Maplesyrup Register Display Tool
==================================
Maplesyrup is a tool that can be used to help determine the security state of an
ARM-based device by examining the system register interface of the CPU.


### Who is this for?
--------------------
Maplesyrup is for anyone who has low level access to a handset or single-board PC
running an ARMv7A/v8A based processor and is interested in knowing the register level
configuration of their CPU at OS runtime. These registers contain featureset and
security information that may influence operation of the system kernel and
running applications.


### Why was this created?
--------------------------
Linux provides featureset and platform information to the user in the /proc and /sys
filesystems, but the configurations governing how these features operate
is sometimes hidden to the user. In some cases, the OS will make use of the 
information to conform to implementation specific features and not
indicate this to the user. In other cases, these features may not be managed
by the operating system at all, but nevertheless could potentially affect the
operation of the system by configuring how a CPU controls access to security
domains, executes specific instructions, and handles CPU exceptions.


### How does it work?
---------------------
Maplesyrup consists of a kernel driver and a user mode component.
The kernel driver collects the information and returns it to the
user mode component which parses the information and presents it
to the user.


### What can I do with the results?
-----------------------------------
The results will show the low level operating configuration of the system,
and can be used to help determine the security state of a device.
They may include security settings such as the status of Virtualization
Extensions, Security Extensions and coprocessor access restrictions. A few
specific examples are listed below:

*   The UWXN and WXN fields within the SCTLR register on ARMv7A based processors
    will affect how the Execute Never (XN) feature will operate.    
*   The Domain Access Control Register (DACR) defines access permissions
    for each of the 16 memory domain regions.
*   The coprocessor Access Control Register (CPACR) can be used to 
    determine whether a coprocessor is implemented and what it's access
    controls are.
*   Trap information details the execution level certain exceptions will be
    taken to and which stack pointer will be used (for AARCH64).
*   Debug Architecture access permissions


### What do the results represent?
----------------------------------
The data stored in the system registers may or may not represent how
the chip actually operates and the nature of the value is entirely
defined by the vendor implementing the device. In addition, some registers
will be read at boot time and the results stored to memory. In these cases,
manually flipping the bit after the OS has booted will likely not have any
effect. Another thing to note is that several registers report information
that is relevant to a procedure and may have no further relevance once that
procedure has completed.


### Features/Limitations
------------------------
* Multi-core support
* Memory-mapped registers are currently not supported
* Only EL0/EL1 are supported
* Support for Cortex A7/A15/A53/A57
* Experimental GICv2 support

  *   This application reads only a small portion of the available system
      registers on an ARM CPU. Please refer to the ARMv7A and ARMv8A Architecture
      Manuals and the appropriate Cortex TRM for official documentation.   

WARNING: This application and kernel module are to be used only
on test systems. Leaving the kernel module installed
on a non-test system will compromise the system's security.
         
### Platforms Tested
--------------------
*   Galaxy S5 Exynos Variant
*   ODROID-XU3
*   HOWCHIP 5250 Ver. C
*   Versatile Express-A9 (QEMU)
*   Android Goldfish 3.4 (QEMU)
*   Versatile JUNO Development Platform
   
### Requirements
----------------
*   Ubuntu 14.04
*   Toolchain for target architecture (if cross compiling) to build usermode and kernel components
  * _arm-linux-gnueabi_ for 32-bit
  * _aarch64-linux-gnu_ for 64-bit
  * _arm-linux-androideabi_ for Android
*   Kernel source tree (required for kernel module build)
*   Linux based device with root access and the ability to load unsigned kernel modules


### Directory Structure
-----------------------
```
./common - Files common to both the kernel module and user mode component
./lib    - The user mode parsing library
./module - The kernel module
./frontend - Entry point and command line parsing

Final build executables will be './frontend/maplesyrup' and './module/maplesyrup_mod.ko'
```

### Build Instructions
----------------------

#### Native ARM Compilation
```
    Run autogen.sh
    ./configure LDFLAGS="-static" 
    make
    cd module   
    export KERNEL_SRC=<path to kernel source tree>
    make
```

#### ARM Cross-Compilation
```
    Run autogen.sh
    Ensure path to toolchain binaries is in PATH (PATH=$PATH:/path/to/toolchain)
    ./configure --host=arm-linux-gnueabi LDFLAGS="-static" (for 32bit)
            --OR--
    ./configure --host=aarch64-linux-gnu LDFLAGS="-static" (for 64bit)
    make  
    cd module
    export ARCH=arm (or arm64)
    export CROSS_COMPILE=arm-linux-gnueabi- (or aarch64-linux-gnu-)
    export KERNEL_SRC=<path to kernel source tree>
    make
```

#### Android Compilation
```
    Requirements: The device must be rooted and the kernel must be configured for loadable kernel modules.

    Download Android NDK
    
    export CROSS_COMPILE=arm-linux-androideabi
    export CC=${CROSS_COMPILE}-gcc
    export CXX=${CROSS_COMPILE}-g++
    export NDK=<path to>/android-ndk-r10d/
    export SYSROOT=$NDK/platforms/android-21/arch-arm
    PATH=$PATH:$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin
    
    Run autogen.sh
    ./configure --host=arm-linux-androideabi CFLAGS="--sysroot=$SYSROOT" LDFLAGS="-static"
    make
    cd module
    export ARCH=arm
    export CROSS_COMPILE=arm-linux-gnueabi-
    export KERNEL_SRC=<path to android kernel source tree>
    make
```

### General Installation Instructions
-------------------------------------
*   Installation on a native ARM system is a simple 'sudo make install'. The kernel
    module can be installed with the typical loadable module routines.

*   Installation onto a virtual machine or ARM-based test system involves
    transferring the 'maplesyrup' executable and the 'maplesyrup_mod.ko' module
    to the system via 'scp' or similar utility.

*   Installation on an Android based handset requires root access and the ability
    for the kernel to load unsigned kernel modules. Once you have this, installation
    is simply copying (via an SD card) the cross-compiled user mode component
    to '/system/bin'. The kernel module can be loaded directly from the SD card.
    These Android instructions have only been tested on a Galaxy S5 and may differ
    from handset to handset.

*   Make sure to 'rmmod maplesyrup_mod' when done to unload the kernel module. Failure
    to do this may leave the system in an unsafe state.

### Usage Instructions
----------------------
```
Usage: maplesyrup --<register|group|bitfield|all> <NAME or ID> --arch=<ARCH> --impl=<IMPL or all> [--core=<CORE or -1>] [OTHER OPTIONS]

  --arch - armv7a or armv8a
  --impl - cortex-a7, cortex-a15, cortex-a53, cortex-a57, or all
  --core - core number, or -1 for all cores
  --calc - supply register value. Use with --register <regname>
  --help - show this menu
  --group - search by group
  --register - search by register
  --bitfield - search by bitfield
  --all - search all available bitfields
  --noparse - don't parse the results into bitfields
  --showfail - show all results regardless of success
  --el0 - force execution at pl0
  --el1 - force execution at pl1
  --show_groups - shows all valid functional groups
  --show_bitfields - shows all valid bitfields
  --show_registers - shows all valid register

Examples:
    (1) maplesyrup --register MIDR --arch=armv7a --impl=all
        to display the MIDR register
    (2) maplesyrup --all --arch=armv7a --impl=all
        to display the all registers
    (3) maplesyrup --register MIDR --arch=armv7a --impl=all --calc=0x410fc073
        supply a value

    Sample output 
    > maplesyrup --register MIDR --arch=armv7a --impl=all --core=0
    
    ============================
    || Maplesyrup 1.0         ||    
    ||    -linux              ||
    ||    -aarch32            ||
    ||                        ||
    || 000005/000040 entries  ||
    ============================
    [cpu0/cortex-a7/armv7a/1/ident/MIDR/03:00/Revision]: 0x3 (Revision) (0)
    [cpu0/cortex-a7/armv7a/1/ident/MIDR/15:04/Primary part number]: 0xc07 (Primary part number) (0)
    [cpu0/cortex-a7/armv7a/1/ident/MIDR/19:16/Architecture]: 0xf (Architecture) (0)
    [cpu0/cortex-a7/armv7a/1/ident/MIDR/23:20/Variant]: 0x0 (Variant) (0)
    [cpu0/cortex-a7/armv7a/1/ident/MIDR/31:24/Implementer]: 0x41 (Implementer) (0)
      |      |        |    |   |     |     |       |          |        |         |
      |      |        |    |   |     |     |       |          |        |         |
     Core #  |        |    |   |     |     |       |          |        |         |
         Part number  |    |   |     |     |       |          |        |         |
     The decode table used |   |     |     |       |          |        |         |
           The exception level |     |     |       |          |        |         |
                The functional group |     |       |          |        |         |
                         The register name |       |          |        |         |
                    The bitfield size and position |          |        |         |
                                             The field name   |        |         |
                                                 Actual result value   |         |
                                                            A brief description  |
                                                                            Result validity
```
*   Only valid results will typically be displayed, but invalid results can be shown with the
    --showfail flag. This could help in debugging why a certain register could not be read.
    Errors could be any of the following:

    ```
    MS_ERROR_SUCCESS    = 0,    // Success!
    MS_ERROR_ILLINST    = 1,    // Illegal instruction
    MS_ERROR_UNDEFINST  = 2,    // Undefined instruction
    MS_ERROR_SMP        = 3,    // Unable to bring CPU up
    MS_ERROR_SMP_SINGLE = 4,    // Unable to schedule thread on CPU
    MS_ERROR_NORESULT   = 5,    // CPU does not exist, or was not requested
    MS_ERROR_TABLE      = 6,    // Problem reading the decode table
    ```

*   The actual result value is typically going to be the default "reset"
    value as recommended by the vendor. If it's not, for example if all access
    restrictions for all the memory domains is set to 0x3 on a commercial
    handset device, then that may be something worth investigating. Typically,
    0x1 (or any non-zero value) means that a setting is "on" or active.
    A 0x0 means that the setting is "off" or not implemented. Of course, to
    determine specifically what a value means, you currently have to consult
    the appropriate Technical Reference Manual for your processor.

*   Most field names were chosen to match those found in various ARM documents.
    In cases where an abbreviated field name could not be found then a reasonable
    acronym was created from the description of the bit's function. Similarly for
    group names, in cases where a functional group for a register could not be found,
    then it was either assigned to the "impl_def" (implementation defined) group
    or the "other" group. Also, RAZ, RES0, RA0 and RES1 fields are simply
    referred to as "Reserved".

*   The bitfield size and position represents the starting and stopping
    points within the register value that the decoding represents.
    Parsing into fieldnames can be disabled with the --noparse flag.

*   Register naming conventions follow the ARMv7A architecture. If
    there is a direct mapping between an ARMv8A register and an ARMv7A
    register, then the ARMv7A name is used. If there is no direct mapping,
    then the register is prefixed with "AA64" (for example, "AA64MMFR0").
    The exception level typically appended to ARMv8A registers,
    and any register prefix such as "ID_" has been omitted (for example,
    ID_AARCH64MMFR0_EL1 will just be AA64MMFR0).

*   For Linux, the exception level indicates whether the register was accessed
    in user mode (0) or kernel mode (1). In most cases, the register will be
    accessed in the mode that requires the least amount of privilege and
    still return defined results. On 32-bit chips, it is possible to specify
    a desired exception level with the --el0 or --el1 flags. This feature on
    aarch64 systems is currently incomplete.

*   The decode table used indicates which Maplesyrup internal table was used
    to parse the 32bit or 64bit register value. The part number is the actual
    Cortex type (called a "part number" internally) reported by the chip itself.
    Registers accessed at EL0 will have a NULL part number.

*   Core number is the core the registers were accessed on. Multi-core systems
    may have a different set of available registers for each cluster. Maplesyrup
    is configured to support 8 cores, but can be changed to support more through
    the MS_MAX_CPUS #define in 'parse.h'. Registers accessed at EL0 (user mode in
    Linux) will have a core number of 0 and is not indicative of the actual core
    the code was run on.

*   Maplesyrup does not intend to represent an exhaustive list of all
    coprocessor registers accessible via the MRC and MRS instructions, and
    any obvious exclusion should not be read as intentional.
    There are a lot of registers and any one could have been unintentionally
    skipped or misidentified.
    
*   I make no guarantee that any of the above statements are complete or true,
    or that the results returned by Maplesyrup are correct, accurate or
    meaningful. I _can_ guarantee that it compiles successfully on my system. YMMV.

### How do I add my own bitfield decoding?
------------------------------------------
Bitfield decodings are statically programmed into the application, but
can easily be expanded to include new or missing decodings. The relevant
files you may need to modify are listed below:
*   ./lib/src/parse_breakout.c

The following files may need to be modified if a new register needs to be added:
*   ./common/jumptable.c
*   ./common/parse.h

The actual decode tables are stored under
*   ./lib/src/bitfields/
and follow the naming convention bitfield_*.c/h.
