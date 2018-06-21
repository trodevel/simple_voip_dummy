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

// $Revision: 9416 $ $Date:: 2018-06-20 #$ $Author: serge $

#ifndef SIMPLE_VOIP_DUMMY__CONFIG_H
#define SIMPLE_VOIP_DUMMY__CONFIG_H

namespace simple_voip_dummy {

struct Config
{
    unsigned icr_ok_response_probability;       // InitiateCallRequest - InitiateCallResponse probability
    unsigned icr_reject_response_probability;   // InitiateCallRequest
    unsigned drop_ok_response_probability;          // DropRequest - InitiateCallResponse probability
    unsigned drop_err_not_rej_response_probability; // DropRequest - ErrorResponse / Not RejectResponse probability
    unsigned connected_probability;             // Connected Probability
    unsigned dialing_duration_min;              //
    unsigned dialing_duration_max;              //
    unsigned ringing_duration_min;              //
    unsigned ringing_duration_max;              //
    unsigned call_duration_min;                 //
    unsigned call_duration_max;                 //
};

} // namespace simple_voip_dummy

#endif  // SIMPLE_VOIP_DUMMY__CONFIG_H
