/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

#define MAKE_KEY(a,b) {#a,b},

namespace input {
	struct {
		char* name;
		int value;
	} keys[] = {
		MAKE_KEY( KEY_ESCAPE ,         0x01)
		MAKE_KEY( KEY_1      ,         0x02)
		MAKE_KEY( KEY_2      ,         0x03)
		MAKE_KEY( KEY_3      ,         0x04)
		MAKE_KEY( KEY_4      ,         0x05)
		MAKE_KEY( KEY_5      ,         0x06)
		MAKE_KEY( KEY_6      ,         0x07)
		MAKE_KEY( KEY_7      ,         0x08)
		MAKE_KEY( KEY_8      ,         0x09)
		MAKE_KEY( KEY_9      ,         0x0A)
		MAKE_KEY( KEY_0      ,         0x0B)
		MAKE_KEY( KEY_MINUS  ,         0x0C)
		MAKE_KEY( KEY_EQUALS ,         0x0D)
		MAKE_KEY( KEY_BACK   ,         0x0E)
		MAKE_KEY( KEY_TAB    ,         0x0F)
		MAKE_KEY( KEY_Q      ,         0x10)
		MAKE_KEY( KEY_W      ,         0x11)
		MAKE_KEY( KEY_E      ,         0x12)
		MAKE_KEY( KEY_R      ,         0x13)
		MAKE_KEY( KEY_T      ,         0x14)
		MAKE_KEY( KEY_Y      ,         0x15)
		MAKE_KEY( KEY_U      ,         0x16)
		MAKE_KEY( KEY_I      ,         0x17)
		MAKE_KEY( KEY_O      ,         0x18)
		MAKE_KEY( KEY_P      ,         0x19)
		MAKE_KEY( KEY_LBRACKET ,       0x1A)
		MAKE_KEY( KEY_RBRACKET ,       0x1B)
		MAKE_KEY( KEY_RETURN   ,       0x1C)   
		MAKE_KEY( KEY_LCONTROL ,       0x1D)
		MAKE_KEY( KEY_A        ,       0x1E)
		MAKE_KEY( KEY_S        ,       0x1F)
		MAKE_KEY( KEY_D        ,       0x20)
		MAKE_KEY( KEY_F        ,       0x21)
		MAKE_KEY( KEY_G        ,       0x22)
		MAKE_KEY( KEY_H        ,       0x23)
		MAKE_KEY( KEY_J        ,       0x24)
		MAKE_KEY( KEY_K        ,       0x25)
		MAKE_KEY( KEY_L        ,       0x26)
		MAKE_KEY( KEY_SEMICOLON,       0x27)
		MAKE_KEY( KEY_APOSTROPHE ,     0x28)
		MAKE_KEY( KEY_GRAVE      ,     0x29)  
		MAKE_KEY( KEY_LSHIFT     ,     0x2A)
		MAKE_KEY( KEY_BACKSLASH  ,     0x2B)
		MAKE_KEY( KEY_Z          ,     0x2C)
		MAKE_KEY( KEY_X          ,     0x2D)
		MAKE_KEY( KEY_C          ,     0x2E)
		MAKE_KEY( KEY_V          ,     0x2F)
		MAKE_KEY( KEY_B          ,     0x30)
		MAKE_KEY( KEY_N          ,     0x31)
		MAKE_KEY( KEY_M          ,     0x32)
		MAKE_KEY( KEY_COMMA      ,     0x33)
		MAKE_KEY( KEY_PERIOD     ,     0x34)   
		MAKE_KEY( KEY_SLASH      ,     0x35)  
		MAKE_KEY( KEY_RSHIFT     ,     0x36)
		MAKE_KEY( KEY_MULTIPLY   ,     0x37)   
		MAKE_KEY( KEY_LMENU      ,     0x38)  
		MAKE_KEY( KEY_SPACE      ,     0x39)
		MAKE_KEY( KEY_CAPITAL    ,     0x3A)
		MAKE_KEY( KEY_F1         ,     0x3B)
		MAKE_KEY( KEY_F2         ,     0x3C)
		MAKE_KEY( KEY_F3        ,      0x3D)
		MAKE_KEY( KEY_F4        ,      0x3E)
		MAKE_KEY( KEY_F5        ,      0x3F)
		MAKE_KEY( KEY_F6        ,      0x40)
		MAKE_KEY( KEY_F7        ,      0x41)
		MAKE_KEY( KEY_F8        ,      0x42)
		MAKE_KEY( KEY_F9        ,      0x43)
		MAKE_KEY( KEY_F10       ,      0x44)
		MAKE_KEY( KEY_NUMLOCK   ,      0x45)
		MAKE_KEY( KEY_SCROLL    ,      0x46 )   
		MAKE_KEY( KEY_NUMPAD7   ,      0x47)
		MAKE_KEY( KEY_NUMPAD8   ,      0x48)
		MAKE_KEY( KEY_NUMPAD9   ,      0x49)
		MAKE_KEY( KEY_SUBTRACT  ,      0x4A )   
		MAKE_KEY( KEY_NUMPAD4   ,      0x4B)
		MAKE_KEY( KEY_NUMPAD5   ,      0x4C)
		MAKE_KEY( KEY_NUMPAD6   ,      0x4D)
		MAKE_KEY( KEY_ADD       ,      0x4E )  
		MAKE_KEY( KEY_NUMPAD1   ,      0x4F)
		MAKE_KEY( KEY_NUMPAD2   ,      0x50)
		MAKE_KEY( KEY_NUMPAD3   ,      0x51)
		MAKE_KEY( KEY_NUMPAD0   ,      0x52)
		MAKE_KEY( KEY_DECIMAL   ,      0x53)   
		MAKE_KEY( KEY_OEM_102   ,      0x56)   
		MAKE_KEY( KEY_F11       ,      0x57)
		MAKE_KEY( KEY_F12       ,      0x58)
		MAKE_KEY( KEY_F13       ,      0x64 )   
		MAKE_KEY( KEY_F14       ,      0x65  )  
		MAKE_KEY( KEY_F15       ,      0x66   ) 
		MAKE_KEY( KEY_KANA      ,      0x70)    
		MAKE_KEY( KEY_ABNT_C1   ,      0x73)    
		MAKE_KEY( KEY_CONVERT   ,      0x79)    
		MAKE_KEY( KEY_NOCONVERT ,      0x7B)    
		MAKE_KEY( KEY_YEN       ,      0x7D)    
		MAKE_KEY( KEY_ABNT_C2   ,      0x7E)    
		MAKE_KEY( KEY_NUMPADEQUALS ,   0x8D)    
		MAKE_KEY( KEY_PREVTRACK    ,   0x90)    
		MAKE_KEY( KEY_AT           ,   0x91)    
		MAKE_KEY( KEY_COLON        ,   0x92)    
		MAKE_KEY( KEY_UNDERLINE    ,   0x93)    
		MAKE_KEY( KEY_KANJI        ,   0x94)    
		MAKE_KEY( KEY_STOP         ,   0x95)    
		MAKE_KEY( KEY_AX           ,   0x96)   
		MAKE_KEY( KEY_UNLABELED    ,   0x97)    
		MAKE_KEY( KEY_NEXTTRACK    ,   0x99)    
		MAKE_KEY( KEY_NUMPADENTER  ,   0x9C)    
		MAKE_KEY( KEY_RCONTROL     ,   0x9D)
		MAKE_KEY( KEY_MUTE         ,   0xA0)    
		MAKE_KEY( KEY_CALCULATOR   ,   0xA1)    
		MAKE_KEY( KEY_PLAYPAUSE    ,   0xA2)    
		MAKE_KEY( KEY_MEDIASTOP    ,   0xA4)    
		MAKE_KEY( KEY_VOLUMEDOWN   ,   0xAE)    
		MAKE_KEY( KEY_VOLUMEUP     ,   0xB0)    
		MAKE_KEY( KEY_WEBHOME      ,   0xB2)    
		MAKE_KEY( KEY_NUMPADCOMMA  ,   0xB3)    /* , on numeric keypad (NEC PC98) */
		MAKE_KEY( KEY_DIVIDE       ,   0xB5)    /* / on numeric keypad */
		MAKE_KEY( KEY_SYSRQ        ,   0xB7)
		MAKE_KEY( KEY_RMENU        ,   0xB8)    /* right Alt */
		MAKE_KEY( KEY_PAUSE        ,   0xC5)    /* Pause */
		MAKE_KEY( KEY_HOME         ,   0xC7)    /* Home on arrow keypad */
		MAKE_KEY( KEY_UP           ,   0xC8)    /* UpArrow on arrow keypad */
		MAKE_KEY( KEY_PRIOR        ,   0xC9)   /* PgUp on arrow keypad */
		MAKE_KEY( KEY_LEFT         ,   0xCB)    /* LeftArrow on arrow keypad */
		MAKE_KEY( KEY_RIGHT        ,   0xCD )   /* RightArrow on arrow keypad */
		MAKE_KEY( KEY_END          ,   0xCF )   /* End on arrow keypad */
		MAKE_KEY( KEY_DOWN         ,   0xD0 )   /* DownArrow on arrow keypad */
		MAKE_KEY( KEY_NEXT         ,   0xD1 )   /* PgDn on arrow keypad */
		MAKE_KEY( KEY_INSERT       ,   0xD2 )   /* Insert on arrow keypad */
		MAKE_KEY( KEY_DELETE       ,   0xD3 )   /* Delete on arrow keypad */
		MAKE_KEY( KEY_LWIN         ,   0xDB )   /* Left Windows key */
		MAKE_KEY( KEY_RWIN         ,   0xDC )   /* Right Windows key */
		MAKE_KEY( KEY_APPS         ,   0xDD )   /* AppMenu key */
		MAKE_KEY( KEY_POWER        ,   0xDE )   /* System Power */
		MAKE_KEY( KEY_SLEEP        ,   0xDF )   /* System Sleep */
		MAKE_KEY( KEY_WAKE         ,   0xE3 )   /* System Wake */
		MAKE_KEY( KEY_WEBSEARCH    ,   0xE5 )   /* Web Search */
		MAKE_KEY( KEY_WEBFAVORITES,    0xE6 )   /* Web Favorites */
		MAKE_KEY( KEY_WEBREFRESH   ,   0xE7 )   /* Web Refresh */
		MAKE_KEY( KEY_WEBSTOP      ,   0xE8 )   /* Web Stop */
		MAKE_KEY( KEY_WEBFORWARD   ,   0xE9 )   /* Web Forward */
		MAKE_KEY( KEY_WEBBACK      ,   0xEA )   /* Web Back */
		MAKE_KEY( KEY_MYCOMPUTER   ,   0xEB )   /* My Computer */
		MAKE_KEY( KEY_MAIL         ,   0xEC )   /* Mail */
		MAKE_KEY( KEY_MEDIASELECT  ,   0xED )   /* Media Select */
		MAKE_KEY( BUTTON_0		   ,   0xF0 )   /* Mouse Button 0 */
		MAKE_KEY( BUTTON_1		   ,   0xF1 )   /* Mouse Button 1 */
		MAKE_KEY( BUTTON_2		   ,   0xF2 )   /* Mouse Button 2 */
		MAKE_KEY( BUTTON_3		   ,   0xF3 )   /* Mouse Button 3 */
		MAKE_KEY( KEY_MWHEELUP     ,   0xF4 )   /* Mouse Wheel Up */
		MAKE_KEY( KEY_MWHEELDN     ,   0xF5 )   /* Mouse Wheel Down */
		/*
		*  Alternate names for keys, to facilitate transition from DOS.
		*/
		MAKE_KEY( KEY_BACKSPACE    ,   DIK_BACK)            /* backspace */
		MAKE_KEY( KEY_NUMPADSTAR   ,   DIK_MULTIPLY)        /* * on numeric keypad */
		MAKE_KEY( KEY_LALT         ,   DIK_LMENU   )        /* left Alt */
		MAKE_KEY( KEY_CAPSLOCK     ,   DIK_CAPITAL )        /* CapsLock */
		MAKE_KEY( KEY_NUMPADMINUS  ,   DIK_SUBTRACT)        /* - on numeric keypad */
		MAKE_KEY( KEY_NUMPADPLUS   ,   DIK_ADD     )        /* + on numeric keypad */
		MAKE_KEY( KEY_NUMPADPERIOD ,   DIK_DECIMAL )        /* . on numeric keypad */
		MAKE_KEY( KEY_NUMPADSLASH  ,   DIK_DIVIDE  )        /* / on numeric keypad */
		MAKE_KEY( KEY_RALT         ,   DIK_RMENU   )        /* right Alt */
		MAKE_KEY( KEY_UPARROW      ,   DIK_UP      )        /* UpArrow on arrow keypad */
		MAKE_KEY( KEY_PGUP         ,   DIK_PRIOR   )        /* PgUp on arrow keypad */
		MAKE_KEY( KEY_LEFTARROW    ,   DIK_LEFT    )        /* LeftArrow on arrow keypad */
		MAKE_KEY( KEY_RIGHTARROW   ,   DIK_RIGHT   )        /* RightArrow on arrow keypad */
		MAKE_KEY( KEY_DOWNARROW    ,   DIK_DOWN    )        /* DownArrow on arrow keypad */
		MAKE_KEY( KEY_PGDN         ,   DIK_NEXT    )        /* PgDn on arrow keypad */
		{ NULL, -1 }
	};
}