#pragma once

namespace google::protobuf
{
class Message;
}

inline bool ( *ProtobufSerializePartialToArrayOriginal )( google::protobuf::Message* , void* , int ) = nullptr;
