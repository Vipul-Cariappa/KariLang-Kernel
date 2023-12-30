/***************************************************************************
* Copyright (c) 2023, Vipul Cariappa                                  
*                                                                          
* Distributed under the terms of the MIT license.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/


#ifndef KARILANG_INTERPRETER_HPP
#define KARILANG_INTERPRETER_HPP

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "config.hpp"
#include "xeus/xinterpreter.hpp"


namespace nl = nlohmann;

namespace karilang_kernel
{
    class KARILANG_API interpreter : public xeus::xinterpreter
    {
    public:

        interpreter();
        virtual ~interpreter() = default;

    protected:

        void configure_impl() override;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
