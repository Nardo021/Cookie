#include <framework/settings/functions.h>

bool c_widget::text_field(
	std::string_view hint ,
	std::string_view label ,
	char* buf ,
	size_t buf_size ,
	const ImVec2& size ,
	ImGuiInputTextFlags flags ,
	ImGuiInputTextCallback callback ,
	void* user_data )
{
	IM_ASSERT( !( flags & ImGuiInputTextFlags_Multiline ) );

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if ( window->SkipItems )
		return false;

	const ImGuiID id = window->GetID( label.data() );
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect bb( pos , pos + size );

	ImGui::ItemSize( bb );
	if ( !ImGui::ItemAdd( bb , id ) )
		return false;

	ImGui::PushItemWidth( size.x );
	ImGui::SetCursorScreenPos( pos );

	const char* hintStr = hint.empty() ? nullptr : hint.data();
	bool changed = false;
	if ( hintStr )
		changed = ImGui::InputTextWithHint( label.data() , hintStr , buf , buf_size , flags , callback , user_data );
	else
		changed = ImGui::InputText( label.data() , buf , buf_size , flags , callback , user_data );

	ImGui::PopItemWidth();
	return changed;
}
