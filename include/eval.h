#pragma once
#include "definitions.h"
#include "game.h"


// TODO: Better eval function
class eval
{
public:
	eval() {}
	eval(const game& g);
	int operator()(const game& b);
	static constexpr int bWt = 340;
	static constexpr int nWt = 300;
	static constexpr int qWt = 900;
	static constexpr int pWt = 100;
	static constexpr int rWt = 500;
	static constexpr int weights[7] = { pWt,rWt,bWt,nWt,pos_inf,qWt,0 };

	void do_move(const move& m,int c);
	void undo_move(const move& m,int c);

private:
	int material_score = 0;

};
