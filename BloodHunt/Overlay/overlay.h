#include <windows.h>
#include <tchar.h>
#include <memory>
#include <thread>
#include <functional>
#include <array>
#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <sstream>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"d3dcompiler") 

using namespace std;




ImFont* Verdana, * DefaultFont;

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf);
	delete[]pwBuf;
	delete[]pBuf;
	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}

auto DrawCrossHair(const FLOAT aSize, ImU32 aColor)-> VOID
{
	auto vList = ImGui::GetOverlayDrawList();

	vList->AddLine({ GameVars.ScreenCenterX ,GameVars.ScreenCenterY - (aSize + 1) }, { GameVars.ScreenCenterX ,GameVars.ScreenCenterY + (aSize + 1) }, aColor, 2);
	vList->AddLine({ GameVars.ScreenCenterX - (aSize + 1),GameVars.ScreenCenterY }, { GameVars.ScreenCenterX + (aSize + 1)  ,GameVars.ScreenCenterY }, aColor, 2);
}

auto DrawLine(const ImVec2& aPoint1, const ImVec2 aPoint2, ImU32 aColor, const FLOAT aLineWidth) -> VOID
{
	auto vList = ImGui::GetOverlayDrawList();
	vList->AddLine(aPoint1, aPoint2, aColor, aLineWidth);
}

void DrawLine4D(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}


auto DrawBox(float x, float y, float w, float h, ImColor color)-> VOID
{
	DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.3f); // top 
	DrawLine(ImVec2(x, y - 1.3f), ImVec2(x, y + h + 1.4f), color, 1.3f); // left
	DrawLine(ImVec2(x + w, y - 1.3f), ImVec2(x + w, y + h + 1.4f), color, 1.3f);  // right
	DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.3f);   // bottom 
}
auto RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)-> VOID
{
	auto vList = ImGui::GetOverlayDrawList();
	vList->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

auto HealthBar(float x, float y, float w, float h, int phealth, bool Outlined)-> VOID
{
	auto vList = ImGui::GetOverlayDrawList();

	int healthValue = max(0, min(phealth, 100));

	ImColor barColor = ImColor
	(
		min(510 * (100 - healthValue) / 100, 255), min(510 * healthValue / 100, 255),
		25,
		255
	);
	if (Outlined)
		vList->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), ImColor(0.f, 0.f, 0.f), 0.0f, 0, 1.0f);

	RectFilled(x, y, x + w, y + (int)(((float)h / 100.0f) * (float)phealth), barColor, 0.0f, 0);
}

auto DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center) -> VOID
{
	ImGui::PushFont(Verdana);
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int index = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * index), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{//
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * index), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (index + 1);
		index++;
	}
	ImGui::PopFont();
}

auto DrawCorneredBox(float X, float Y, float W, float H, const ImU32& color, float thickness) -> VOID
{
	auto vList = ImGui::GetOverlayDrawList();

	float lineW = (W / 3);
	float lineH = (H / 3);
	//black outlines
	auto col = ImGui::GetColorU32(color);

	//corners
	vList->AddLine(ImVec2(X, Y - thickness / 2), ImVec2(X, Y + lineH), col, thickness);//top left
	vList->AddLine(ImVec2(X - thickness / 2, Y), ImVec2(X + lineW, Y), col, thickness);

	vList->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W + thickness / 2, Y), col, thickness);//top right horizontal
	vList->AddLine(ImVec2(X + W, Y - thickness / 2), ImVec2(X + W, Y + lineH), col, thickness);

	vList->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H + (thickness / 2)), col, thickness);//bot left
	vList->AddLine(ImVec2(X - thickness / 2, Y + H), ImVec2(X + lineW, Y + H), col, thickness);

	vList->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W + thickness / 2, Y + H), col, thickness);//bot right
	vList->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H + (thickness / 2)), col, thickness);

}

auto DrawCircle(const ImVec2& aPoint, const FLOAT aR, ImU32 aColor, const FLOAT aLineWidth) -> VOID
{
	auto vList = ImGui::GetOverlayDrawList();
	vList->AddCircle(aPoint, aR, aColor, 120, aLineWidth);
}
auto DrawCircle(float x, float y, float radius, ImVec4 color, int segments)-> VOID
{
	auto vList = ImGui::GetOverlayDrawList();
	vList->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}
auto DrawString(const ImVec2& aPos, const std::string& aString, ImU32 aColor) -> VOID
{
	auto vList = ImGui::GetOverlayDrawList();
	vList->AddText(aPos, aColor, aString.data());
}
auto TabButton(const char* label, int* index, int val, bool sameline) -> VOID
{
	if (*index == val)
	{
		if (ImGui::Button(label, ImVec2(80, 25)))
			*index = val;
		if (sameline)
			ImGui::SameLine();
	}
	else
	{
		if (ImGui::Button(label, ImVec2(80, 25)))
			*index = val;
		if (sameline)
			ImGui::SameLine();
	}
}

class Color
{
public:
	RGBA NiggaGreen = { 128, 224, 0, 200 };
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 0,255,0,255 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,160 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA cyan = { 0, 255, 231, 255 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };
	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };
	RGBA FiledBox = { 0, 0, 0, 125 };
	RGBA Plum = { 221,160,221,160 };
};

Color Col;