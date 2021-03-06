/*
 * Copyright (c) 2018 ManyCoreSoft Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software source and associated documentation files (the "Materials"),
 * to deal in the Materials without restriction, including without limitation
 * the rights to use, copy, modify, compile, merge, publish, distribute,
 * sublicense, and/or sell copies of the Materials, and to permit persons to
 * whom the Materials are furnished to do so, subject the following terms and
 * conditions:
 *
 * All modifications to the Materials used to create a binary that is
 * distributed to third parties shall be provided to Khronos with an
 * unrestricted license to use for the purposes of implementing bug fixes and
 * enhancements to the Materials;
 *
 * If the binary is used as part of an OpenCL(TM) implementation, whether binary
 * is distributed together with or separately to that implementation, then
 * recipient must become an OpenCL Adopter and follow the published OpenCL
 * conformance process for that implementation, details at:
 * http://www.khronos.org/conformance/;
 *
 * The above copyright notice, the OpenCL trademark license, and this permission
 * notice shall be included in all copies or substantial portions of the
 * Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE USE OR OTHER DEALINGS IN
 * THE MATERIALS.
 *
 * OpenCL is a trademark of Apple Inc. used under license by Khronos.
 */

#ifndef _ICD_ENVVAR_H_
#define _ICD_ENVVAR_H_

#include <CL/cl.h>

typedef struct KHRVisibilityRec KHRVisibility;

struct KHRVisibilityRec
{
    char *library;
    unsigned platformIndex;
    cl_device_type deviceType;
    unsigned numVisibleDevices;
    unsigned *deviceIndices;

    cl_platform_id platformID;
    cl_device_id *deviceIDs;

    KHRVisibility *next;
};

void khrIcdOsGetOpenCLVisibleDevicesOnce(void);
int khrIcdOsLibraryFileMatch(const char *name, const char *fileName);

void khrIcdVisibilityAdd(char *library, unsigned platformIndex, cl_device_type deviceType, unsigned numVisibleDevices, unsigned *deviceIndices);
void khrIcdVisibilityReplaceLibraryName(const char *oldName, const char *newName);
void khrIcdVisibilitySetPlatform(char *libraryFile, unsigned index, cl_platform_id platform);

int khrIcdCheckLibraryVisible(const char *libraryFile);
int khrIcdCheckPlatformVisible(const char *libraryFile, unsigned index);
int khrIcdCheckDeviceVisible(cl_platform_id platform, cl_device_id device);

#endif

