/***************************************************************************
* Copyright (c) 2023, Vipul Cariappa                                  
*                                                                          
* Distributed under the terms of the MIT license.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <iostream>
#include <memory>


#include <emscripten/bind.h>

#include "src/xinterpreter.hpp"
#include "xeus/xembind.hpp"


EMSCRIPTEN_BINDINGS(my_module) {
    xeus::export_core();
    using interpreter_type = karilang_kernel::interpreter;
    xeus::export_kernel<interpreter_type>("xkernel");
}

