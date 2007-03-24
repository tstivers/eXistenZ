#pragma once
#include <nxphysics.h>
#include <nxstream.h>

namespace physics {
	class MemoryStream : public NxStream {
	public:
		MemoryStream(NxU32 grow_by = 4096);
		virtual                     ~MemoryStream();

		virtual     NxU8            readByte()                              const;
		virtual     NxU16           readWord()                              const;
		virtual     NxU32           readDword()                             const;
		virtual     float           readFloat()                             const;
		virtual     double          readDouble()                            const;
		virtual     void            readBuffer(void* buffer, NxU32 size)    const;

		virtual     NxStream&       storeByte(NxU8 b);
		virtual     NxStream&       storeWord(NxU16 w);
		virtual     NxStream&       storeDword(NxU32 d);
		virtual     NxStream&       storeFloat(NxReal f);
		virtual     NxStream&       storeDouble(NxF64 f);
		virtual     NxStream&       storeBuffer(const void* buffer, NxU32 size);

		char* buf;
		NxU32 size;
		NxU32 pos;
		NxU32 grow_by;
		void growBuffer();
		void reset();
	};
}