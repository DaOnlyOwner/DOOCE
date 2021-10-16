#pragma once
#include "definitions.h"
#include "game.h"


// TODO: Better eval function
class eval
{
public:
	
	i64 operator()(const game& b);

private:
	static constexpr int bWt = 340;
	static constexpr int nWt = 300;
	static constexpr int qWt = 900;
	static constexpr int pWt = 100;
	static constexpr int rWt = 500;

};
