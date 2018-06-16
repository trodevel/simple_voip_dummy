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

// $Revision: 9381 $ $Date:: 2018-06-15 #$ $Author: serge $

#include "dummy.h"                  // self

#include <typeindex>                // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "simple_voip/objects.h"            // InitiateCallResponse
#include "simple_voip/object_factory.h"     // create_error_response

namespace simple_voip_dummy {

Dummy::Dummy():
        callback_( nullptr )
{
}

Dummy::~Dummy()
{
}

bool Dummy::init(
        const Config                        & config,
        simple_voip::ISimpleVoipCallback    * callback,
        std::string                         & error_msg )
{
    error_msg   = "simple_voip_dummy";

    callback_   = callback;

    return true;
}

void Dummy::consume( const simple_voip::ForwardObject * req )
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

    delete req;
}

void Dummy::handle_InitiateCallRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::InitiateCallRequest &>( * rreq );
}

void Dummy::handle_DropRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::DropRequest &>( * rreq );

    forward_to_call( r );
}

void Dummy::handle_PlayFileRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::PlayFileRequest &>( * rreq );

    forward_to_call( r );
}

void Dummy::handle_PlayFileStopRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::PlayFileStopRequest &>( * rreq );

    forward_to_call( r );
}

void Dummy::handle_RecordFileRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::RecordFileRequest &>( * rreq );

    forward_to_call( r );
}

void Dummy::handle_RecordFileStopRequest( const simple_voip::ForwardObject * rreq )
{
    auto & r = dynamic_cast< const simple_voip::RecordFileStopRequest &>( * rreq );

    forward_to_call( r );
}

void Dummy::check_call_end( MapIdToCall::iterator it )
{
    if( it->second->is_ended() )
    {
        map_id_to_call_.erase( it );
    }
}

void Dummy::send_error_response( uint32_t req_id, const std::string & error_message )
{
    auto resp = simple_voip::create_error_response( req_id, 0, error_message );

    callback_->consume( resp );
}


} // namespace simple_voip_dummy
