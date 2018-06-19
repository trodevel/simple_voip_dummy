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

// $Revision: 9384 $ $Date:: 2018-06-18 #$ $Author: serge $

#include "call.h"                   // self

#include "simple_voip/objects.h"    // InitiateCallResponse

namespace simple_voip_dummy {

Call::Call(
        const Config                        & config,
        Dummy                               * parent ):
                config_( config ),
                parent_( parent )
{
}

Call::~Call()
{
}

void Call::handle( const simple_voip::InitiateCallRequest & req )
{
}

void Call::handle( const simple_voip::DropRequest & req )
{
}

void Call::handle( const simple_voip::PlayFileRequest & req )
{
}

void Call::handle( const simple_voip::PlayFileStopRequest & req )
{
}

void Call::handle( const simple_voip::RecordFileRequest & req )
{
}

void Call::handle( const simple_voip::RecordFileStopRequest & req )
{
}

} // namespace simple_voip_dummy
