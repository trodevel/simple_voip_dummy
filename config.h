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

// $Revision: 9428 $ $Date:: 2018-06-21 #$ $Author: serge $

#ifndef SIMPLE_VOIP_DUMMY__CONFIG_H
#define SIMPLE_VOIP_DUMMY__CONFIG_H

namespace simple_voip_dummy {

struct Config
{
    unsigned icr_ok_response_probability;       // InitiateCallRequest - InitiateCallResponse probability
    unsigned icr_reject_response_probability;   // InitiateCallRequest
    unsigned drop_ok_response_probability;          // DropRequest - DropResponse probability
    unsigned drop_err_not_rej_response_probability; // DropRequest - ErrorResponse / Not RejectResponse probability

    unsigned pf_ok_response_probability;            // PlayFileRequest - PlayFileResponse probability
    unsigned pf_err_not_rej_response_probability;   // PlayFileRequest - ErrorResponse / Not RejectResponse probability

    unsigned pfs_ok_response_probability;           // PlayFileStopRequest - PlayFileStopResponse probability
    unsigned pfs_err_not_rej_response_probability;  // PlayFileStopRequest - ErrorResponse / Not RejectResponse probability

    unsigned rf_ok_response_probability;            // RecordFileRequest - RecordFileResponse probability
    unsigned rf_err_not_rej_response_probability;   // RecordFileRequest - ErrorResponse / Not RejectResponse probability

    unsigned rfs_ok_response_probability;           // RecordFileStopRequest - RecordFileStopResponse probability
    unsigned rfs_err_not_rej_response_probability;  // RecordFileStopRequest - ErrorResponse / Not RejectResponse probability

    unsigned connected_probability;             // Connected Probability
    unsigned waiting_dialing_duration_min;      //
    unsigned waiting_dialing_duration_max;      //
    unsigned dialing_duration_min;              //
    unsigned dialing_duration_max;              //
    unsigned ringing_duration_min;              //
    unsigned ringing_duration_max;              //
    unsigned call_duration_min;                 //
    unsigned call_duration_max;                 //
};

} // namespace simple_voip_dummy

#endif  // SIMPLE_VOIP_DUMMY__CONFIG_H
