#include <framework/settings/functions.h>

#include <cstring>

c_text_editor::c_text_editor()
	: m_line_spacing( 1.3f )
	, m_undo_index( 0 )
	, m_tab_size( 4 )
	, m_overwrite( false )
	, m_read_only( false )
	, m_within_render( false )
	, m_scroll_to_cursor( false )
	, m_scroll_to_top( false )
	, m_text_changed( false )
	, m_colorizer_enabled( false )
	, m_text_start( 20.0f )
	, m_left_margin( 10 )
	, m_cursor_position_changed( false )
	, m_color_range_min( 0 )
	, m_color_range_max( 0 )
	, m_selection_mode( selection_mode::normal )
	, m_check_comments( false )
	, m_last_click( -1.0f )
	, m_handle_keyboard_inputs( true )
	, m_handle_mouse_inputs( true )
	, m_ignore_imgui_child( true )
	, m_show_whitespaces( false )
	, m_start_time( 0 )
{
	m_lines.emplace_back();
}

c_text_editor::~c_text_editor() = default;

void c_text_editor::set_text( const std::string& aText )
{
	m_lines.clear();
	m_lines.emplace_back();

	for ( char chr : aText )
	{
		if ( chr == '\r' )
			continue;
		if ( chr == '\n' )
			m_lines.emplace_back();
		else
			m_lines.back().emplace_back( glyph( chr , pallete_index::def ) );
	}

	if ( m_lines.empty() )
		m_lines.emplace_back();

	m_text_changed = true;
}

std::string c_text_editor::get_text() const
{
	std::string result;

	for ( size_t i = 0; i < m_lines.size(); ++i )
	{
		for ( const auto& ch : m_lines[i] )
			result += ch.m_char;
		if ( i + 1 < m_lines.size() )
			result += '\n';
	}

	return result;
}

void c_text_editor::render( const char* aTitle , const ImVec2& aSize , bool /*aBorder*/ )
{
	std::string text = get_text();
	const size_t bufferSize = text.size() + 1 < 65536 ? 65536 : text.size() + 1;
	std::vector<char> buffer( bufferSize , '\0' );
	if ( !text.empty() )
		std::memcpy( buffer.data() , text.c_str() , text.size() );

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding , ImVec2( 4.f , 4.f ) );
	if ( ImGui::InputTextMultiline( aTitle , buffer.data() , buffer.size() , aSize , ImGuiInputTextFlags_AllowTabInput ) )
		set_text( buffer.data() );
	ImGui::PopStyleVar();
}
