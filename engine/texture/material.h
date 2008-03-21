#pragma once

namespace texture {
	struct Material {
		Material(D3DCOLOR ambient, D3DXVECTOR3 direction, D3DXCOLOR color);
		Material();
		~Material();
		void reset();
		bool operator ==(const Material& other) const {
			if(memcmp(this, &other, sizeof(Material)) == 0)
				return true;
			else
				return false;
		}

		bool operator != (const Material& other) const {
			return !(*this == other);
		}

		D3DCOLOR ambient;
		D3DLIGHT9 light;
	};
};