/*

Random.

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

// $Revision: 9454 $ $Date:: 2018-06-25 #$ $Author: serge $

#include "random.h"         // self

#include <random>           // rand

namespace simple_voip_dummy {

bool get_true( uint32_t probability )
{
    bool res = ( (uint32_t) rand() % 100 ) < probability;

    return res;
}

uint32_t get_random( uint32_t min, uint32_t max )
{
    auto diff = max - min;

    auto n = ( diff > 0 ) ? ( rand() % diff ) : 0;

    auto res = min + n;

    return res;
}

} // namespace simple_voip_dummy
