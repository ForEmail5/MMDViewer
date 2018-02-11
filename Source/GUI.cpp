#include "GUI.h"
#include "CeShi.h"
#include "Dialog.h"
#include "MMDViewer.h"

GUI::GUI(MMDViewer* viewer, RenderWindow* window, D3DGraphicsContext* gc)
: mmd_viewer(viewer)
, window(window)
, gc(gc)
{
	/* ��ʼ�� ImGui */
	ImGui_ImplDX11_Init(RenderWindow::GetHwnd(), gc->GetDevice(), gc->GetDeviceContext());

	this->InitGUIFont();

	str_null = mmd::UTF16ToUTF8(L"��");
	str_play = mmd::UTF16ToUTF8(L"����");
	str_stop = mmd::UTF16ToUTF8(L"ֹͣ");
	str_model = mmd::UTF16ToUTF8(L"ģ��: ");
	str_bone_motion = mmd::UTF16ToUTF8(L"��������: ");
	str_camera_motion = mmd::UTF16ToUTF8(L"��ͷ����:" );
}

GUI::~GUI()
{
	ImGui_ImplDX11_Shutdown();
}

void GUI::InitGUIFont()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChinese());
}

void GUI::DrawDemoWindow()
{
	ImGui::ShowTestWindow(0);
}

void GUI::DrawRightWindow()
{
	int width = window->GetRenderWindowWidth() - window->GetRenderRect().w;
	int height = window->GetRenderRect().h;

	/* ���˵��� */
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu(mmd::UTF16ToUTF8(L"�ļ�").c_str()) )
		{
			if ( ImGui::MenuItem(mmd::UTF16ToUTF8(L"���ģ��").c_str(), "Ctrl+F") )
			{
				std::string full_path = Dialog::DGetOpenFileName("ѡ��ģ���ļ�", "ģ���ļ�(*.pmd *.pmx)\0*.pmd;*.pmx\0");
				mmd_viewer->LoadModel(full_path);
			}
			if ( ImGui::MenuItem(mmd::UTF16ToUTF8(L"��ӹ�������").c_str(), "Ctrl+B") )
			{
				std::string full_path = Dialog::DGetOpenFileName("ѡ�����ļ�", "�����ļ�(*.vmd)\0*.vmd\0");
				mmd_viewer->LoadMotion(full_path);
			}
			if ( ImGui::MenuItem(mmd::UTF16ToUTF8(L"��Ӿ�ͷ����").c_str(), "Ctrl+C") )
			{
				std::string full_path = Dialog::DGetOpenFileName("ѡ��ͷ�����ļ�", "��ͷ�����ļ�(*.vmd)\0*.vmd\0");
				mmd_viewer->LoadCameraMotion(full_path);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	/* �ұߴ��� */
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0);
	ImGui::SetNextWindowPos(ImVec2(0, 23));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("RightWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		ImGui::Text("FPS: %d\t    Drawcall: %d  ", (int)ImGui::GetIO().Framerate, gc->GetDrawCount());

		ImGui::Text(str_model.c_str()); ImGui::SameLine();
		if ( mmd_viewer->HasModel() )
		{
			ImGui::Button(mmd::UTF16ToUTF8(mmd::NativeToUTF16(mmd_viewer->GetModelName())).c_str());
		}
		else ImGui::Button(str_null.c_str());

		ImGui::Text(str_bone_motion.c_str()); ImGui::SameLine();
		if ( mmd_viewer->HasBoneMotion() )
		{
			ImGui::Button(mmd::UTF16ToUTF8(mmd::NativeToUTF16(mmd_viewer->GetBoneMotionName())).c_str());
		}
		else ImGui::Button(str_null.c_str());

		ImGui::Text(str_camera_motion.c_str()); ImGui::SameLine();
		if ( mmd_viewer->HasCameraMotion() )
		{
			ImGui::Button(mmd::UTF16ToUTF8(mmd::NativeToUTF16(mmd_viewer->GetCameraMotionName())).c_str());
		}
		else ImGui::Button(str_null.c_str());

		if ( ImGui::Button(str_play.c_str(), ImVec2(ImGui::GetWindowWidth(), 0)) )
		{
			mmd_viewer->PlayAnimation();
		}
		if ( ImGui::Button(str_stop.c_str(), ImVec2(ImGui::GetWindowWidth(), 0)) )
		{
			mmd_viewer->StopAnimation();
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void GUI::BeginDraw()
{
	ImGui_ImplDX11_NewFrame();

	float cinv = 1.0f / 255.0f;

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(151 * cinv, 151 * cinv, 151 * cinv, 255 * cinv));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(238 * cinv, 238 * cinv, 242 * cinv, 255 * cinv));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(238 * cinv, 238 * cinv, 242 * cinv, 255 * cinv));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 100 * cinv));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 128 * cinv));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 128 * cinv));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0, 0, 0, 80 * cinv));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0, 0, 0, 90 * cinv));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0, 0, 0, 90 * cinv));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
}

void GUI::Draw()
{
	CeShi::GetInstance()->GUIDraw();

	//this->DrawDemoWindow();
	this->DrawRightWindow();

	ImGui::PopStyleColor(10);

	/* ��ʼ ImGui ��Ⱦ */
	ImGui::Render();
}