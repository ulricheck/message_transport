//
// Created by Ulrich Eck on 07/05/16.
//

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include "message_transport/logging.h"
#include "message_transport/common/publisher.h"
#include "message_transport/sharedmem/SharedMemoryBlock.h"
#include "message_transport/serialization/message_types.h"


namespace pt = boost::property_tree;
namespace mt = message_transport;
namespace mts = message_transport::serialization;

int main(int argc, char **argv)
{
    std::string segment_name = MSGTSharedMemoryDefaultBlock;
    int segment_size = 1000000;
    int num_messages = 100;

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

    LOG_INFO("Create Publisher - segment: " << segment_name);
    mt::Publisher pub(config);
    LOG_INFO("Initialize to topic: test_message");
    pub.do_initialise<mts::TestMessage>("test_message");

    int send_count = 0;
    while (send_count < num_messages) {
        mts::TestMessage msg;
        msg.value = send_count;
        LOG_INFO("Sending message: " << send_count);
        pub.publish(msg);
        boost::this_thread::sleep_for( boost::chrono::milliseconds(500) );
        send_count++;
    }

    LOG_INFO("Shutdown Publisher");
    pub.shutdown();

    return 0;
}