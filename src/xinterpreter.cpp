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

extern "C" int init_language();
extern "C" int execute_string(const char *string);

namespace nl = nlohmann;

namespace karilang_kernel {

interpreter::interpreter() { xeus::register_interpreter(this); }

nl::json interpreter::execute_request_impl(int execution_counter,
                                           const std::string &code, bool silent,
                                           bool store_history,
                                           nl::json user_expressions,
                                           bool allow_stdin) {

    nl::json pub_data;
    pub_data["text/plain"] = "Executted???";

    std::cout << "Executting!" << std::endl;

    execute_string(code.c_str());

    publish_execution_result(execution_counter, std::move(pub_data),
                             nl::json::object());

    return xeus::create_successful_reply();
}

void interpreter::configure_impl() { init_language(); }

nl::json interpreter::is_complete_request_impl(const std::string &code) {
    // Insert code here to validate the ``code``
    // and use `create_is_complete_reply` with the corresponding status
    // "unknown", "incomplete", "invalid", "complete"
    return xeus::create_is_complete_reply("complete" /*status*/,
                                          "   " /*indent*/);
}

nl::json interpreter::complete_request_impl(const std::string &code,
                                            int cursor_pos) {

    return xeus::create_complete_reply(nl::json::array(), /*matches*/
                                       cursor_pos,        /*cursor_start*/
                                       cursor_pos         /*cursor_end*/
    );
}

nl::json interpreter::inspect_request_impl(const std::string & /*code*/,
                                           int /*cursor_pos*/,
                                           int /*detail_level*/) {

    return xeus::create_inspect_reply(
        true /*found*/,
        {{std::string("text/plain"), std::string("hello!")}}, /*data*/
        {{std::string("text/plain"), std::string("hello!")}}  /*meta-data*/
    );
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
