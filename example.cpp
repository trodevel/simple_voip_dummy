/*

Example.

Copyright (C) 2016 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 13958 $ $Date:: 2020-10-05 #$ $Author: serge $

#include <iostream>         // cout
#include <typeinfo>
#include <sstream>          // stringstream
#include <fstream>          // std::ifstream
#include <atomic>           // std::atomic
#include <vector>           // std::vector
#include <thread>           // std::thread

#include "dummy.h"              // simple_voip_dummy::Dummy
#include "init_config.h"        // simple_voip_dummy::init_config

#include "simple_voip/objects.h"
#include "simple_voip/str_helper.h"
#include "simple_voip/object_factory.h"         // simple_voip::create_initiate_call_request
#include "utils/dummy_logger.h"     // dummy_log_set_log_level
#include "scheduler/scheduler.h"    // Scheduler

class Callback: virtual public simple_voip::ISimpleVoipCallback
{
public:
    Callback( simple_voip::ISimpleVoip * dummy ):
        dialer_( dummy ),
        call_id_( 0 ),
        last_req_id_( 0 )
    {
    }

    // interface ISimpleVoipCallback
    void consume( const simple_voip::CallbackObject * req )
    {
        std::cout << "got " << *req << std::endl;

        if( typeid( *req ) == typeid( simple_voip::InitiateCallResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::InitiateCallResponse *>( req );

            call_id_    = m->call_id;
        }
        else if( typeid( *req ) == typeid( simple_voip::Failed ) )
        {
            call_id_    = 0;
        }
        else if( typeid( *req ) == typeid( simple_voip::ConnectionLost ) )
        {
            call_id_    = 0;
        }

        delete req;
    }

    void control_thread()
    {
        std::cout << "type exit or quit to quit: " << std::endl;
        std::cout << "call <party>" << std::endl;
        std::cout << "drop" << std::endl;
        std::cout << "play <file>" << std::endl;
        std::cout << "rec <file>" << std::endl;
        std::cout << "stop_play" << std::endl;
        std::cout << "stop_rec" << std::endl;

        std::string input;

        while( true )
        {
            std::cout << "enter command: ";

            std::getline( std::cin, input );

            std::cout << "your input: " << input << std::endl;

            bool b = process_input( input );

            if( b == false )
                break;
        };

        std::cout << "exiting ..." << std::endl;
    }

private:
    bool process_input( const std::string & input )
    {
        std::string cmd;

        std::stringstream stream( input );
        if( stream >> cmd )
        {
            if( cmd == "exit" || cmd == "quit" )
            {
                return false;
            }
            else if( cmd == "call" )
            {
                std::string s;
                stream >> s;

                last_req_id_++;

                dialer_->consume( simple_voip::create_initiate_call_request( last_req_id_, s ) );
            }
            else if( cmd == "drop" )
            {
                last_req_id_++;

                dialer_->consume( simple_voip::create_drop_request( last_req_id_, call_id_ ) );
            }
            else if( cmd == "play" )
            {
                std::string filename;
                stream >> filename;

                last_req_id_++;

                dialer_->consume( simple_voip::create_play_file_request( last_req_id_, call_id_, filename ) );
            }
            else if( cmd == "rec" )
            {
                std::string filename;
                stream >> filename;

                last_req_id_++;

                dialer_->consume( simple_voip::create_record_file_request( last_req_id_, call_id_, filename ) );
            }
            else if( cmd == "stop_play" )
            {
                last_req_id_++;

                dialer_->consume( simple_voip::create_play_file_stop_request( last_req_id_, call_id_ ) );
            }
            else if( cmd == "stop_rec" )
            {
                last_req_id_++;

                dialer_->consume( simple_voip::create_record_file_stop_request( last_req_id_, call_id_ ) );
            }
            else
            {
                std::cout << "ERROR: unknown command '" << cmd << "'" << std::endl;
            }
        }
        else
        {
            std::cout << "ERROR: cannot read command" << std::endl;
        }
        return true;
    }

private:
    simple_voip::ISimpleVoip    * dialer_;

    std::atomic<int>            call_id_;

    uint32_t                    last_req_id_;
};

int main()
{
    simple_voip_dummy::Config config;

    config_reader::ConfigReader cr;

    std::string config_file( "example.ini" );

    cr.init( config_file );

    std::cout << "loaded config file " + config_file << std::endl;

    simple_voip_dummy::init_config( & config, cr );

    simple_voip_dummy::Dummy            dummy;

    Callback test( & dummy );

    std::string error_msg;

    scheduler::Scheduler sched( scheduler::Duration( std::chrono::milliseconds( 1 ) ) );

    auto log_id_dummy       = dummy_logger::register_module( "SimpleVoipDummy" );
    auto log_id_call        = dummy_logger::register_module( "Call" );

    dummy_logger::set_log_level( log_id_dummy,      log_levels_log4j::TRACE );
    dummy_logger::set_log_level( log_id_call,       log_levels_log4j::TRACE );

    bool b = dummy.init( log_id_dummy, log_id_call, config, & test, & sched, & error_msg );
    if( b == false )
    {
        std::cout << "cannot initialize voip module: " << error_msg << std::endl;
        return 0;
    }

    sched.run();
    dummy.start();

    std::vector< std::thread > tg;

    tg.push_back( std::thread( std::bind( &Callback::control_thread, &test ) ) );

    for( auto & t : tg )
        t.join();

    dummy.shutdown();

    sched.shutdown();

    std::cout << "Done! =)" << std::endl;

    return 0;
}
