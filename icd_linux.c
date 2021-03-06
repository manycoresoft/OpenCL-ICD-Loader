/*
 * Copyright (c) 2016 The Khronos Group Inc.
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

#include "icd.h"
#include "icd_envvar.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>

static pthread_once_t initialized = PTHREAD_ONCE_INIT;

/*
 * 
 * Vendor enumeration functions
 *
 */

// go through the list of vendors in the two configuration files
void khrIcdOsVendorsEnumerate(void)
{
    DIR *dir = NULL;
    struct dirent *dirEntry = NULL;
#ifdef __ANDROID__
    char *vendorPath = "/system/vendor/Khronos/OpenCL/vendors/";
#else
    char *vendorPath = "/etc/OpenCL/vendors/";
#endif // ANDROID

    // open the directory
    dir = opendir(vendorPath);
    if (NULL == dir) 
    {
        KHR_ICD_TRACE("Failed to open path %s\n", vendorPath);
        goto Cleanup;
    }

    // attempt to load all files in the directory
    for (dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir) )
    {
        switch(dirEntry->d_type)
        {
        case DT_UNKNOWN:
        case DT_REG:
        case DT_LNK:
            {
                const char* extension = ".icd";
                FILE *fin = NULL;
                char* fileName = NULL;
                char* buffer = NULL;
                long bufferSize = 0;

                // make sure the file name ends in .icd
                if (strlen(extension) > strlen(dirEntry->d_name) )
                {
                    break;
                }
                if (strcmp(dirEntry->d_name + strlen(dirEntry->d_name) - strlen(extension), extension) ) 
                {
                    break;
                }

                // allocate space for the full path of the vendor library name
                fileName = malloc(strlen(dirEntry->d_name) + strlen(vendorPath) + 1);
                if (!fileName) 
                {
                    KHR_ICD_TRACE("Failed allocate space for ICD file path\n");
                    break;
                }
                sprintf(fileName, "%s%s", vendorPath, dirEntry->d_name);

                // open the file and read its contents
                fin = fopen(fileName, "r");
                if (!fin)
                {
                    free(fileName);
                    break;
                }
                fseek(fin, 0, SEEK_END);
                bufferSize = ftell(fin);

                buffer = malloc(bufferSize+1);
                if (!buffer)
                {
                    free(fileName);
                    fclose(fin);
                    break;
                }                
                memset(buffer, 0, bufferSize+1);
                fseek(fin, 0, SEEK_SET);                       
                if (bufferSize != (long)fread(buffer, 1, bufferSize, fin) )
                {
                    free(fileName);
                    free(buffer);
                    fclose(fin);
                    break;
                }
                // ignore a newline at the end of the file
                if (buffer[bufferSize-1] == '\n') buffer[bufferSize-1] = '\0';

                khrIcdVisibilityReplaceLibraryName(dirEntry->d_name, buffer);
                khrIcdVisibilityReplaceLibraryName(fileName, buffer);

                // load the string read from the file
                if (khrIcdCheckLibraryVisible(buffer))
                {
                    khrIcdVendorAdd(buffer);
                }
                
                free(fileName);
                free(buffer);
                fclose(fin);
            }
            break;
        default:
            break;
        }
    }

Cleanup:

    // free resources and exit
    if (dir) 
    {
        closedir(dir);
    }
}

// go through the list of vendors only once
void khrIcdOsVendorsEnumerateOnce(void)
{
    pthread_once(&initialized, khrIcdOsVendorsEnumerate);
}

/*
 * 
 * Dynamic library loading functions
 *
 */

// dynamically load a library.  returns NULL on failure
void *khrIcdOsLibraryLoad(const char *libraryName)
{
    return dlopen (libraryName, RTLD_NOW);
}

// get a function pointer from a loaded library.  returns NULL on failure.
void *khrIcdOsLibraryGetFunctionAddress(void *library, const char *functionName)
{
    return dlsym(library, functionName);
}

// unload a library
void khrIcdOsLibraryUnload(void *library)
{
    dlclose(library);
}

