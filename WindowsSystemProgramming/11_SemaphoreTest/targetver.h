/*
The targetver.h file is used to specify the minimum operating system requirements for building the application.
It includes the <SDKDDKVer.h> header file, which defines macros that indicate the minimum version of the Windows operating system required to run the application.
This allows the application to use features that are only available on newer versions of Windows and to avoid using features that are not available on older versions.
The macros defined in <SDKDDKVer.h> are used by other system headers to enable or disable functionality based on the target operating system version.
*/

#pragma once

#include <SDKDDKVer.h>
