/*

Call.

Copyright (C) 2018 Sergey Kolevatov

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

// $Revision: 9416 $ $Date:: 2018-06-20 #$ $Author: serge $

#include "call.h"                   // self

#include <map>                              // std::map

#include "simple_voip/objects.h"            // InitiateCallResponse
#include "simple_voip/object_factory.h"     // create_error_response
#include "utils/dummy_logger.h"             // dummy_log
#include "scheduler/onetime_job_aux.h"      // create_and_insert_one_time_job

#include "random.h"                         // get_random

namespace simple_voip_dummy {

uint32_t epoch_now()
{
    auto tm_now = std::chrono::system_clock::now();

    auto tm_epoch = std::chrono::duration_cast<std::chrono::hours>( tm_now.time_since_epoch() ).count();

    return tm_epoch;
}


Call::Call(
        uint32_t                            call_id,
        unsigned int                        log_id,
        const Config                        & config,
        Dummy                               * parent,
        simple_voip::ISimpleVoipCallback    * callback,
        scheduler::IScheduler               * scheduler ):
                call_id_( call_id ),
                log_id_( log_id ),
                state_( state_e::IDLE ),
                config_( config ),
                parent_( parent ),
                callback_( callback ),
                scheduler_( scheduler )
{
}

Call::~Call()
{
}

void Call::handle( const simple_voip::InitiateCallRequest & req )
{
    auto resp = simple_voip::create_initiate_call_response( req.req_id, call_id_ );

    callback_->consume( resp );

    auto ev = simple_voip::create_message_t<simple_voip::Dialing>( call_id_ );

    auto exec_time = calc_exec_time( config_.dialing_duration_min, config_.dialing_duration_max );

    schedule_event( ev, exec_time );

    next_state( state_e::DIALING );
}

void Call::handle( const simple_voip::DropRequest & req )
{
    auto has_to_execute = get_true( config_.drop_ok_response_probability );

    if( has_to_execute )
    {
        auto resp = simple_voip::create_drop_response( req.req_id );

        next_state( state_e::CLOSED );
    }
    else
    {
        auto has_to_error = get_true( config_.drop_err_not_rej_response_probability );

        dummy_log_info( log_id_, "not accepted: is rejection %u", (int) has_to_error );

        if( has_to_error )
            parent_->send_error_response( req.req_id, "error" );
        else
            parent_->send_reject_response( req.req_id, "rejected" );
    }
}

void Call::handle( const simple_voip::PlayFileRequest & req )
{
}

void Call::handle( const simple_voip::PlayFileStopRequest & req )
{
}

void Call::handle( const simple_voip::RecordFileRequest & req )
{
}

void Call::handle( const simple_voip::RecordFileStopRequest & req )
{
}

void Call::next_state( state_e state )
{
    dummy_logi_debug( log_id_, call_id_, "switched state %s --> %s", to_string( state_ ).c_str(), to_string( state ).c_str() );

    state_  = state;
}

#define TUPLE_VAL_STR(_x_)  _x_,#_x_

const std::string & Call::to_string( const state_e & l )
{
    typedef state_e Type;
    typedef std::map< Type, std::string > Map;
    static Map m =
    {
        { Type:: TUPLE_VAL_STR( IDLE ) },
        { Type:: TUPLE_VAL_STR( DIALING ) },
        { Type:: TUPLE_VAL_STR( RINGING ) },
        { Type:: TUPLE_VAL_STR( CONNECTED ) },
        { Type:: TUPLE_VAL_STR( CLOSED ) },
    };

    auto it = m.find( l );

    static const std::string undef( "???" );

    if( it == m.end() )
        return undef;

    return it->second;
}

void Call::schedule_event( simple_voip::CallbackObject * ev, uint32_t exec_time )
{
    std::string error_msg;

    scheduler::job_id_t sched_job_id;

    auto b = scheduler::create_and_insert_one_time_job(
            & sched_job_id,
            error_msg,
            * scheduler_,
            "timer_job",
            exec_time,
            std::bind( &Dummy::consume, this, ev ) );

    if( b == false )
    {
        dummy_logi_error( log_id_, call_id_, "cannot set timer: %s", error_msg.c_str() );
    }
}

uint32_t Call::calc_exec_time( uint32_t min, uint32_t max )
{
    auto dur = get_random( min, max );

    auto now = epoch_now();

    auto exec_time = now + dur;

    return exec_time;
}

} // namespace simple_voip_dummy
