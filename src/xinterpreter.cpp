/***************************************************************************
 * Copyright (c) 2023, Vipul Cariappa
 *
 * Distributed under the terms of the MIT license.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "xeus/xhelper.hpp"
#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"

#include "src/xinterpreter.hpp"

#include "utils.hpp"

extern "C" int init_language();
extern "C" int execute_string(const char *string);
extern "C" char *STDOUT_REDIRECT_STRING;
extern "C" char *STDERR_REDIRECT_STRING;

namespace nl = nlohmann;

namespace karilang_kernel {

interpreter::interpreter() { xeus::register_interpreter(this); }

nl::json interpreter::execute_request_impl(int execution_counter,
                                           const std::string &code, bool silent,
                                           bool store_history,
                                           nl::json user_expressions,
                                           bool allow_stdin) {

    nl::json pub_data;

    STDOUT_REDIRECT_STRING[0] = 0;
    STDERR_REDIRECT_STRING[0] = 0;
    execute_string(code.c_str());

    if (STDERR_REDIRECT_STRING[0]) {
        publish_execution_error("Error", STDERR_REDIRECT_STRING, {});
        return xeus::create_error_reply("Error", STDERR_REDIRECT_STRING);
    } else {
        pub_data["text/plain"] = STDOUT_REDIRECT_STRING;
        publish_execution_result(execution_counter, std::move(pub_data),
                                 nl::json::object());

        return xeus::create_successful_reply();
    }
}

void interpreter::configure_impl() { init_language(); }

nl::json interpreter::is_complete_request_impl(const std::string &code) {
    std::string trimmed_code = trim(code);
    if (trimmed_code.back() == ';')
        return xeus::create_is_complete_reply("complete");
    else
        return xeus::create_is_complete_reply("incomplete", "  ");
}

nl::json interpreter::complete_request_impl(const std::string & /*code*/,
                                            int cursor_pos) {

    return xeus::create_complete_reply(nl::json::array(), cursor_pos,
                                       cursor_pos);
}

nl::json interpreter::inspect_request_impl(const std::string & /*code*/,
                                           int /*cursor_pos*/,
                                           int /*detail_level*/) {

    return xeus::create_inspect_reply();
}

void interpreter::shutdown_request_impl() { std::cout << "Bye!!" << std::endl; }

nl::json interpreter::kernel_info_request_impl() {

    const std::string protocol_version = "5.3";
    const std::string implementation = "xkarilang";
    const std::string implementation_version = KARILANG_VERSION;
    const std::string language_name = "KariLang";
    const std::string language_version = "0.0.1";
    const std::string language_mimetype = "text/x-KariLang";
    ;
    const std::string language_file_extension = "kari";
    ;
    const std::string language_pygments_lexer = "";
    const std::string language_codemirror_mode = "";
    const std::string language_nbconvert_exporter = "";
    const std::string banner = "xkarilang";
    const bool debugger = true;
    const nl::json help_links = nl::json::array();

    return xeus::create_info_reply(
        protocol_version, implementation, implementation_version, language_name,
        language_version, language_mimetype, language_file_extension,
        language_pygments_lexer, language_codemirror_mode,
        language_nbconvert_exporter, banner, debugger, help_links);
}

} // namespace karilang_kernel
