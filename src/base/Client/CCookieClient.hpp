#pragma once

#include <Common/Common.hpp>

class CCSGOInput;
class CUserCmd;

class ICookieClient
{
public:
	virtual void OnRender() = 0;
	virtual void OnClientOutput() = 0;
};

class CCookieClient final : public ICookieClient
{
public:
	auto OnInit() -> void;
	auto OnShutdown() -> void;

	auto OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void;

	[[nodiscard]] auto IsInitialized() const -> bool { return m_initialized; }

	virtual void OnRender() override;
	virtual void OnClientOutput() override;

private:
	auto PollHotkeys() -> void;

	bool m_initialized = false;
};

auto GetCookieClient() -> CCookieClient*;
