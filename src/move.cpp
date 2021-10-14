#include "move.h"

bool operator==(const move& lhs, const move& rhs)
{
	return lhs.captured == rhs.captured && lhs.from == rhs.from && lhs.to == rhs.to && lhs.moved == rhs.moved && lhs.promo == rhs.promo && lhs.mtype == rhs.mtype;
}