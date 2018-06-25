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

// $Revision: 9466 $ $Date:: 2018-06-25 #$ $Author: serge $

#include <iostream>         // cout
#include <typeinfo>
#include <sstream>          // stringstream
#include <fstream>          // std::ifstream
#include <atomic>           // std::atomic
#include <vector>           // std::vector
#include <thread>           // std::thread

#include "dummy.h"              // simple_voip_dummy::Dummy
#include "config.h"             // Config

#include "simple_voip/objects.h"
#include "utils/dummy_logger.h"     // dummy_log_set_log_level
#include "scheduler/scheduler.h"    // Scheduler
#include "config_reader/config_reader.h"    // config_reader::ConfigReader

#define GET_VALUE( _v, _s, _toe )               cr.get_value( & cfg-> _v, _s, #_v, _toe )
#define GET_VALUE_CONVERTED( _v, _s, _toe )     cr.get_value_converted( & cfg-> _v, _s, #_v, _toe )

void init_config( simple_voip_dummy::Config * cfg, const config_reader::ConfigReader & cr )
{
    const std::string section( "simple_voip_dummy" );

    GET_VALUE_CONVERTED( icr_ok_response_probability,       section, true );
    GET_VALUE_CONVERTED( icr_reject_response_probability,   section, true );
    GET_VALUE_CONVERTED( drop_ok_response_probability,      section, true );
    GET_VALUE_CONVERTED( drop_err_not_rej_response_probability, section, true );

    GET_VALUE_CONVERTED( pf_ok_response_probability,            section, true );
    GET_VALUE_CONVERTED( pf_err_not_rej_response_probability,   section, true );

    GET_VALUE_CONVERTED( pfs_ok_response_probability,           section, true );
    GET_VALUE_CONVERTED( pfs_err_not_rej_response_probability,  section, true );

    GET_VALUE_CONVERTED( rf_ok_response_probability,            section, true );
    GET_VALUE_CONVERTED( rf_err_not_rej_response_probability,   section, true );

    GET_VALUE_CONVERTED( rfs_ok_response_probability,           section, true );
    GET_VALUE_CONVERTED( rfs_err_not_rej_response_probability,  section, true );

    GET_VALUE_CONVERTED( connected_probability,                 section, true );
    GET_VALUE_CONVERTED( waiting_dialing_duration_min,          section, true );
    GET_VALUE_CONVERTED( waiting_dialing_duration_max,          section, true );
    GET_VALUE_CONVERTED( dialing_duration_min,                  section, true );
    GET_VALUE_CONVERTED( dialing_duration_max,                  section, true );
    GET_VALUE_CONVERTED( ringing_duration_min,                  section, true );
    GET_VALUE_CONVERTED( ringing_duration_max,                  section, true );
    GET_VALUE_CONVERTED( call_duration_min,                     section, true );
    GET_VALUE_CONVERTED( call_duration_max,                     section, true );

    GET_VALUE_CONVERTED( next_dtmf_delay_min,                   section, true );
    GET_VALUE_CONVERTED( next_dtmf_delay_max,                   section, true );
    GET_VALUE_CONVERTED( dtmf_min,                              section, true );
    GET_VALUE_CONVERTED( dtmf_max,                              section, true );

}

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
        std::cout << "got " << typeid( *req ).name() << " ";

        if( typeid( *req ) == typeid( simple_voip::InitiateCallResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::InitiateCallResponse *>( req );

            std::cout << m->call_id << std::endl;

            call_id_    = m->call_id;
        }
        else if( typeid( *req ) == typeid( simple_voip::ErrorResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::ErrorResponse *>( req );

            std::cout << m->descr << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::RejectResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::RejectResponse *>( req );

            std::cout << m->descr << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::DropResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::DropResponse *>( req );

            std::cout << m->req_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::Failed ) )
        {
            auto m = dynamic_cast< const simple_voip::Failed *>( req );

            std::cout << m->call_id << std::endl;

            call_id_    = 0;
        }
        else if( typeid( *req ) == typeid( simple_voip::ConnectionLost ) )
        {
            auto m = dynamic_cast< const simple_voip::ConnectionLost *>( req );

            std::cout << m->call_id << std::endl;

            call_id_    = 0;
        }
        else if( typeid( *req ) == typeid( simple_voip::Dialing ) )
        {
            auto m = dynamic_cast< const simple_voip::Dialing *>( req );

            std::cout << m->call_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::Ringing ) )
        {
            auto m = dynamic_cast< const simple_voip::Ringing *>( req );

            std::cout << m->call_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::Connected ) )
        {
            auto m = dynamic_cast< const simple_voip::Connected *>( req );

            std::cout << m->call_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::CallDuration ) )
        {
            auto m = dynamic_cast< const simple_voip::CallDuration *>( req );

            std::cout << m->call_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::DtmfTone ) )
        {
            auto m = dynamic_cast< const simple_voip::DtmfTone *>( req );

            std::cout << "got " << typeid( *req ).name() << static_cast<uint16_t>( m->tone ) << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::PlayFileResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::PlayFileResponse *>( req );

            std::cout << m->req_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::PlayFileStopResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::PlayFileStopResponse *>( req );

            std::cout << m->req_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::RecordFileResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::RecordFileResponse *>( req );

            std::cout << m->req_id << std::endl;
        }
        else if( typeid( *req ) == typeid( simple_voip::RecordFileStopResponse ) )
        {
            auto m = dynamic_cast< const simple_voip::RecordFileStopResponse *>( req );

            std::cout << m->req_id << std::endl;
        }
        else
        {
            std::cout << "unknown" << std::endl;
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

                auto req = new simple_voip::InitiateCallRequest;

                req->req_id = last_req_id_;
                req->party  = s;

                dialer_->consume( req );
            }
            else if( cmd == "drop" )
            {
                last_req_id_++;

                auto req = new simple_voip::DropRequest;

                req->req_id     = last_req_id_;
                req->call_id    = call_id_;

                dialer_->consume( req );
            }
            else if( cmd == "play" )
            {
                std::string filename;
                stream >> filename;

                last_req_id_++;

                auto req = new simple_voip::PlayFileRequest;

                req->req_id     = last_req_id_;
                req->call_id    = call_id_;
                req->filename   = filename;

                dialer_->consume( req );
            }
            else if( cmd == "rec" )
            {
                std::string filename;
                stream >> filename;

                last_req_id_++;

                auto req = new simple_voip::RecordFileRequest;

                req->req_id     = last_req_id_;
                req->call_id    = call_id_;
                req->filename   = filename;

                dialer_->consume( req );
            }
            else if( cmd == "stop_play" )
            {
                last_req_id_++;

                auto req = new simple_voip::PlayFileStopRequest;

                req->req_id     = last_req_id_;
                req->call_id    = call_id_;

                dialer_->consume( req );
            }
            else if( cmd == "stop_rec" )
            {
                last_req_id_++;

                auto req = new simple_voip::RecordFileStopRequest;

                req->req_id     = last_req_id_;
                req->call_id    = call_id_;

                dialer_->consume( req );
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

    if( cr.init( config_file ) == false )
    {
        std::cerr << "ERROR: cannot read config file " + config_file << std::endl;

        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "loaded config file " + config_file << std::endl;
    }

    init_config( & config, cr );

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
