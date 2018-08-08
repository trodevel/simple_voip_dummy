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

// $Revision: 9638 $ $Date:: 2018-08-08 #$ $Author: serge $

#include "call.h"                   // self

#include <map>                              // std::map

#include "simple_voip/objects.h"            // InitiateCallResponse
#include "simple_voip/object_factory.h"     // create_error_response
#include "utils/dummy_logger.h"             // dummy_log
#include "scheduler/timeout_job_aux.h"      // create_and_insert_timeout_job

#include "random.h"                         // get_random
#include "dummy.h"                          // Dummy

namespace simple_voip_dummy {

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
                player_state_( media_state_e::IDLE ),
                recorder_state_( media_state_e::IDLE ),
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

    auto duration = get_random( config_.waiting_dialing_duration_min, config_.waiting_dialing_duration_max );

    schedule_event( ev, duration, "Dialing" );

    next_state( state_e::WAITING_DIALING );
}

void Call::handle( const simple_voip::DropRequest & req )
{
    if( ! ( state_ == state_e::WAITING_DIALING ||
            state_ == state_e::DIALING ||
            state_ == state_e::RINGING ||
            state_ == state_e::CONNECTED
            ) )
    {
        parent_->send_error_response( req.req_id, "no initated call" );
        return;
    }

    auto has_to_execute = execute_req_or_reject(
            req.req_id,
            config_.drop_ok_response_probability,
            config_.drop_err_not_rej_response_probability,
            "DropRequest" );

    if( has_to_execute == false )
        return;

    auto resp = simple_voip::create_drop_response( req.req_id );

    callback_->consume( resp );

    player_next_state( media_state_e::IDLE );
    recorder_next_state( media_state_e::IDLE );

    next_state( state_e::CLOSED );
}

void Call::handle( const simple_voip::PlayFileRequest & req )
{
    if( state_ != state_e::CONNECTED )
    {
        parent_->send_error_response( req.req_id, "not connected" );
        return;
    }

    if( player_state_ != media_state_e::IDLE )
    {
        parent_->send_error_response( req.req_id, "already playing" );
        return;
    }

    auto has_to_execute = execute_req_or_reject(
            req.req_id,
            config_.pf_ok_response_probability,
            config_.pf_err_not_rej_response_probability,
            "PlayFileRequest" );

    if( has_to_execute == false )
        return;

    auto resp = simple_voip::create_play_file_response( req.req_id );

    callback_->consume( resp );

    player_next_state( media_state_e::BUSY );
}

void Call::handle( const simple_voip::PlayFileStopRequest & req )
{
    if( state_ != state_e::CONNECTED )
    {
        parent_->send_error_response( req.req_id, "not connected" );
        return;
    }

    if( player_state_ != media_state_e::BUSY )
    {
        parent_->send_error_response( req.req_id, "already idle" );
        return;
    }

    auto has_to_execute = execute_req_or_reject(
            req.req_id,
            config_.pfs_ok_response_probability,
            config_.pfs_err_not_rej_response_probability,
            "PlayFileStopRequest" );

    if( has_to_execute == false )
        return;

    auto resp = simple_voip::create_play_file_stop_response( req.req_id );

    callback_->consume( resp );

    player_next_state( media_state_e::IDLE );
}

void Call::handle( const simple_voip::RecordFileRequest & req )
{
}

void Call::handle( const simple_voip::RecordFileStopRequest & req )
{
}


void Call::handle( const simple_voip::Dialing & req )
{
    if( state_ != state_e::WAITING_DIALING )
    {
        delete & req;
        return;
    }

    auto ev = simple_voip::create_message_t<simple_voip::Ringing>( call_id_ );

    auto duration = get_random( config_.dialing_duration_min, config_.dialing_duration_max );

    schedule_event( ev, duration, "Ringing" );

    next_state( state_e::DIALING );
}

void Call::handle( const simple_voip::Ringing & req )
{
    if( state_ != state_e::DIALING )
    {
        delete & req;
        return;
    }

    auto has_to_exec = get_true( config_.connected_probability );

    auto ev = ( has_to_exec )?
            (simple_voip::CallbackObject *)simple_voip::create_message_t<simple_voip::Connected>( call_id_ ):
            (simple_voip::CallbackObject *)simple_voip::create_message_t<simple_voip::Failed>( call_id_ );

    auto duration = get_random( config_.ringing_duration_min, config_.ringing_duration_max );

    std::string s_c( "Connected" );
    std::string s_f( "Failed" );

    schedule_event( ev, duration, has_to_exec ? s_c : s_f );

    next_state( state_e::RINGING );
}

void Call::handle( const simple_voip::Failed & req )
{
    if( state_ != state_e::RINGING )
    {
        delete & req;
        return;
    }

    callback_->consume( & req );

    next_state( state_e::CLOSED );
}

void Call::handle( const simple_voip::Connected & req )
{
    if( state_ != state_e::RINGING )
    {
        delete & req;
        return;
    }

    callback_->consume( & req );

    auto ev = simple_voip::create_message_t<simple_voip::ConnectionLost>( call_id_ );

    auto duration = get_random( config_.call_duration_min, config_.call_duration_max );

    schedule_event( ev, duration, "ConnectionLost" );

    schedule_dtmf_tone();

    next_state( state_e::CONNECTED );
}

void Call::handle( const simple_voip::ConnectionLost & req )
{
    if( state_ != state_e::CONNECTED )
    {
        delete & req;
        return;
    }

    callback_->consume( & req );

    player_next_state( media_state_e::IDLE );
    recorder_next_state( media_state_e::IDLE );

    next_state( state_e::CLOSED );
}

void Call::handle( const simple_voip::DtmfTone & req )
{
    if( state_ != state_e::CONNECTED )
    {
        delete & req;
        return;
    }

    callback_->consume( & req );

    schedule_dtmf_tone();
}

void Call::next_state( state_e state )
{
    dummy_logi_info( log_id_, call_id_, "switched state %s --> %s", to_string( state_ ).c_str(), to_string( state ).c_str() );

    state_  = state;
}

void Call::player_next_state( media_state_e state )
{
    dummy_logi_debug( log_id_, call_id_, "switched player state %s --> %s", to_string( player_state_ ).c_str(), to_string( state ).c_str() );

    player_state_  = state;
}

void Call::recorder_next_state( media_state_e state )
{
    dummy_logi_debug( log_id_, call_id_, "switched recorder state %s --> %s", to_string( recorder_state_ ).c_str(), to_string( state ).c_str() );

    recorder_state_  = state;
}

#define TUPLE_VAL_STR(_x_)  _x_,#_x_

const std::string & Call::to_string( const state_e & l )
{
    typedef state_e Type;
    typedef std::map< Type, std::string > Map;
    static Map m =
    {
        { Type:: TUPLE_VAL_STR( IDLE ) },
        { Type:: TUPLE_VAL_STR( WAITING_DIALING ) },
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

const std::string & Call::to_string( const media_state_e & l )
{
    typedef media_state_e Type;
    typedef std::map< Type, std::string > Map;
    static Map m =
    {
        { Type:: TUPLE_VAL_STR( IDLE ) },
        { Type:: TUPLE_VAL_STR( BUSY ) },
    };

    auto it = m.find( l );

    static const std::string undef( "???" );

    if( it == m.end() )
        return undef;

    return it->second;
}

void Call::schedule_event( const simple_voip::CallbackObject * ev, uint32_t duration, const std::string & descr )
{
    dummy_logi_trace( log_id_, call_id_, "schedule_event: %u %s", duration, descr.c_str() );

    std::string error_msg;

    scheduler::job_id_t sched_job_id;

    auto b = scheduler::create_and_insert_timeout_job(
            & sched_job_id,
            & error_msg,
            * scheduler_,
            "timer_job",
            scheduler::Duration( duration ),
            std::bind( static_cast<void (Dummy::*)(const simple_voip::CallbackObject * )>(&Dummy::consume), parent_, ev ) );

    if( b == false )
    {
        dummy_logi_error( log_id_, call_id_, "cannot set timer: %s", error_msg.c_str() );
    }
    else
    {
        dummy_logi_debug( log_id_, call_id_, "scheduled execution in: %u sec", duration );
    }
}

void Call::schedule_dtmf_tone()
{
    auto tone_int = get_random( config_.dtmf_min, config_.dtmf_max );

    auto tone = static_cast<simple_voip::DtmfTone::tone_e>( tone_int );

    auto ev = simple_voip::create_dtmf_tone( call_id_, tone );

    auto duration = get_random( config_.next_dtmf_delay_min, config_.next_dtmf_delay_max );

    schedule_event( ev, duration, "DtmfTone" );
}

bool Call::execute_req_or_reject( uint32_t req_id, uint32_t ok_prob, uint32_t err_not_rej_prob, const std::string & comment )
{
    auto has_to_execute = get_true( ok_prob );

    if( has_to_execute )
        return true;

    auto has_to_error = get_true( err_not_rej_prob );

    dummy_log_info( log_id_, "not accepted: is rejection %u", (int) has_to_error );

    if( has_to_error )
        parent_->send_error_response( req_id, "error" );
    else
        parent_->send_reject_response( req_id, "rejected" );

    return false;
}

bool Call::is_ended() const
{
    return state_ == state_e::CLOSED;
}

} // namespace simple_voip_dummy
