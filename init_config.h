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

// $Revision: 9473 $ $Date:: 2018-06-26 #$ $Author: serge $

#ifndef SIMPLE_VOIP_DUMMY__INIT_CONFIG_H
#define SIMPLE_VOIP_DUMMY__INIT_CONFIG_H

#include "config_reader/config_reader.h"    // config_reader::ConfigReader#

#include "config.h"             // Config

namespace simple_voip_dummy {

void init_config( simple_voip_dummy::Config * cfg, const config_reader::ConfigReader & cr );

} // namespace simple_voip_dummy

#endif  // SIMPLE_VOIP_DUMMY__INIT_CONFIG_H
