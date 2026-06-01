#pragma once

#include <Common/Common.hpp>
#include <ImGui/imgui.h>

#include "CDraw.hpp"

#include <cstdarg>
#include <vector>

enum ENotificationType : int
{
	N_TYPE_INFO = 0 ,
	N_TYPE_SUCCESS ,
	N_TYPE_WARNING ,
	N_TYPE_ERROR ,
	N_TYPE_MAX
};

class CNotify final
{
public:
	static constexpr std::size_t kMaxTextSize = 64U;
	static constexpr float kMaxLifetime = 5.f;
	static constexpr std::size_t kMaxCount = 5U;

	struct NotificationData_t
	{
		NotificationData_t( ENotificationType type , const char* format , ... );

		[[nodiscard]] auto GetTypeColor() const -> ImU32;
		[[nodiscard]] auto GetTypeLabel() const -> const char*;
		[[nodiscard]] auto GetIcon() const -> const char*;
		[[nodiscard]] auto Data() const -> const char*;
		[[nodiscard]] auto GetTimeDelta( float currentTime ) const -> float;

		ENotificationType nType = N_TYPE_INFO;
		char szBuffer[kMaxTextSize]{};
		float flCreationTime = 0.f;
		AnimationHandler_t animHandler{};
	};

	auto Push( ENotificationType type , const char* format , ... ) -> void;
	auto Render() -> void;

private:
	auto Remove( std::size_t index ) -> void;

	std::vector<NotificationData_t> m_notifications{};
};

auto GetNotify() -> CNotify*;
