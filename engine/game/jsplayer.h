#pragma once

namespace game {
	class Player;
}

namespace jsplayer {
	JSBool createPlayerObject(JSContext* cx, JSObject* parent, const char* name, game::Player* player);
}