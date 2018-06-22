/*

Dummy.

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

// $Revision: 9442 $ $Date:: 2018-06-21 #$ $Author: serge $

#include "dummy.h"                  // self

#include <typeindex>                // std::type_index
#include <typeinfo>
#include <unordered_map>
#include <cassert>                  // assert

#include "simple_voip/objects.h"            // InitiateCallResponse
#include "simple_voip/object_factory.h"     // create_error_response
#include "utils/dummy_logger.h"             // dummy_log

#include "random.h"                         // get_true

namespace simple_voip_dummy {

Dummy::Dummy():
        log_id_( 0 ),
        callback_( nullptr ),
        scheduler_( scheduler ),
        last_call_id_( 0 )
{
}

Dummy::~Dummy()
{
}

bool Dummy::init(
        unsigned int                        log_id,
        const Config                        & config,
        simple_voip::ISimpleVoipCallback    * callback,
        scheduler::IScheduler               * scheduler,
        std::string                         * error_msg )
{
    * error_msg   = "simple_voip_dummy";

    log_id_     = log_id;
    callback_   = callback;
    scheduler_  = scheduler;

    dummy_log_info( log_id_, "init OK" );

    return true;
}

void Dummy::consume( const simple_voip::ForwardObject * req )
{
    WorkerBase::consume( req );
}

void Dummy::consume( const simple_voip::CallbackObject * req )
{
    WorkerBase::consume( req );
}

void Dummy::start()
{
    WorkerBase::start();
}

void Dummy::shutdown()
{
    WorkerBase::shutdown();
}

// WorkerT interface

void Dummy::handle( const simple_voip::IObject * req )
{
    dummy_log_debug( log_id_, "handle: %s", typeid( *req ).name() );

    if( dynamic_cast< const simple_voip::ForwardObject *>( req ) != nullptr )
    {
        handle( dynamic_cast< const simple_voip::ForwardObject *>( req ) );

        delete req;
    }
    else if( dynamic_cast< const simple_voip::CallbackObject *>( req ) != nullptr )
    {
        handle( dynamic_cast< const simple_voip::CallbackObject *>( req ) );

        // callback objects will be consumed by the consumer
    }
}

void Dummy::handle( const simple_voip::ForwardObject * req )
{
    typedef Dummy Type;

    typedef void (Type::*PPMF)( const simple_voip::ForwardObject * r );

#define HANDLER_MAP_ENTRY(_v)       { typeid( simple_voip::_v ),            & Type::handle_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        HANDLER_MAP_ENTRY( InitiateCallRequest ),
        HANDLER_MAP_ENTRY( DropRequest ),
        HANDLER_MAP_ENTRY( PlayFileRequest ),
        HANDLER_MAP_ENTRY( PlayFileStopRequest ),
        HANDLER_MAP_ENTRY( RecordFileRequest ),
        HANDLER_MAP_ENTRY( RecordFileStopRequest ),
    };

#undef HANDLER_MAP_ENTRY

    auto it = funcs.find( typeid( * req ) );

    if( it != funcs.end() )
    {
        (this->*it->second)( req );
    }
}

void Dummy::handle( const simple_voip::CallbackObject * req )
{
    typedef Dummy Type;

    typedef void (Type::*PPMF)( const simple_voip::CallbackObject * r );

#define HANDLER_MAP_ENTRY(_v)       { typeid( simple_voip::_v ),            & Type::handle_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        HANDLER_MAP_ENTRY( Dialing ),
        HANDLER_MAP_ENTRY( Ringing ),
        HANDLER_MAP_ENTRY( Connected ),
        HANDLER_MAP_ENTRY( ConnectionLost ),
        HANDLER_MAP_ENTRY( DtmfTone ),
    };

#undef HANDLER_MAP_ENTRY

    auto it = funcs.find( typeid( * req ) );

    if( it != funcs.end() )
    {
        (this->*it->second)( req );
    }
}

void Dummy::handle_InitiateCallRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::InitiateCallRequest &>( * rreq );

    auto has_to_init = get_true( config_.icr_ok_response_probability );

    if( has_to_init )
    {
        last_call_id_++;

        auto call = new Call( last_call_id_, log_id_, config_, this, callback_, scheduler_ );

        dummy_log_info( log_id_, "new call %u %s", last_call_id_, r.party.c_str() );

        auto b = map_id_to_call_.insert( std::make_pair( last_call_id_, call ) ).second;

        assert( b );(void)b;

        call->handle( r );
    }
    else
    {
        auto has_to_reject = get_true( config_.icr_reject_response_probability );

        dummy_log_info( log_id_, "not accepted: is rejection %u", (int) has_to_reject );

        if( has_to_reject )
            send_reject_response( r.req_id, "rejected" );
        else
            send_error_response( r.req_id, "error" );
    }
}

void Dummy::handle_DropRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::DropRequest &>( * rreq );

    forward_req_to_call( r );
}

void Dummy::handle_PlayFileRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::PlayFileRequest &>( * rreq );

    forward_req_to_call( r );
}

void Dummy::handle_PlayFileStopRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::PlayFileStopRequest &>( * rreq );

    forward_req_to_call( r );
}

void Dummy::handle_RecordFileRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::RecordFileRequest &>( * rreq );

    forward_req_to_call( r );
}

void Dummy::handle_RecordFileStopRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::RecordFileStopRequest &>( * rreq );

    forward_req_to_call( r );
}

void Dummy::handle_Dialing( const simple_voip::CallbackObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::Dialing &>( * rreq );

    forward_resp_to_call( r );
}

void Dummy::handle_Ringing( const simple_voip::CallbackObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::Ringing &>( * rreq );

    forward_resp_to_call( r );
}

void Dummy::handle_Connected( const simple_voip::CallbackObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::Connected &>( * rreq );

    forward_resp_to_call( r );
}

void Dummy::handle_ConnectionLost( const simple_voip::CallbackObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::ConnectionLost &>( * rreq );

    forward_resp_to_call( r );
}

void Dummy::handle_DtmfTone( const simple_voip::CallbackObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::DtmfTone &>( * rreq );

    forward_resp_to_call( r );
}

void Dummy::check_call_end( MapIdToCall::iterator it )
{
    if( it->second->is_ended() )
    {
        delete it->second;

        map_id_to_call_.erase( it );
    }
}

void Dummy::send_error_response( uint32_t req_id, const std::string & error_message )
{
    auto resp = simple_voip::create_error_response( req_id, 0, error_message );

    callback_->consume( resp );
}

void Dummy::send_reject_response( uint32_t req_id, const std::string & error_message )
{
    auto resp = simple_voip::create_reject_response( req_id, 0, error_message );

    callback_->consume( resp );
}

template <class T>
void Dummy::forward_resp_to_call( const T & req )
{
    auto call_id = req.call_id;

    auto it = map_id_to_call_.find( call_id );

    if( it != map_id_to_call_.end() )
    {
        it->second->handle( req );

        check_call_end( it );
    }
    else
    {
        dummy_log_info( log_id_, "call %u not found", call_id );
    }
}

} // namespace simple_voip_dummy
