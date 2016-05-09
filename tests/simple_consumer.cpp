//
// Created by Ulrich Eck on 07/05/16.
//

#include <iostream>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include "message_transport/logging.h"
#include "message_transport/common/subscriber.h"
#include "message_transport/sharedmem/SharedMemoryBlock.h"
#include "message_transport/serialization/message_types.h"


namespace pt = boost::property_tree;
namespace mt = message_transport;
namespace mts = message_transport::serialization;


int receive_count = 0;

void test_callback(const mts::TestMessageConstPtr& m) {
    std::cout << "received: " << m->value << std::endl;
    receive_count++;
}



int main(int argc, char **argv)
{
    std::string segment_name = MSGTSharedMemoryDefaultBlock;
    int segment_size = 1000000;

    try {

        // describe program options
        namespace po = boost::program_options;
        po::options_description poDesc("Allowed options", 80);
        poDesc.add_options()
                ("help", "print this help message")
                ("size", po::value<int>(&segment_size), "Segment Size")
                ("name", po::value<std::string>(&segment_name), "Segment Name");

        // specify default options
        po::positional_options_description inputOptions;
        //inputOptions.add( "utql", 1 );

        // parse options from command line and environment
        po::variables_map poOptions;
        po::store(po::command_line_parser(argc, argv).options(poDesc).positional(inputOptions).run(), poOptions);
        po::store(po::parse_environment(poDesc, "MSGT_DEFAULTS_"), poOptions);
        po::notify(poOptions);

        // print help message if nothing specified
        if (poOptions.count("help")) {
            std::cout << "Syntax: msgt_manager [options] " << std::endl << std::endl;
            std::cout << poDesc << std::endl;
            return 0;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error parsing command line parameters : " << e.what() << std::endl;
        std::cerr << "Try msgt_manager --help for help" << std::endl;
        return 1;
    }

    LOG_INFO("Create Subscriber config");
    pt::ptree e1, e2;
    boost::shared_ptr<pt::ptree> config(new pt::ptree);

    e1.put<std::string>("", segment_name);
    e2.put<int>("", segment_size);
    config->put_child("segment_name", e1);
    config->put_child("segment_size", e2);

    LOG_INFO("Create Subscriber - segment: " << segment_name);
    mt::Subscriber sub(config);
    LOG_INFO("Subscribe to topic: test_message");
    boost::function< void ( const mts::TestMessageConstPtr& ) > cb;
    cb = &test_callback;
    sub.do_subscribe<mts::TestMessage>("test_message", "sharedmem", 1, cb);

    std::cout << "press enter to exit" << std::endl;
    std::cin.ignore();

    LOG_INFO("Shutdown Subscriber");
    sub.shutdown();

    return 0;
}