/*
soco2root - Convert soco2 event files to root
https://gitlab.ikp.uni-koeln.de/jmayer/soco2root
Copyright (C) 2017  Jan Mayer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
namespace po = boost::program_options;

#include "FSUtils.h"
#include "Soco2Root.h"

using asio_service = boost::asio::io_service;
using asio_worker  = std::unique_ptr<asio_service::work>;
struct ThreadPool
{
    ThreadPool(size_t threads)
        : service()
        , working(new asio_worker::element_type(service))
    {
        for (std::size_t i = 0; i < threads; ++i)
        {
            auto worker = boost::bind(&asio_service::run, &(this->service));
            group.add_thread(new boost::thread(worker));
        }
    }

    template <class F>
    void enqueue(F f)
    {
        service.post(f);
    }

    ~ThreadPool()
    {
        working.reset();
        group.join_all();
        service.stop();
    }

    private:
    asio_service service;
    asio_worker working;
    boost::thread_group group;
};

std::string getOutputFilename(const std::string& input, const po::variables_map& vm)
{

    if (vm.count("output-dir"))
    {
        return SOCO::FSUtils::buildFilename(SOCO::FSUtils::basename(input),
                                            vm["output-dir"].as<std::string>(),
                                            ".root");
    }
    else
    {
        return SOCO::FSUtils::stripExtension(input) + ".root";
    }
}

int main(int ac, char* av[])
{
    try
    {
        po::options_description desc("soco2root");
        // clang-format off
        desc.add_options()
            ("help,h", "Display this help message")
            ("version,v", "Display the version number")
            ("threads,t", po::value<int>()->default_value(1), "Number of threads")
            ("output-dir,o", po::value<std::string>(), "Output directory. If not set, input file location is used")
            ("input-files", po::value<std::vector<std::string>>(), "Input files");
        // clang-format on

        po::positional_options_description p;
        p.add("input-files", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc;
            return 0;
        }
        if (vm.count("version"))
        {
            std::cout << "soco2root version 1.0" << std::endl;
            return 0;
        }

        if (vm.count("input-files"))
        {
            const std::vector<std::string> files = vm["input-files"].as<std::vector<std::string>>();
            const int threads                    = vm["threads"].as<int>();

            if (threads > 1)
            {
                std::cout << "Using thread pool with " << threads << " threads." << std::endl;
                ThreadPool pool(threads);
                for (const std::string& input : files)
                {
                    pool.enqueue([=]() {
                        Soco2Root s2r(input, getOutputFilename(input, vm));
                        s2r.process();
                    });
                }
            }
            else
            {
                std::cout << "No multithreading." << std::endl;
                for (const std::string& input : files)
                {
                    Soco2Root s2r(input, getOutputFilename(input, vm));
                    s2r.process();
                }
            }
        }
        else
        {
            throw std::runtime_error("Not Input files!");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
