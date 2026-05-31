#pragma once

#include <Windows.h>
#include <ImGui/imgui.h>
#include <cstdio>
#include <cstring>

namespace Menu
{
    inline int*  s_keyBindTarget = nullptr;
    inline bool  s_keyBindWaitMouseUp = false;
    inline bool  s_keyBindKeyWasDown[256] = {};

    inline bool IsAnyMouseDown()
    {
        return ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0
            || ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) != 0
            || ( GetAsyncKeyState( VK_MBUTTON ) & 0x8000 ) != 0
            || ( GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000 ) != 0
            || ( GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000 ) != 0;
    }

    inline void SnapshotKeyState()
    {
        for ( int vk = 0; vk < 256; ++vk )
            s_keyBindKeyWasDown[vk] = ( GetAsyncKeyState( vk ) & 0x8000 ) != 0;
    }

    inline void BeginKeyBindListen( int* key )
    {
        s_keyBindTarget = key;
        s_keyBindWaitMouseUp = true;
        std::memset( s_keyBindKeyWasDown , 0 , sizeof( s_keyBindKeyWasDown ) );
    }

    inline const char* VkToLabel( int vk , char* buf , size_t bufSize )
    {
        if ( vk == 0 )
            return "None";

        switch ( vk )
        {
        case VK_LBUTTON:  return "Mouse1";
        case VK_RBUTTON:  return "Mouse2";
        case VK_MBUTTON:  return "Mouse3";
        case VK_XBUTTON1: return "Mouse4";
        case VK_XBUTTON2: return "Mouse5";
        case VK_SHIFT:    return "Shift";
        case VK_CONTROL:  return "Ctrl";
        case VK_MENU:     return "Alt";
        case VK_SPACE:    return "Space";
        case VK_TAB:      return "Tab";
        case VK_CAPITAL:  return "Caps Lock";
        case VK_ESCAPE:   return "Esc";
        case VK_INSERT:   return "Insert";
        case VK_DELETE:   return "Delete";
        case VK_HOME:     return "Home";
        case VK_END:      return "End";
        case VK_PRIOR:    return "Page Up";
        case VK_NEXT:     return "Page Down";
        case VK_LEFT:     return "Left";
        case VK_RIGHT:    return "Right";
        case VK_UP:       return "Up";
        case VK_DOWN:     return "Down";
        default: break;
        }

        if ( vk >= '0' && vk <= '9' )
        {
            snprintf( buf , bufSize , "%c" , vk );
            return buf;
        }

        if ( vk >= 'A' && vk <= 'Z' )
        {
            snprintf( buf , bufSize , "%c" , vk );
            return buf;
        }

        if ( vk >= VK_F1 && vk <= VK_F24 )
        {
            snprintf( buf , bufSize , "F%d" , vk - VK_F1 + 1 );
            return buf;
        }

        if ( vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9 )
        {
            snprintf( buf , bufSize , "Num %d" , vk - VK_NUMPAD0 );
            return buf;
        }

        snprintf( buf , bufSize , "0x%02X" , vk );
        return buf;
    }

    inline void KeyBindWidget( const char* label , int* key , bool* useKey = nullptr )
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted( label );
        ImGui::SameLine( 140.f );

        char labelBuf[32];
        const bool listening = ( s_keyBindTarget == key );
        const char* display = listening ? "Press a key..." : VkToLabel( *key , labelBuf , sizeof( labelBuf ) );

        ImGui::PushID( label );
        if ( ImGui::Button( display , ImVec2( 130.f , 0.f ) ) && !listening )
            BeginKeyBindListen( key );

        if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenBlockedByActiveItem ) )
            ImGui::SetTooltip( "Click, release mouse, then press a key. Esc cancels." );

        ImGui::SameLine();
        if ( ImGui::Button( "Unbind" ) )
        {
            *key = 0;
            if ( useKey )
                *useKey = false;
            if ( s_keyBindTarget == key )
                s_keyBindTarget = nullptr;
        }
        if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenBlockedByActiveItem ) )
            ImGui::SetTooltip( "Clear bind. Feature stays always active when enabled." );
        ImGui::PopID();

        if ( !listening )
            return;

        if ( s_keyBindWaitMouseUp )
        {
            if ( IsAnyMouseDown() )
                return;

            s_keyBindWaitMouseUp = false;
            SnapshotKeyState();
            return;
        }

        if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) && !s_keyBindKeyWasDown[VK_ESCAPE] )
        {
            s_keyBindTarget = nullptr;
            return;
        }

        for ( int vk = 1; vk < 256; ++vk )
        {
            if ( vk == VK_ESCAPE )
                continue;

            const bool down = ( GetAsyncKeyState( vk ) & 0x8000 ) != 0;
            if ( down && !s_keyBindKeyWasDown[vk] )
            {
                *key = vk;
                if ( useKey )
                    *useKey = true;
                s_keyBindTarget = nullptr;
                break;
            }

            s_keyBindKeyWasDown[vk] = down;
        }
    }
}
