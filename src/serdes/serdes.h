#ifndef SERDES_H_
#define SERDES_H_

#include "serialisable.h"

#define SERIALISABLE(cls)                        \
	using Serialisable##cls = Serialisable<cls>; \
                                                 \
	template <>                                  \
	constexpr void Serialisable##cls::processMembers(Operator&& op) const

#define MEMBER(name) op(m_struct.name)

#endif  // SERDES_H_
