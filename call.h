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

// $Revision: 9415 $ $Date:: 2018-06-20 #$ $Author: serge $

#ifndef SIMPLE_VOIP_DUMMY__CALL_H
#define SIMPLE_VOIP_DUMMY__CALL_H

#include <string>

#include "simple_voip/i_simple_voip.h"              // ISimpleVoip
#include "simple_voip/i_simple_voip_callback.h"     // ISimpleVoipCallback
#include "simple_voip/objects.h"                    // InitiateCallRequest
#include "scheduler/i_scheduler.h"                  // IScheduler

#include "config.h"                                 // Config

namespace simple_voip_dummy {

class Dummy;

class Call
{
    enum class state_e
    {
        IDLE,
        DIALING,
        RINGING,
        CONNECTED,
        CLOSED
    };

public:
    Call(
            uint32_t                            call_id,
            unsigned int                        log_id,
            const Config                        & config,
            Dummy                               * parent,
            simple_voip::ISimpleVoipCallback    * callback,
            scheduler::IScheduler               * scheduler );
    ~Call();

    void handle( const simple_voip::InitiateCallRequest & req );
    void handle( const simple_voip::DropRequest & req );
    void handle( const simple_voip::PlayFileRequest & req );
    void handle( const simple_voip::PlayFileStopRequest & req );
    void handle( const simple_voip::RecordFileRequest & req );
    void handle( const simple_voip::RecordFileStopRequest & req );

    bool is_ended() const;

private:

    const std::string & to_string( const state_e & l );

    void next_state( state_e state );

    void schedule_event( simple_voip::CallbackObject * ev, uint32_t exec_time );

    uint32_t calc_exec_time( uint32_t min, uint32_t max );

private:

    uint32_t                            call_id_;

    unsigned int                        log_id_;

    state_e                             state_;

    Config                              config_;

    Dummy                               * parent_;
    simple_voip::ISimpleVoipCallback    * callback_;
    scheduler::IScheduler               * scheduler_;
};

} // namespace simple_voip_dummy

#endif  // SIMPLE_VOIP_DUMMY__CALL_H
