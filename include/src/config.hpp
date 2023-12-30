/***************************************************************************
* Copyright (c) 2023, Vipul Cariappa                                  
*                                                                          
* Distributed under the terms of the MIT license.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#ifndef KARILANG_CONFIG_HPP
#define KARILANG_CONFIG_HPP

// Project version
#define KARILANG_VERSION_MAJOR 0
#define KARILANG_VERSION_MINOR 1
#define KARILANG_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define KARILANG_CONCATENATE(A, B) KARILANG_CONCATENATE_IMPL(A, B)
#define KARILANG_CONCATENATE_IMPL(A, B) A##B
#define KARILANG_STRINGIFY(a) KARILANG_STRINGIFY_IMPL(a)
#define KARILANG_STRINGIFY_IMPL(a) #a

#define KARILANG_VERSION KARILANG_STRINGIFY(KARILANG_CONCATENATE(KARILANG_VERSION_MAJOR,   \
                 KARILANG_CONCATENATE(.,KARILANG_CONCATENATE(KARILANG_VERSION_MINOR,   \
                                  KARILANG_CONCATENATE(.,KARILANG_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef KARILANG_EXPORTS
        #define KARILANG_API __declspec(dllexport)
    #else
        #define KARILANG_API __declspec(dllimport)
    #endif
#else
    #define KARILANG_API
#endif

#endif