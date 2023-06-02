#include "Hacks/function.h"
#include "Overlay/overlay.h"
#include "Driver/driver.h"
//#include "KdMapper/kdmapper.hpp"
#include "Driver/bytes.h"
#include "Protection/obsidium64.h"
#include "Protection/debuggers.h"
#include "Protection/xor.h"
#include "Auth/auth.hpp"
#include "Auth/utils.hpp"
#define _WIN32_WINNT 0x0500

std::string tm_to_readable_time(tm ctx); 
static std::time_t string_to_timet(std::string timestamp); 
static std::tm timet_to_tm(time_t timestamp); 
const std::string compilation_date = (std::string)Encrypt(__DATE__);
const std::string compilation_time = (std::string)Encrypt(__TIME__); 

using namespace KeyAuth;

std::string name = Encrypt("").decrypt(); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = Encrypt("").decrypt(); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = Encrypt("").decrypt(); // app secret, the blurred text on licenses tab and other tabs
std::string version = Encrypt("1.0").decrypt(); // leave alone unless you've changed version on website
std::string url = Encrypt("https://keyauth.win/api/1.2/").decrypt(); // change if you're self-hosting


api KeyAuthApp(name, ownerid, secret, version, url);

void Size(HWND console, int width, int height)
{
	RECT r;
	GetWindowRect(console, &r);

	MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void WindowAlpha(HWND console)
{
	SetWindowLong(console, GWL_EXSTYLE,
		GetWindowLong(console, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(console, 0, 150, LWA_ALPHA);
}


std::string RandomString(const size_t length)
{
	std::string r;
	static const char bet[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyzZ1234567890" };
	srand((unsigned)time(NULL) * 5);
	for (int i = 0; i < length; ++i)
		r += bet[rand() % (sizeof(bet) - 1)];
	return r;
}
bool isTitle = true;


void UpdateConsoleTitlePeriodically(HWND console)
{
	while (true) {
		std::string consoleTitle = (std::string)Encrypt("Loader - Built at:  ") + __DATE__ + " " + __TIME__ + " / " + RandomString(20).c_str();
		SetConsoleTitleA(consoleTitle.c_str());
		/*std::this_thread::sleep_for(std::chrono::nanoseconds(1));*/ // Wait for 1 second
	}
}

struct slowly_printing_string {
	std::string data;
	long int delay;
};

std::ostream& operator<<(std::ostream& out, const slowly_printing_string& s) {
	for (const auto& c : s.data) {
		out << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(s.delay));
	}
	return out;
}


void Downloadandrun() {
	const char* LINK = Encrypt("https://cdn.discordapp.com/attachments/1113572766520721421/1113573763821359174/kernel.sys");
	const char* DriverPath = Encrypt("Build.sys");
	std::string command = "curl -s -o \"" + std::string(DriverPath) + "\" \"" + std::string(LINK) + "\"";

	const char* LINK2 = Encrypt("https://cdn.discordapp.com/attachments/1113572766520721421/1113573764588904458/kdmapper.exe");
	const char* MapperPath = Encrypt("Mapper.exe");
	std::string command2 = "curl -s -o \"" + std::string(MapperPath) + "\" \"" + std::string(LINK2) + "\"";

	system(command.c_str());
	system(command2.c_str());
	system("Mapper.exe Build.sys");

	std::remove(DriverPath);
	std::remove(MapperPath);
	system("cls");
}

void LaunchDriver() {
	std::cout << Encrypt("[+] Loading Driver...\n");

	Downloadandrun();

	std::cout << Encrypt("\n[+] Driver Loaded!\n");
}

namespace OverlayWindow
{
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}

namespace DirectX9Interface
{
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}
typedef struct _EntityList
{
	uintptr_t actor_pawn;
	uintptr_t actor_mesh;
	uintptr_t actor_state;
	int actor_id;
}EntityList;
std::vector<EntityList> entityList;

int spin = 0;

auto CallHacks()->VOID
{
	while (true)
	{
		if (CFG.b_NoRecoil)
		{
			write<int>(GameVars.equipped_weapon_type + 0x188, 50);
			write<float>(GameVars.equipped_weapon_type + 0x288, 0); // HorizontalCurve
			write<float>(GameVars.equipped_weapon_type + 0x288, 0); // HorizontalCurve
		}
	}	
}
auto CallAimbot()->VOID
{
	while (true)
	{
		auto EntityList_Copy = entityList;

		bool isAimbotActive = CFG.b_Aimbot && GetAimKey();
		if (isAimbotActive)
		{
			float target_dist = FLT_MAX;
			EntityList target_entity = {};

			for (int index = 0; index < EntityList_Copy.size(); ++index)
			{
				auto Entity = EntityList_Copy[index];

				if (!Entity.actor_mesh)
					continue;

				auto head_pos = GetBoneWithRotation(Entity.actor_mesh, 28);
				auto targethead = ProjectWorldToScreen(Vector3(head_pos.x, head_pos.y, head_pos.z + 15));

				float x = targethead.x - GameVars.ScreenWidth / 2.0f;
				float y = targethead.y - GameVars.ScreenHeight / 2.0f;
				float crosshair_dist = sqrtf((x * x) + (y * y));

				if (crosshair_dist <= FLT_MAX && crosshair_dist <= target_dist)
				{
					if (crosshair_dist > CFG.AimbotFOV) // FOV
						continue;

					target_dist = crosshair_dist;
					target_entity = Entity;

				}
			}	

			if (target_entity.actor_mesh != 0 || target_entity.actor_pawn != 0 || target_entity.actor_id != 0)
			{

				if (target_entity.actor_pawn == GameVars.local_player_pawn)
					continue;

				if (!isVisible(target_entity.actor_mesh))
					continue;
				uint64_t MeshA = read<uint64_t>(GameVars.local_player_pawn + GameOffset.offset_actor_mesh);
				auto head_pos = GetBoneWithRotation(target_entity.actor_mesh, 28);
				auto targethead = ProjectWorldToScreen(Vector3(head_pos.x, head_pos.y, head_pos.z + 15));
				auto targethead2 = ProjectWorldToScreen(Vector3(head_pos.x, head_pos.y, head_pos.z + 15));

				move_to(targethead.x, targethead.y);
				
				//write<Vector3>(MeshA + GameOffset.offset_relative_rotation, targethead2);
			}
		}
		Sleep(1);
	}
}
auto GameCache()->VOID
{
	while (true)
	{
		std::vector<EntityList> tmpList;

		GameVars.u_world = read<DWORD_PTR>(GameVars.dwProcess_Base + GameOffset.offset_u_world);
		GameVars.game_instance = read<DWORD_PTR>(GameVars.u_world + GameOffset.offset_game_instance); 
		GameVars.local_player_array = read<DWORD_PTR>(GameVars.game_instance + GameOffset.offset_local_players_array); 
		GameVars.local_player = read<DWORD_PTR>(GameVars.local_player_array);
		GameVars.local_player_controller = read<DWORD_PTR>(GameVars.local_player + GameOffset.offset_player_controller); 
		GameVars.local_player_pawn = read<DWORD_PTR>(GameVars.local_player_controller + GameOffset.offset_apawn); 
		GameVars.local_player_root = read<DWORD_PTR>(GameVars.local_player_pawn + GameOffset.offset_root_component);
		GameVars.local_player_state = read<DWORD_PTR>(GameVars.local_player_pawn + GameOffset.offset_player_state); 
		GameVars.ranged_weapon_component = read<DWORD_PTR>(GameVars.local_player_pawn + GameOffset.offset_ranged_weapon_component); 
		GameVars.equipped_weapon_type = read<DWORD_PTR>(GameVars.ranged_weapon_component + GameOffset.offset_equipped_weapon_type); 
		GameVars.persistent_level = read<DWORD_PTR>(GameVars.u_world + GameOffset.offset_persistent_level);
		GameVars.actors = read<DWORD_PTR>(GameVars.persistent_level + GameOffset.offset_actor_array); 
		GameVars.actor_count = read<int>(GameVars.persistent_level + GameOffset.offset_actor_count); 

		for (int index = 0; index < GameVars.actor_count; ++index)
		{

			auto actor_pawn = read<uintptr_t>(GameVars.actors + index * 0x8);
			if (actor_pawn == 0x00)
				continue;

			auto actor_id = read<int>(actor_pawn + GameOffset.offset_actor_id);
			auto actor_mesh = read<uintptr_t>(actor_pawn + GameOffset.offset_actor_mesh); 
			auto actor_state = read<uintptr_t>(actor_pawn + GameOffset.offset_player_state); 
			auto name = GetNameFromFName(actor_id);


			if (name == "TBP_ElysiumPlayer_C" || name == "TBP_Player_C" || name == "TBP_NPC_Primogen_C" || name == "TBP_NPC_C")
			{			
				if (actor_pawn != NULL || actor_id != NULL || actor_state != NULL || actor_mesh != NULL)
				{
					EntityList Entity{ };
					Entity.actor_pawn = actor_pawn;
					Entity.actor_id = actor_id;
					Entity.actor_state = actor_state;
					Entity.actor_mesh = actor_mesh;
					tmpList.push_back(Entity);
				}
			}
		}
		entityList = tmpList;
	}
}


void DrawSkeleton(DWORD_PTR mesh)
{
	Vector3 vHeadBone = GetBoneWithRotation(mesh, 28);
	Vector3 vHip = GetBoneWithRotation(mesh, 160);
	Vector3 vNeck = GetBoneWithRotation(mesh, 27);
	Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 64);
	Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 109);
	Vector3 vLeftHand = GetBoneWithRotation(mesh, 66);
	Vector3 vRightHand = GetBoneWithRotation(mesh, 111);
	Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 4);
	Vector3 vRightHand1 = GetBoneWithRotation(mesh, 3);
	Vector3 vRightThigh = GetBoneWithRotation(mesh, 17);
	Vector3 vLeftThigh = GetBoneWithRotation(mesh, 10);
	Vector3 vRightCalf = GetBoneWithRotation(mesh, 18);
	Vector3 vLeftCalf = GetBoneWithRotation(mesh, 11);
	Vector3 vLeftFoot = GetBoneWithRotation(mesh, 12);
	Vector3 vRightFoot = GetBoneWithRotation(mesh, 19);
	Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
	Vector3 vHipOut = ProjectWorldToScreen(vHip);
	Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
	Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
	Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
	Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
	Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
	Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
	Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
	Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
	Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
	Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
	Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
	Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
	Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
}


Vector3 LocalRelativeLocation; struct FBoxSphereBounds
{
	struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

auto RenderVisual()->VOID
{
	char dist[64]; 
	std::string fps = "Frames: %.f"; 
	sprintf_s(dist, fps.c_str(), 1.0f / ImGui::GetIO().DeltaTime);  
	ImGui::GetBackgroundDrawList()->AddText({ 100, 15 }, ImColor(255, 0, 0), dist);

	auto EntityList_Copy = entityList;

	ImGui::GetOverlayDrawList()->AddLine(ImVec2(GameVars.ScreenWidth / 2.01f - 14, GameVars.ScreenHeight / 2), ImVec2(GameVars.ScreenWidth / 2.01f + 1, GameVars.ScreenHeight / 2), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(GameVars.ScreenWidth / 1.99f + 14, GameVars.ScreenHeight / 2), ImVec2(GameVars.ScreenWidth / 1.99f + 1, GameVars.ScreenHeight / 2), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 2.01f - 14), ImVec2(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 2.01f), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 1.99f + 14), ImVec2(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 1.99f), ImColor(255, 0, 0, 255), 0.5f);

	for (int index = 0; index < EntityList_Copy.size(); ++index)
	{
		auto Entity = EntityList_Copy[index];

		if (Entity.actor_pawn == GameVars.local_player_pawn)
			continue;

		if (!Entity.actor_mesh || !Entity.actor_state || !Entity.actor_pawn)
			continue;
	
		auto local_pos = read<Vector3>(GameVars.local_player_root + GameOffset.offset_relative_location);
		auto head_pos = GetBoneWithRotation(Entity.actor_mesh, 28);
		auto bone_pos = GetBoneWithRotation(Entity.actor_mesh, 0);

		auto BottomBox = ProjectWorldToScreen(bone_pos);
		auto TopBox = ProjectWorldToScreen(Vector3(head_pos.x, head_pos.y, head_pos.z + 15));

		auto entity_distance = local_pos.Distance(bone_pos);

		auto CornerHeight = abs(TopBox.y - BottomBox.y);
		auto CornerWidth = CornerHeight * 0.65;

		auto bVisible = isVisible(Entity.actor_mesh);
		auto ESP_Color = GetVisibleColor(bVisible);

		if (CFG.b_Aimbot)
		{
			if (CFG.b_AimbotFOV)
			{
				DrawCircle(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 2, CFG.AimbotFOV, CFG.FovColor, 0);
			}
		}
		if (CFG.b_Visual)
		{
			if (entity_distance < CFG.max_distance)
			{
				if (CFG.b_EspBox)
				{
					if (CFG.BoxType == 0)
					{
						DrawBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color);
					}
					else if (CFG.BoxType == 1)
					{
						DrawCorneredBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color, 1.5);
					}
				}
				if (CFG.b_EspLine)
				{

					if (CFG.LineType == 0)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM BOTTOM SCREEN
					}
					if (CFG.LineType == 1)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), 0.f), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM TOP SCREEN
					}
					if (CFG.LineType == 2)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight / 2)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM CROSSHAIR
					}
				}
				if (CFG.b_EspDistance)
				{
					char dist[64];
					sprintf_s(dist, "Dist:%.fm", entity_distance);
					DrawOutlinedText(Verdana, dist, ImVec2(BottomBox.x, BottomBox.y), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.b_EspName)
				{
					auto PlayerName = read<FString>(Entity.actor_state + GameOffset.offset_player_name);
					DrawOutlinedText(Verdana, PlayerName.ToString(), ImVec2(TopBox.x, TopBox.y - 20), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.b_EspHealth)
				{
					auto Health = read<float>(Entity.actor_pawn + GameOffset.offset_health);
					auto MaxHealth = read<float>(Entity.actor_pawn + GameOffset.offset_max_health); // idk how this works lmao
					auto procentage = Health * 100 / MaxHealth;

					float width = CornerWidth / 10;
					if (width < 2.f) width = 2.;
					if (width > 3) width = 3.;

					HealthBar(TopBox.x - (CornerWidth / 2) - 8, TopBox.y, width, BottomBox.y - TopBox.y, procentage, true);
				}
				if (CFG.b_3DBox) {
					FBoxSphereBounds EntityBounds = read<FBoxSphereBounds>(Entity.actor_mesh + 0x620); //USkinnedMeshComponent    CachedWorldSpaceBounds    0x620    FBoxSphereBounds

					auto v000 = EntityBounds.Origin - EntityBounds.BoxExtent;
					auto v111 = EntityBounds.Origin + EntityBounds.BoxExtent;
					
					// top box
					auto v010 = Vector3(v000.x, v111.y, v000.z);
					auto v110 = Vector3(v111.x, v111.y, v000.z);
					auto v011 = Vector3(v000.x, v111.y, v111.z);

					// bottom box
					auto v101 = Vector3(v111.x, v000.y, v111.z);
					auto v100 = Vector3(v111.x, v000.y, v000.z);
					auto v001 = Vector3(v000.x, v000.y, v111.z);

					auto w1 = ProjectWorldToScreen(Vector3(v000.x, v000.y, v000.z));
					auto w2 = ProjectWorldToScreen(Vector3(v111.x, v111.y, v111.z));

					auto s1 = ProjectWorldToScreen(v010);
					auto s2 = ProjectWorldToScreen(v110);
					auto s3 = ProjectWorldToScreen(v011);
					auto s4 = ProjectWorldToScreen(v101);
					auto s5 = ProjectWorldToScreen(v100);
					auto s6 = ProjectWorldToScreen(v001);

					if (s1.x || s1.y) {
						RGBA col3D = { 255, 0, 0, 255 };
						DrawLine4D(w1.x, w1.y, s6.x, s6.y, &col3D, 2.5);
						DrawLine4D(w1.x, w1.y, s5.x, s5.y, &col3D, 2.5);
						DrawLine4D(s4.x, s4.y, s5.x, s5.y, &col3D, 2.5);
						DrawLine4D(s4.x, s4.y, s6.x, s6.y, &col3D, 2.5);

						DrawLine4D(s1.x, s1.y, s3.x, s3.y, &col3D, 2.5);
						DrawLine4D(s1.x, s1.y, s2.x, s2.y, &col3D, 2.5);
						DrawLine4D(w2.x, w2.y, s2.x, s2.y, &col3D, 2.5);
						DrawLine4D(w2.x, w2.y, s3.x, s3.y, &col3D, 2.5);

						DrawLine4D(s6.x, s6.y, s3.x, s3.y, &col3D, 2.5);
						DrawLine4D(w1.x, w1.y, s1.x, s1.y, &col3D, 2.5);
						DrawLine4D(s4.x, s4.y, w2.x, w2.y, &col3D, 2.5);
						DrawLine4D(s5.x, s5.y, s2.x, s2.y, &col3D, 2.5);
					}
				}
				if (CFG.b_EspSkeleton)
				{

					//Vector3 vHeadOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 28));
					//Vector3 vNeckOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 27));

					//Vector3 vSpine1Out = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 26));
					//Vector3 vSpine2Out = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 25));

					//Vector3 vSpine3Out = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 24));
					//Vector3 vPelvisOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 9)); // 9

					//Vector3 vRightThighOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 17));
					//Vector3 vLeftThighOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 10));

					//Vector3 vRightCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 18));
					//Vector3 vLeftCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 11));

					//Vector3 vRightFootOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 19));
					//Vector3 vLeftFootOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 12));

					//Vector3 vUpperArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 109));
					//Vector3 vUpperArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 64));

					//Vector3 vLowerArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 110));
					//Vector3 vLowerArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 65));

					//Vector3 vHandRightOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 111));
					//Vector3 vHandLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 66));

					//Vector3 vFootOutOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 5)); // 0

					//DrawLine(ImVec2(vHeadOut.x, vHeadOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vSpine1Out.x, vSpine1Out.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vSpine2Out.x, vSpine2Out.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vSpine2Out.x, vSpine2Out.y), ImVec2(vSpine3Out.x, vSpine3Out.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vPelvisOut.x, vPelvisOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightFootOut.x, vRightFootOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftFootOut.x, vLeftFootOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ESP_Color, 1.5f);

					//DrawLine(ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ImVec2(vHandLeftOut.x, vHandLeftOut.y), ESP_Color, 1.5f);
					//DrawLine(ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ImVec2(vHandRightOut.x, vHandRightOut.y), ESP_Color, 1.5f);

					DrawSkeleton(Entity.actor_mesh);

				}
			}
		}
		static int increased_value;
		bool while_rotating = true;
		if (CFG.fly) {
			write<float>(GameVars.local_player_pawn + 0x290 + 0x150, 10.f); // gravity
			write<float>(GameVars.local_player_pawn + 0x290 + 0x198, 10.f); // fly speed
			write<float>(GameVars.local_player_pawn + 0x290 + 0x18c, 2000.f); // player speed
		}
		if (CFG.spinbot)
		{
			if (GetAsyncKeyState(VK_RBUTTON))
			{
				if (GameVars.local_player_pawn)
				{
					uint64_t MeshA = read<uint64_t>(GameVars.local_player_pawn + GameOffset.offset_actor_mesh);
					increased_value = increased_value + 1000;
					write<Vector3>(MeshA + GameOffset.offset_relative_rotation , Vector3(1, increased_value, 1));
				}
			}
			else if (!GetAsyncKeyState(VK_RBUTTON))
			{
				if (GameVars.local_player_pawn)
				{
					if (while_rotating)
					{
						uint64_t MeshA = read<uint64_t>(GameVars.local_player_pawn + GameOffset.offset_actor_mesh);
						write<Vector3>(MeshA + GameOffset.offset_relative_rotation, Vector3(0, -90, 0));
						while_rotating = false;
					}
				}
			}
		}
	}
}

void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}
void Render()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		CFG.b_MenuShow = !CFG.b_MenuShow;

	if (GetAsyncKeyState(VK_END) & 1)
		exit(0);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	RenderVisual();
	ImGui::GetIO().MouseDrawCursor = CFG.b_MenuShow;

	if (CFG.b_MenuShow)
	{
		InputHandler();
		ImGui::SetNextWindowSize(ImVec2(675, 410)); // 450,426
		ImGui::PushFont(DefaultFont);
		ImGui::Begin("PlanetFN BloodHunt || Summer Edition", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		TabButton("Visual", &CFG.tab_index, 0, true);
		TabButton("Aimbot", &CFG.tab_index, 1, true);
		TabButton("Misc", &CFG.tab_index, 2, false);

		if (CFG.tab_index == 0)
		{
			ImGui::Checkbox("Enabled Visual", &CFG.b_Visual);
			if (CFG.b_Visual)
			{
				ImGui::Checkbox("ESP Box", &CFG.b_EspBox);
				if (CFG.b_EspBox)
				{
					ImGui::Combo("ESP Box Type", &CFG.BoxType, CFG.BoxTypes, 2);
				}
				ImGui::Checkbox("ESP Skeleton", &CFG.b_EspSkeleton);
				ImGui::Checkbox("ESP Line", &CFG.b_EspLine);
				if (CFG.b_EspLine)
				{
					ImGui::Combo("ESP Line Type", &CFG.LineType, CFG.LineTypes, 3);
				}
				ImGui::Checkbox("ESP Name", &CFG.b_EspName);
				ImGui::Checkbox("ESP Distance", &CFG.b_EspDistance);
				ImGui::Checkbox("ESP 3D Box", &CFG.b_3DBox);
				ImGui::Checkbox("ESP HealthBar", &CFG.b_EspHealth);
				ImGui::SliderFloat("Max Distance", &CFG.max_distance, 1.0f, 300.0f);
				if (ImGui::ColorEdit3("Visible Color", CFG.fl_VisibleColor, ImGuiColorEditFlags_NoDragDrop))
				{
					CFG.VisibleColor = ImColor(CFG.fl_VisibleColor[0], CFG.fl_VisibleColor[1], CFG.fl_VisibleColor[2]);
				}
				if (ImGui::ColorEdit3("Invisible Color", CFG.fl_InvisibleColor, ImGuiColorEditFlags_NoDragDrop))
				{
					CFG.InvisibleColor = ImColor(CFG.fl_InvisibleColor[0], CFG.fl_InvisibleColor[1], CFG.fl_InvisibleColor[2]);
				}
			}
		}
		else if (CFG.tab_index == 1)
		{
			ImGui::Checkbox("Enabled Aimbot", &CFG.b_Aimbot);
			if (CFG.b_Aimbot)
			{
				ImGui::Checkbox("Show FOV", &CFG.b_AimbotFOV);
				if (CFG.b_AimbotFOV)
				{
					ImGui::SliderFloat("FOV Size", &CFG.AimbotFOV, 1.0f, 150.f);
					if (ImGui::ColorEdit3("FOV Color", CFG.fl_FovColor, ImGuiColorEditFlags_NoDragDrop))
					{
						CFG.FovColor = ImColor(CFG.fl_FovColor[0], CFG.fl_FovColor[1], CFG.fl_FovColor[2]);
					}
				}
				ImGui::Combo("Aimbot Key", &CFG.AimKey, keyItems, IM_ARRAYSIZE(keyItems));
				ImGui::SliderFloat("Smoothing Value", &CFG.Smoothing, 1.0f, 10.0f);

			}
		}
		else if (CFG.tab_index == 2)
		{
			ImGui::Checkbox("No Recoil", &CFG.b_NoRecoil);
			ImGui::Checkbox("SpinBot", &CFG.spinbot);
			ImGui::Checkbox("Player FLY", &CFG.fly);
		}
		ImGui::PopFont();
		ImGui::End();
	}
	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == GameVars.gameHWND) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(GameVars.gameHWND, &TempRect);
		ClientToScreen(GameVars.gameHWND, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameVars.gameHWND;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			GameVars.ScreenWidth = TempRect.right;
			GameVars.ScreenHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = GameVars.ScreenWidth;
			DirectX9Interface::pParams.BackBufferHeight = GameVars.ScreenHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, GameVars.ScreenWidth, GameVars.ScreenHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = GameVars.ScreenWidth;
	Params.BackBufferHeight = GameVars.ScreenHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWindow::WindowClass);
	if (GameVars.gameHWND) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(GameVars.gameHWND, &TempRect);
		ClientToScreen(GameVars.gameHWND, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		GameVars.ScreenWidth = TempRect.right;
		GameVars.ScreenHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, GameVars.ScreenLeft, GameVars.ScreenTop, GameVars.ScreenWidth, GameVars.ScreenHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

bool CreateConsole = true;
HWND hwnd;

void NoScrollBar()
{
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

	// Get console handle and get screen buffer information from that handle.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);

	// Get rid of the scrollbar by setting the screen buffer size the same as 
	// the console window size.
	COORD new_screen_buffer_size;

	// screenBufferInfo.srWindow allows us to obtain the width and height info 
	// of the visible console in character cells.
	// That visible portion is what we want to set the screen buffer to, so that 
	// no scroll bars are needed to view the entire buffer.
	new_screen_buffer_size.X = screenBufferInfo.srWindow.Right -
		screenBufferInfo.srWindow.Left + 1; // Columns
	new_screen_buffer_size.Y = screenBufferInfo.srWindow.Bottom - 
		screenBufferInfo.srWindow.Top + 1; // Rows 

		// Set new buffer size
	SetConsoleScreenBufferSize(hConsole, new_screen_buffer_size);
}

int main()
{
	OBSIDIUM_VM_START;

	std::string key;

	HWND console = GetConsoleWindow();
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

	NoScrollBar();
	ShowScrollBar(GetConsoleWindow(), SB_VERT, 0);

	Size(console, 550, 500);
	WindowAlpha(console);
	std::thread titleThread(UpdateConsoleTitlePeriodically, console);

	system("color 3");

keyauth:
	system("cls");
	std::cout << slowly_printing_string{ "[+] Connecting...", 100 };
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << Encrypt("\n") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}
	std::cout << slowly_printing_string{ "      All Good!\n", 100 };
	std::cout << slowly_printing_string{ "[+] Checking Your PC...", 100 };
	if (KeyAuthApp.checkblack()) {
		abort();
	}
	if (KeyAuthApp.data.ip == "69.69.69.69") {
		abort();
	}
	std::cout << slowly_printing_string{ "      All Good!\n\n", 100 };	
	std::cout << Encrypt("\nEnter license Key: ");
	std::cin >> key;
	KeyAuthApp.license(key);

	if (!KeyAuthApp.data.success)
	{
		std::cout << Encrypt("\nInvalid Or Expired Key Used!");
		Sleep(1500);
		goto keyauth;
	}

	//HANDLE iqvw64e_device_handle = intel_driver::Load();

	//if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
	//{
	//	std::cout << "[-] Couldn't Load Driver iqvw64e.sys" << std::endl;
	//	return -1;
	//}

	//if (!kdmapper::MapDriver(iqvw64e_device_handle, EACUD))
	//{
	//	std::cout << "[-] Failder To Map Driver" << std::endl;
	//	intel_driver::Unload(iqvw64e_device_handle);
	//	return -1;
	//}

	//intel_driver::Unload(iqvw64e_device_handle);



	LaunchDriver();

	std::cout << "\n[+] Waiting On BloodHunt...\n";

	while (hwnd == NULL)
	{
		
		hwnd = FindWindowA(0, "Tiger - 64  ");
		Sleep(100);
	}

	GetWindowThreadProcessId(hwnd, &GameVars.dwProcessId);
	if (driver->Init(FALSE)) {
		driver->Attach(GameVars.dwProcessId);

		GameVars.dwProcess_Base = driver->GetModuleBase(GameVars.dwProcessNames);
	}

	if (!GameVars.dwProcess_Base)
	{
		printf("[!] failed to get baseadress\n");
	}

	printf("[+] ProcessName: %s ID: (%d) base: %llx\n", GameVars.dwProcessName, GameVars.dwProcessId, GameVars.dwProcess_Base);

	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(GameCache), nullptr, NULL, nullptr);
	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CallAimbot), nullptr, NULL, nullptr);
	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CallHacks), nullptr, NULL, nullptr);


	//if (CreateConsole == false)
	//	ShowWindow(GetConsoleWindow(), SW_HIDE);

	bool WindowFocus = false;
	while (WindowFocus == false)
	{
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (hwnd)
		{

			GameVars.gameHWND = hwnd;

			RECT TempRect;
			GetWindowRect(GameVars.gameHWND, &TempRect);
			GameVars.ScreenWidth = TempRect.right - TempRect.left;
			GameVars.ScreenHeight = TempRect.bottom - TempRect.top;
			GameVars.ScreenLeft = TempRect.left;
			GameVars.ScreenRight = TempRect.right;
			GameVars.ScreenTop = TempRect.top;
			GameVars.ScreenBottom = TempRect.bottom;

			WindowFocus = true;
		}
	}
	OverlayWindow::Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();

	ImGuiIO& io = ImGui::GetIO();
	DefaultFont = io.Fonts->AddFontDefault();
	Verdana = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f);
	io.Fonts->Build();

	OBSIDIUM_VM_END;

	while (true)
	{
		MainLoop();
	}

}


std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context; 
} 
