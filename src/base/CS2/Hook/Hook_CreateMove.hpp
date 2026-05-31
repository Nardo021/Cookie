#pragma once

#include <Common/Common.hpp>

#include <CS2/SDK/Network/CNetworkMessages.hpp>

#include <GameClient/ProtobufSerialize.hpp>

class CCSGOInput;

auto Hook_CreateMove( CCSGOInput* pInput , uint32_t split_screen_index , bool frame_active ) -> bool;

using CreateMove_t = decltype( &Hook_CreateMove );
inline CreateMove_t CreateMove_o = nullptr;

auto Hook_MessageLite_SerializePartialToArray( google::protobuf::Message* pMsg , void* out_buffer , int size ) -> bool;
