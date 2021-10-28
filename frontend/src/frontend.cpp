#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "frontend.h"
#include <cstdio>
#include "stb_image.h"
#include <tuple>
#include "board.h"
#include "imgui_stdlib.h"
#include "fen.h"
#include "stb_image_resize.h"
#include <cstdlib>
#include "bitwise_ops.h"
#include <optional>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cmath>

namespace
{
	std::tuple<bool,GLuint,int,int> load_image(const char* path)
	{
		int w_nonscaled, h_nonscaled;
		auto data = stbi_load(path, &w_nonscaled, &h_nonscaled, nullptr, 4);
		int w = 0.14 * w_nonscaled;
		int h = 0.14 * h_nonscaled;
		stbi_uc* scaled_data = (stbi_uc*)malloc(sizeof(stbi_uc) * w *h*4);
		stbir_resize_uint8(data, w_nonscaled, h_nonscaled, 0, scaled_data, w, h, 0, 4);
		if (data == nullptr || scaled_data == nullptr)
			return { false,0,0,0 };
		GLuint img;
		glGenTextures(1, &img);
		glBindTexture(GL_TEXTURE_2D, img);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, scaled_data);
		stbi_image_free(data);
		free(scaled_data);
		return { true,img,w,h };
	}

	void set_image(const char* path, std::map<piece, image>& m,piece p)
	{
		auto [succ, img, w, h] = load_image(path);
		if (!succ) throw std::runtime_error(std::string("Failed to load image ") + path);
		image img_;
		img_.w = w;
		img_.h = h;
		img_.id = img;
		m[p] = img_;
	}

}

frontend::frontend() : tt_error_timer(1000*5),mtt_error_timer(1000*5),fen_error_timer(1000*5)
{
	tt_error_timer.stop();
	mtt_error_timer.stop();
	fen_error_timer.stop();
	set_image(RESOURCE_DIR "/wB.png", pieceToImage, { color::white,piece_type::bishop });
	set_image(RESOURCE_DIR "/wQ.png", pieceToImage, { color::white,piece_type::queen });
	set_image(RESOURCE_DIR "/wR.png", pieceToImage, { color::white,piece_type::rook });
	set_image(RESOURCE_DIR "/wK.png", pieceToImage, { color::white,piece_type::king });
	set_image(RESOURCE_DIR "/wN.png", pieceToImage, { color::white,piece_type::knight });
	set_image(RESOURCE_DIR "/wP.png", pieceToImage, { color::white,piece_type::pawn });

	set_image(RESOURCE_DIR "/bB.png", pieceToImage, { color::black,piece_type::bishop });
	set_image(RESOURCE_DIR "/bQ.png", pieceToImage, { color::black,piece_type::queen });
	set_image(RESOURCE_DIR "/bR.png", pieceToImage, { color::black,piece_type::rook });
	set_image(RESOURCE_DIR "/bK.png", pieceToImage, { color::black,piece_type::king });
	set_image(RESOURCE_DIR "/bN.png", pieceToImage, { color::black,piece_type::knight });
	set_image(RESOURCE_DIR "/bP.png", pieceToImage, { color::black,piece_type::pawn });
}

void frontend::render_board(ImDrawList* dl)
{
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 pos = ImGui::GetWindowPos();
	int c = 1;
	ImU32 colors[2] = { ImGui::ColorConvertFloat4ToU32({238/255.f,238/255.f,210/255.f,1.f}),ImGui::ColorConvertFloat4ToU32({118/255.f,150/255.f,86/255.f,1.f}) };
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			c = 1 - c;
			auto col = colors[c];
			dl->AddRectFilled({ i * sq_size+min.x+pos.x,j * sq_size+min.y +pos.y}, { i * sq_size + sq_size + min.x + pos.x,j * sq_size + sq_size + min.y + pos.y},col);
		}
		c = 1 - c;
	}

	for (int x = 0; x < 8; x++)
	{
		std::string txt;
		txt += 'a' + x;
		dl->AddText({ x * sq_size + pos.x + sq_size/2.f,BOARD_SIZE + pos.y + 35 }, 
			ImGui::ColorConvertFloat4ToU32({ 1.f, 1.f, 1.f, 1.f }), txt.c_str());
	}

	for (int y = 0; y < 8; y++)
	{
		std::string txt;
		txt += '0' + (8-y);
		dl->AddText({ BOARD_SIZE + pos.x+22, y * sq_size + pos.y +59},
			ImGui::ColorConvertFloat4ToU32({ 1.f, 1.f, 1.f, 1.f }), txt.c_str());
	}
}

std::pair<ImVec2, ImVec2> frontend::get_min_max(int x, int y, piece p, ImVec2 offset)
{
	auto pos = ImGui::GetWindowPos();
	image& img = pieceToImage[p];
	float offset_x = piece_offsets_x[p.type];
	float offset_y = piece_offsets_y[p.type];
	auto min = ImVec2(x * sq_size + pos.x + offset_x + offset.x, y * sq_size + pos.y + offset_y + offset.y);
	auto max = ImVec2(x * sq_size + pos.x + img.w + offset_x + offset.x, y * sq_size + pos.y + img.h + offset_y + offset.y);
	return { min,max };
}

void frontend::render_pieces(ImDrawList* dl)
{
	if (gp == nullptr) return;
	auto mailbox = gp->get_game().get_board().as_mailbox();
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (from == point{ x,y })
				continue;

			auto p = mailbox[x][y];
			if (!p.has_value()) continue;
			image& img = pieceToImage[p.value()];
			int y_in=0,x_in = 0;
			if (view_as_white)
			{
				y_in = y;
				x_in = x;
			}
			else
			{
				y_in = 7 - y;
				x_in = 7 - x;
			}
			auto [min, max] = get_min_max(x_in, y_in, p.value(), { 0, 0 });
			dl->AddImage((void*)img.id, min, max);
		}
	}

	if (from.is_valid())
	{
		auto p = mailbox[from.x][from.y];
		image& img = pieceToImage[p.value()];
		auto [min, max] = get_min_max(view_as_white ? from.x : 7-from.x, view_as_white?from.y : 7-from.y, p.value(), { (float)delta.x,(float)delta.y });
		dl->AddImage((void*)img.id, min, max);
	}
}

void frontend::update_click()
{
	if (gp == nullptr) return;
	auto mailbox = gp->get_game().get_board().as_mailbox();
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !from.is_valid())
	{
		auto pos = ImGui::GetMousePos();
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				auto p = mailbox[x][y];
				if (!p.has_value()) continue;
				auto& img = pieceToImage[p.value()];
				auto [min, max] = get_min_max(view_as_white ? x : 7-x, view_as_white ? y : 7-y, p.value(), { 0,0 });
				auto r = rect{ point{(int)min.x,(int)min.y},point{(int)max.x,(int)max.y} };
				auto click_pos = point{ (int)pos.x,(int)pos.y};
				if (r.contains_point(click_pos))
				{
					from = { x,y };
					last_mp = click_pos;
				}
			}
		}
	}
}

void frontend::update_drag()
{
	if (gp == nullptr) return;
	//printf("%i", from.is_valid());
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && from.is_valid())
	{
		auto pos = ImGui::GetMousePos();
		delta = { delta.x + (int)pos.x - last_mp.x,delta.y + (int)pos.y - last_mp.y };
		last_mp = point{ (int)pos.x,(int)pos.y };
	}
}

std::pair<point, point> frontend::update_let_go()
{
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && from.is_valid())
	{
		auto wp = ImGui::GetWindowPos();
		auto min = ImGui::GetWindowContentRegionMin();
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				auto r = rect{ {x * sq_size + (int)min.x + (int)wp.x,y * sq_size + (int)min.y + (int)wp.y},{x * sq_size + (int)min.x + sq_size + (int)wp.x,y * sq_size + (int)min.y + sq_size + (int)wp.y} };
				auto mp = point{ (int)ImGui::GetMousePos().x,(int)ImGui::GetMousePos().y };
				if (r.contains_point(mp))
				{
					auto to = point{ x,y };
					auto from_ = from;
					from.set_invalid();
					delta = { 0,0 };
					return{ from_,to };
				}
			}
		}
		from.set_invalid();
		delta = { 0,0 };
		return {from, { -1,-1 }};
	}
	return { {-1,-1}, { -1,-1 } };
}

void frontend::render_info()
{
	if (gp == nullptr) return;
	if (std::abs(score) >= mate)
	{
		int mate_in = std::abs(score) - mate;
		ImGui::Text("Score: Mate in %i plies", mate_in);
	}
	else ImGui::Text("Score: %f", score/100.f);
	ImGui::Text("Reached depth: %i", depth);
	ImGui::Text("Searched nodes: %iM (%i)", searched_nodes / 1000 / 1000, searched_nodes);
	ImGui::InputTextMultiline("Principal Variation", &pv, ImVec2(100, 100), ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	std::string current_fen = fen::game_to_fen(gp->get_game());
	ImGui::InputText("Current FEN", &current_fen, ImGuiInputTextFlags_ReadOnly);
	std::stringstream stream;
	stream << std::hex << gp->get_game().get_hash();
	std::string hash = stream.str();
	ImGui::InputText("Hash", &hash,ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	std::string gos_str = "Game State: ";
	if (gos == game_over_state::white_won) gos_str += "white won!";
	else if (gos == game_over_state::black_won) gos_str += "black won!";
	else if (gos == game_over_state::draw) gos_str += "Draw";
	else gos_str += "Running";

	ImGui::TextColored(ImVec4(1,1,0,1),"%s", gos_str.c_str());

}

void frontend::render_gui()
{
	ImGui::InputFloat("Minutes to think",&minutes_to_think);
	ImGui::DragInt("Transposition table size exponent",&tt_size_exponent,1.0f,20,35);
	ImGui::InputText("From FEN", &from_fen); 
	if (ImGui::Button("Load initial position"))
	{
		from_fen = from_fen_default;
	}

	ImGui::RadioButton("Play as white", &play_color, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Play as black", &play_color, 1);
	bool new_game_st = ImGui::Button("New game");

	ImGui::Separator();
	ImGui::Checkbox("View from white perspective", &view_as_white);

	if (new_game_st)
	{
		if (minutes_to_think == 0)
		{
			minutes_to_think = 5;
			mtt_error_timer.restart();
		}
		if (tt_size_exponent == 0)
		{
			tt_size_exponent = 20;
			tt_error_timer.restart();
		}

		game g;
		try
		{
			g = fen::fen_to_game(from_fen);
		}
		catch (std::exception&)
		{
			fen_error_timer.restart();
			return;
		}

		if (thinking_thread.joinable())
			thinking_thread.join();

		// For now the computer can only play black
		if (new_game_st)
			gp = std::unique_ptr<gameplay>(new gameplay_st(minutes_to_think, color::black, g, 1ULL << tt_size_exponent));
		
		// Let the computer do the first move
		gos = gp->get_game().get_game_over_state();
		if ((play_color == 1 && g.get_game_context().turn == color::white) || (play_color == 0 && g.get_game_context().turn == color::black))
		{
			thinking_thread = std::thread([&]() {computer_pick_next_move(); });
		}

		if (play_color == 0)
		{
			view_as_white = true;
		}
		else
		{
			view_as_white = false;
		}
	}


	if (mtt_error_timer.is_running() || tt_error_timer.is_running() || fen_error_timer.is_running())
	{
		ImGui::Separator();
	}

	if (mtt_error_timer.is_running())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Using value of 5 for minutes to think");
	}

	if (tt_error_timer.is_running())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Using value of 20 for transposition table size exponent");
	}

	if (fen_error_timer.is_running())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "FEN String was invalid");
	}

}

frontend::~frontend()
{
	if(thinking_thread.joinable())
		thinking_thread.join();
}


void frontend::computer_pick_next_move()
{
	if (gp == nullptr)
	{
		thinking = false;
		return;
	}

	auto info_ = gp->pick_next_move();
	if (!info_.has_value())
	{
		printf("Error: Move had no value");
		return;
	}
	auto info = info_.value();
	pv.clear();
	for (int i = 0; i < info.principal_variation.size(); i++)
	{
		pv += std::to_string(i + 1) + ". " + gp->get_game().from_move_to_dooce_algebraic_notation(info.principal_variation[i]) + "\n";
	}
	depth = info.depth;
	searched_nodes = info.searched_nodes;
	score = info.score;
	thinking = false;
	gos = gp->get_game().get_game_over_state();
}

void frontend::update_game(const point& from_, const point& to)
{
	if (gp == nullptr || !from_.is_valid() || !to.is_valid() || thinking || gos != game_over_state::running) return;
	
	int idx_from = ops::to_idx(7-from_.x, 7-from_.y);
	int idx_to = ops::to_idx(view_as_white? 7-to.x : to.x, view_as_white?7-to.y:to.y);
	// Doesn't check for promotions right now.
	std::string from_str = sq_idx_to_str(idx_from);
	std::string to_str = sq_idx_to_str(idx_to);
	//printf("from: %s, to: %s\n", from_str.c_str(), to_str.c_str());
	std::optional<move> m;
	if (gp->get_game().get_game_context().turn == color::white)
		m = gp->get_game().from_dooce_algebraic_notation<color::white>(from_str + to_str);
	else m = gp->get_game().from_dooce_algebraic_notation<color::black>(from_str + to_str);
	if (!m.has_value() || !gp->incoming_move(m.value()))
		return;
	gos = gp->get_game().get_game_over_state();
	thinking = true;
	if(thinking_thread.joinable())
		thinking_thread.join();
	thinking_thread = std::thread([&]() {computer_pick_next_move(); });
}

void frontend::render()
{
	if (gp != nullptr)
	{
		bool open = true;
		ImGui::SetNextWindowSize({ BOARD_SIZE + 40, BOARD_SIZE + 55 });
		if (ImGui::Begin("Board", &open, ImGuiWindowFlags_NoResize))
		{
			update_click();
			update_drag();
			auto [from_,to] = update_let_go();
			update_game(from_,to);
			auto* dl = ImGui::GetWindowDrawList();
			render_board(dl);
			render_pieces(dl);
			ImGui::End();
		}
		if (!open)
		{
			if (thinking_thread.joinable())
				thinking_thread.join();
			gp = nullptr;
		}

		ImGui::Begin("Info");
		render_info(); 
		ImGui::End();

	}

	ImGui::Begin("Options");
	render_gui();
	ImGui::End();

}
