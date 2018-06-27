/*

Init Config.

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

// $Revision: 9474 $ $Date:: 2018-06-26 #$ $Author: serge $


#include "init_config.h"             // self

namespace simple_voip_dummy {

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

} // namespace simple_voip_dummy
