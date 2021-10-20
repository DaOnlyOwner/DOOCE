#pragma once
#include <gameplay.h>
#include "imgui.h"
#include <memory>
#include <map>
#include "definitions.h"

class frontend
{
public:
	frontend();
	void render();
private:
	std::map<piece, ImTextureID> pieceToImage;

	void render_board(ImDrawList* dl);
	std::unique_ptr<gameplay> gp;
};