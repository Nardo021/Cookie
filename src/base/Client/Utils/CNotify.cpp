#include "CNotify.hpp"

#include <Client/Core/CEasing.hpp>
#include <Client/Fonts/EmbeddedFonts.hpp>
#include <font_awesome_5.h>

#include <cstdio>
#include <ctime>

static CNotify g_CNotify{};

namespace
{
	auto GetTime() noexcept -> float
	{
		return static_cast<float>( std::clock() ) / static_cast<float>( CLOCKS_PER_SEC );
	}
}

CNotify::NotificationData_t::NotificationData_t( ENotificationType type , const char* format , ... ) :
	nType( type ) , flCreationTime( GetTime() ) , animHandler( &CEasing::InQuad , &CEasing::OutQuad )
{
	va_list args;
	va_start( args , format );
	vsnprintf( szBuffer , sizeof( szBuffer ) , format , args );
	va_end( args );
}

auto CNotify::NotificationData_t::GetTypeColor() const -> ImU32
{
	switch ( nType )
	{
	case N_TYPE_INFO:
		return IM_COL32( 0 , 200 , 255 , 255 );
	case N_TYPE_SUCCESS:
		return IM_COL32( 0 , 220 , 80 , 255 );
	case N_TYPE_WARNING:
		return IM_COL32( 255 , 220 , 0 , 255 );
	case N_TYPE_ERROR:
		return IM_COL32( 255 , 60 , 60 , 255 );
	default:
		return IM_COL32( 255 , 255 , 255 , 255 );
	}
}

auto CNotify::NotificationData_t::GetIcon() const -> const char*
{
	switch ( nType )
	{
	case N_TYPE_INFO:
		return ICON_FA_INFO;
	case N_TYPE_SUCCESS:
		return ICON_FA_CHECK;
	case N_TYPE_WARNING:
		return ICON_FA_EXCLAMATION;
	case N_TYPE_ERROR:
		return ICON_FA_TIMES;
	default:
		return nullptr;
	}
}

auto CNotify::NotificationData_t::GetTypeLabel() const -> const char*
{
	switch ( nType )
	{
	case N_TYPE_INFO:
		return "[i]";
	case N_TYPE_SUCCESS:
		return "[+]";
	case N_TYPE_WARNING:
		return "[!]";
	case N_TYPE_ERROR:
		return "[x]";
	default:
		return "";
	}
}

auto CNotify::NotificationData_t::Data() const -> const char*
{
	return szBuffer;
}

auto CNotify::NotificationData_t::GetTimeDelta( float currentTime ) const -> float
{
	return currentTime - flCreationTime;
}

auto CNotify::Push( ENotificationType type , const char* format , ... ) -> void
{
	if ( format == nullptr )
		return;

	char buffer[kMaxTextSize]{};
	va_list args;
	va_start( args , format );
	vsnprintf( buffer , sizeof( buffer ) , format , args );
	va_end( args );

	if ( buffer[0] == '\0' )
		return;

	while ( m_notifications.size() >= kMaxCount )
		m_notifications.erase( m_notifications.begin() );

	m_notifications.emplace_back( type , "%s" , buffer );
}

auto CNotify::Remove( std::size_t index ) -> void
{
	if ( index >= m_notifications.size() )
		return;

	m_notifications.erase( m_notifications.begin() + static_cast<std::ptrdiff_t>( index ) );
}

auto CNotify::Render() -> void
{
	if ( m_notifications.empty() || !ImGui::GetCurrentContext() )
		return;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();
	const float currentTime = GetTime();
	float paddingY = style.WindowPadding.y;

	for ( std::size_t i = 0; i < m_notifications.size(); )
	{
		NotificationData_t& data = m_notifications[i];

		if ( data.Data()[0] == '\0' )
		{
			Remove( i );
			continue;
		}

		const float timeDelta = data.GetTimeDelta( currentTime );
		data.animHandler.Update( io.DeltaTime , 0.25f );

		if ( timeDelta >= ( kMaxLifetime - 0.25f ) )
			data.animHandler.SetSwitch( false );
		else if ( timeDelta <= 0.25f )
			data.animHandler.SetSwitch( true );

		const float animValue = data.animHandler.GetValue( 1.f );

		if ( !data.animHandler.GetSwitch() && animValue <= 0.11f )
		{
			Remove( i );
			continue;
		}

		char windowName[32];
		snprintf( windowName , sizeof( windowName ) , "CookieNotify##%zu" , i );

		const ImVec2 slideOffset{ style.WindowPadding.x * animValue , paddingY };
		ImGui::SetNextWindowPos( slideOffset , ImGuiCond_Always );
		ImGui::SetNextWindowBgAlpha( 0.85f * animValue );

		ImGui::Begin( windowName , nullptr ,
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs );

		const ImU32 typeColor = data.GetTypeColor();
		ImVec4 typeColorVec = ImGui::ColorConvertU32ToFloat4( typeColor );
		typeColorVec.w *= animValue;

		if ( ImFont* iconFont = EmbeddedFonts::GetFontAwesome() )
		{
			if ( const char* icon = data.GetIcon() )
			{
				ImGui::PushFont( iconFont );
				ImGui::TextColored( typeColorVec , "%s" , icon );
				ImGui::PopFont();
				ImGui::SameLine();
			}
		}
		else
		{
			ImGui::TextColored( typeColorVec , "%s" , data.GetTypeLabel() );
			ImGui::SameLine();
		}

		ImVec4 textColor = ImGui::GetStyleColorVec4( ImGuiCol_Text );
		textColor.w *= animValue;
		ImGui::TextColored( textColor , "%s" , data.Data() );

		paddingY += ImGui::GetWindowHeight() + style.ItemSpacing.y;
		ImGui::End();

		++i;
	}
}

auto GetNotify() -> CNotify*
{
	return &g_CNotify;
}
