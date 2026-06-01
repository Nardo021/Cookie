#pragma once

#include <Common/Common.hpp>
#include <Common/MemoryEngine.hpp>

#define NETWORK_CLIENT_SERVICE_INTERFACE_VERSION "NetworkClientService_001"

class CNetworkGameClient
{
public:
	auto Update() -> void
	{
		auto* deltaTickPtr = reinterpret_cast<int*>( reinterpret_cast<std::uintptr_t>( this ) + 0x258 );
		*deltaTickPtr = -1;
	}
};

class INetworkClientService
{
public:
	auto GetNetworkClient() -> CNetworkGameClient*
	{
		VirtualFn( CNetworkGameClient* )( INetworkClientService* );
		return vget< Fn >( this , 23 )( this );
	}
};
