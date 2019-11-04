////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
// Oliveira
//          (Advisor: Ricardo Reis)
//
// BSD 3-Clause License
//
// Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include "Parameters.h"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

Parameters::Parameters(int argc, char** argv) {
        namespace po = boost::program_options;
        po::options_description dscp("Usage");
        // clang-format off
        dscp.add_options()
                ("interactive,c"        , po::value<bool>()            , "Enables interactive mode")
                ("input-lef,l"          , po::value<std::string>()     , "Input LEF file (mandatory)")
                ("input-def,d"          , po::value<std::string>()     , "Input DEF file (mandatory)")
                ("output,o"             , po::value<std::string>()     , "Output Guide file (mandatory)")
                ("adjust,a"             , po::value<float>()           , "Global capacity adjustment (float) (optional)")
                ("min-layer,n"          , po::value<int>()             , "Min routing layer (int) (optional)")
                ("max-layer,m"          , po::value<int>()             , "Max routing layer (int) (optional)")
                ("unidir-route,u"       , po::value<bool>()             , "Unidirectional route (bool) (optional)")
                ;
        // clang-format on

        po::variables_map vm;
        try {
                po::store(
                    po::command_line_parser(argc, argv)
                        .options(dscp)
                        .style(po::command_line_style::unix_style |
                               po::command_line_style::allow_long_disguise)
                        .run(),
                    vm);
                po::notify(vm);

                if (vm.count("help") || ( !vm.count("interactive") && (
                    !vm.count("output") || !vm.count("input-lef") || !vm.count("input-def")))) {
                        std::cout << "\n" << dscp;
                        std::exit(1);
                }

                if (vm.count("input-lef")) {
                        _inputLefFile = vm["input-lef"].as<std::string>();
                }
                if (vm.count("input-def")) {
                        _inputDefFile = vm["input-def"].as<std::string>();
                }
                if (vm.count("output")) {
                        _outputFile = vm["output"].as<std::string>();
                }
                if (vm.count("adjust")) {
                        _adjustment = vm["adjust"].as<float>();
                }
                if (vm.count("min-layer")) {
                        _minRoutingLayer = vm["min-layer"].as<int>();
                }
                if (vm.count("max-layer")) {
                        _maxRoutingLayer = vm["max-layer"].as<int>();
                }
                if (vm.count("unidir-route")) {
                        _unidirectionalRoute = vm["unidir-route"].as<bool>();
                }
                if (vm.count("interactive")) {
                        _interactiveMode = vm["interactive"].as<bool>();
                }
        } catch (const po::error& ex) {
                std::cerr << ex.what() << '\n';
        }

        if (!isInteractiveMode())
        {
                printAll();
        }
}

void Parameters::printAll() const {
        // clang-format off
        std::cout << "\nOptions: \n";
        std::cout << std::setw(20) << std::left << "Input LEF file: ";
        std::cout << _inputLefFile << "\n";
        std::cout << std::setw(20) << std::left << "Input DEF file: ";
        std::cout << _inputDefFile << "\n";
        std::cout << std::setw(20) << std::left << "Output Guide file: ";
        std::cout << _outputFile << "\n";
        std::cout << std::setw(20) << std::left << "Global capacity adjustment: ";
        std::cout << _adjustment << "\n";
        std::cout << std::setw(20) << std::left << "Min routing layer: ";
        std::cout << _minRoutingLayer << "\n";
        std::cout << std::setw(20) << std::left << "Max routing layer: ";
        std::cout << _maxRoutingLayer << "\n";
        std::cout << std::setw(20) << std::left << "Unidirectional route: ";
        std::cout << _unidirectionalRoute << "\n";

        std::cout << "\n";
        // clang-format on
}