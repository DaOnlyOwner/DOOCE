#define STB_IMAGE_IMPLEMENTATION
#include "frontend.h"
#define BOARD_SIZE 640
#include <cstdio>
#include "stb_image.h"
#include "GLFW/glfw3.h"
#include <tuple>

namespace
{
	std::tuple<bool,GLuint,int,int> load_image(const char* path)
	{
		int w, h;
		auto data = stbi_load(path, &w, &h, nullptr, 4);
		if (data == nullptr)
			return { false,0,0,0 };
		GLuint img;
		glGenTextures(1, &img);
		glBindTexture(GL_TEXTURE_2D, img);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return { true,img,w,h };
	}
}

frontend::frontend()
{

}

void frontend::render_board(ImDrawList* dl)
{
	constexpr int sq_size = BOARD_SIZE / 8;
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
}


void frontend::render()
{

	ImGui::SetNextWindowSize({ BOARD_SIZE+15, BOARD_SIZE+30});
	//ImGui::SetNextWindowPos({0.f,0.f}, 1);
	ImGui::Begin("Board",false,ImGuiWindowFlags_NoResize);
	auto* dl = ImGui::GetWindowDrawList();
	render_board(dl);
	ImGui::End();
}
