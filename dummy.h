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

#ifndef SIMPLE_VOIP_DUMMY__DUMMY_H
#define SIMPLE_VOIP_DUMMY__DUMMY_H

#include <map>
#include <string>

#include "simple_voip/i_simple_voip.h"              // ISimpleVoip
#include "simple_voip/i_simple_voip_callback.h"     // ISimpleVoipCallback
#include "workt/worker_t.h"                         // WorkerT

#include "config.h"                                 // Config
#include "call.h"                                   // Call

namespace simple_voip_dummy {

class Dummy;

typedef workt::WorkerT< const simple_voip::ForwardObject *, Dummy> WorkerBase;

class Dummy:
        public WorkerBase,
        virtual public simple_voip::ISimpleVoip
{
public:
    Dummy();
    ~Dummy();

    bool init(
            const Config                        & config,
            simple_voip::ISimpleVoipCallback    * callback,
            std::string                         & error_msg );

    virtual void consume( const simple_voip::ForwardObject * req );

    void start();

    void shutdown();

private:

    typedef std::map<uint32_t,Call*>    MapIdToCall;

private:

    // WorkerT interface
    void handle( const simple_voip::ForwardObject * req );

    void handle_InitiateCallRequest( const simple_voip::ForwardObject * req );
    void handle_DropRequest( const simple_voip::ForwardObject * req );
    void handle_PlayFileRequest( const simple_voip::ForwardObject * req );
    void handle_PlayFileStopRequest( const simple_voip::ForwardObject * req );
    void handle_RecordFileRequest( const simple_voip::ForwardObject * req );
    void handle_RecordFileStopRequest( const simple_voip::ForwardObject * req );

    void check_call_end( MapIdToCall::iterator it );
    void send_error_response( uint32_t req_id, const std::string & error_message );

    template <class T>
    void forward_to_call( const T & req )
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
            send_error_response( req.req_id );
        }
    }

private:

    Config                              config_;

    simple_voip::ISimpleVoipCallback    * callback_;

    MapIdToCall                         map_id_to_call_;
};

} // namespace simple_voip_dummy

#endif  // SIMPLE_VOIP_DUMMY__DUMMY_H
