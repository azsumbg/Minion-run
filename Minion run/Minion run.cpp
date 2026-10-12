#include "framework.h"
#include "Minion run.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "D2BMPLOADER.h"
#include "ErrH.h"
#include "FCheck.h"
#include "gifresizer.h"
#include "PlatServ.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "platserv.lib")

constexpr wchar_t bWinClassName[]{ L"Minions" };

constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t snd_file[]{ L".\\res\\snd\\main.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int no_record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int record{ 2003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HICON bIcon{ nullptr };
HCURSOR bCursor{ nullptr };
HCURSOR outCursor{ nullptr };
PAINTSTRUCT PaintStuct{};
HDC bPaint{ nullptr };
HWND bHwnd{ nullptr };
MSG bMsg{};
BOOL bRet{ 0 };

UINT bTimer{ 0 };

POINT cur_pos{};

float x_scale{ 0 };
float y_scale{ 0 };

D2D1_RECT_F b1Rect{ 50.0f, 10.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 50.0f, 10.0f, scr_width * 2.0f / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 50.0f, 10.0f, scr_width - 50.0f, 40.0f };

D2D1_RECT_F b1TxtRect{ 80.0f, 15.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 70.0f, 15.0f, scr_width * 2.0f / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 70.0f, 15.0f, scr_width - 50.0f, 40.0f };

bool pause = false;
bool in_client = true;
bool sound = true;
bool show_help = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

bool need_background_left = false;
bool need_background_right = false;

bool name_set = false;
bool level_skipped = false;

bool hero_killed = false;

wchar_t current_player[16]{ L"TARLYO" };

int score{ 0 };
float level{ 1.0f };

float distance{ 600.0f };

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1RadialGradientBrush* SunBrush{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmText{ nullptr };
IDWriteTextFormat* midText{ nullptr };
IDWriteTextFormat* bigText{ nullptr };

ID2D1Bitmap* bmpAcid{ nullptr };
ID2D1Bitmap* bmpArmor{ nullptr };
ID2D1Bitmap* bmpChest{ nullptr };
ID2D1Bitmap* bmpOpenChest{ nullptr };
ID2D1Bitmap* bmpLife{ nullptr };
ID2D1Bitmap* bmpSpit{ nullptr };

ID2D1Bitmap* bmpCastle{ nullptr };
ID2D1Bitmap* bmpCloud1{ nullptr };
ID2D1Bitmap* bmpCloud2{ nullptr };
ID2D1Bitmap* bmpCloud3{ nullptr };
ID2D1Bitmap* bmpGround{ nullptr };
ID2D1Bitmap* bmpPlatform1{ nullptr };
ID2D1Bitmap* bmpPlatform2{ nullptr };
ID2D1Bitmap* bmpPlatform3{ nullptr };
ID2D1Bitmap* bmpRIP{ nullptr };

ID2D1Bitmap* bmpLogo{ nullptr };
ID2D1Bitmap* bmpLevel{ nullptr };
ID2D1Bitmap* bmpWin{ nullptr };
ID2D1Bitmap* bmpLoose{ nullptr };
ID2D1Bitmap* bmpRecord{ nullptr };

ID2D1Bitmap* bmpBackground1[6]{ nullptr };
ID2D1Bitmap* bmpBackground2[3]{ nullptr };
ID2D1Bitmap* bmpIntro[17]{ nullptr };
ID2D1Bitmap* bmpPause[17]{ nullptr };

ID2D1Bitmap* bmpAngry[4]{ nullptr };
ID2D1Bitmap* bmpCaveman[8]{ nullptr };
ID2D1Bitmap* bmpZombie[53]{ nullptr };

ID2D1Bitmap* bmpMinion[16]{ nullptr };

///////////////////////////////////////////////////////////

indll::RANDIT RandIt{};

indll::BAG<indll::FIELD*> bBackgrounds;
indll::BAG<indll::FIELD*> bGrounds;
indll::BAG<indll::FIELD*> bPlatforms;
indll::BAG<indll::FIELD*> bClouds;

D2D1_RECT_F Sun{};

indll::HERO* Hero{ nullptr };


///////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
	check.Release();
};
template<HasRelease T>bool FreeMem(T** var)
{
	if ((*var))
	{
		(*var)->Release();
		(*var) = nullptr;
		return true;
	}

	return false;
}
void LogErr(const wchar_t* what)
{
	std::wofstream err(L".\\res\\data\\error.log", std::ios::app);

	err << what << L" Time stamp: " << std::chrono::system_clock::now() << std::endl;

	err.close();
}
void ClearResources()
{
	if (!FreeMem(&iFactory))LogErr(L"Error releasing main D2D1 Factory !");
	if (!FreeMem(&Draw))LogErr(L"Error releasing main D2D1 HwndRenderTarget !");
	if (!FreeMem(&b1BckgBrush))LogErr(L"Error releasing D2D1 b1BckgBrush !");
	if (!FreeMem(&b2BckgBrush))LogErr(L"Error releasing D2D1 b2BckgBrush !");
	if (!FreeMem(&b3BckgBrush))LogErr(L"Error releasing D2D1 b3BckgBrush !");
	if (!FreeMem(&SunBrush))LogErr(L"Error releasing D2D1 SunBrush !");
	if (!FreeMem(&inactBrush))LogErr(L"Error releasing D2D1 inactBrush !");
	if (!FreeMem(&statBrush))LogErr(L"Error releasing D2D1 statBrush !");
	if (!FreeMem(&txtBrush))LogErr(L"Error releasing D2D1 txtBrush !");
	if (!FreeMem(&hgltBrush))LogErr(L"Error releasing D2D1 hgltBrush !");

	if (!FreeMem(&iWriteFactory))LogErr(L"Error releasing main D2D1 WriteFactory !");
	if (!FreeMem(&nrmText))LogErr(L"Error releasing main D2D1 nrmText format !");
	if (!FreeMem(&midText))LogErr(L"Error releasing main D2D1 midText format !");
	if (!FreeMem(&bigText))LogErr(L"Error releasing main D2D1 bigText format !");

	if (!FreeMem(&bmpAcid))LogErr(L"Error releasing D2D1 bmpAcid image !");
	if (!FreeMem(&bmpArmor))LogErr(L"Error releasing D2D1 bmpArmor image !");
	if (!FreeMem(&bmpChest))LogErr(L"Error releasing D2D1 bmpChest image !");
	if (!FreeMem(&bmpOpenChest))LogErr(L"Error releasing D2D1 bmpOpenChest image !");
	if (!FreeMem(&bmpLife))LogErr(L"Error releasing D2D1 bmpLife image !");
	if (!FreeMem(&bmpSpit))LogErr(L"Error releasing D2D1 bmpSpit image !");

	if (!FreeMem(&bmpCastle))LogErr(L"Error releasing D2D1 bmpCastle image !");
	if (!FreeMem(&bmpGround))LogErr(L"Error releasing D2D1 bmpGround image !");
	if (!FreeMem(&bmpPlatform1))LogErr(L"Error releasing D2D1 bmpPlatform1 image !");
	if (!FreeMem(&bmpPlatform2))LogErr(L"Error releasing D2D1 bmpPlatform2 image !");
	if (!FreeMem(&bmpPlatform3))LogErr(L"Error releasing D2D1 bmpPlatform3 image !");
	if (!FreeMem(&bmpCloud1))LogErr(L"Error releasing D2D1 bmpCloud1 image !");
	if (!FreeMem(&bmpCloud2))LogErr(L"Error releasing D2D1 bmpCloud2 image !");
	if (!FreeMem(&bmpCloud3))LogErr(L"Error releasing D2D1 bmpCloud3 image !");
	if (!FreeMem(&bmpRIP))LogErr(L"Error releasing D2D1 bmpRIP image !");

	if (!FreeMem(&bmpLogo))LogErr(L"Error releasing D2D1 bmpLogo image !");
	if (!FreeMem(&bmpWin))LogErr(L"Error releasing D2D1 bmpWin image !");
	if (!FreeMem(&bmpLoose))LogErr(L"Error releasing D2D1 bmpLoose image !");
	if (!FreeMem(&bmpRecord))LogErr(L"Error releasing D2D1 bmpRecord image !");

	for (int i = 0; i < 6; ++i)if (!FreeMem(&bmpBackground1[i]))LogErr(L"Error releasing D2D1 bmpBackground1 image set!");
	for (int i = 0; i < 3; ++i)if (!FreeMem(&bmpBackground2[i]))LogErr(L"Error releasing D2D1 bmpBackground2 image set!");

	for (int i = 0; i < 17; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing D2D1 bmpIntro image set!");
	for (int i = 0; i < 17; ++i)if (!FreeMem(&bmpPause[i]))LogErr(L"Error releasing D2D1 bmpPause image set!");

	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpAngry[i]))LogErr(L"Error releasing D2D1 bmpAngry image set!");
	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpCaveman[i]))LogErr(L"Error releasing D2D1 bmpCaveman image set!");
	for (int i = 0; i < 53; ++i)if (!FreeMem(&bmpZombie[i]))LogErr(L"Error releasing D2D1 bmpZombie image set!");

	for (int i = 0; i < 16; ++i)if (!FreeMem(&bmpMinion[i]))LogErr(L"Error releasing D2D1 bmpMinion image set!");
}
void ErrExit(int what)
{
	MessageBeep(MB_ICONERROR);

	MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	std::remove(tmp_file);
	ClearResources();
	exit(1);
}

void GameOver()
{
	PlaySound(NULL, NULL, NULL);

	KillTimer(bHwnd, bTimer);





	bMsg.message = WM_QUIT;
	bMsg.wParam = 0;
}
void InitGame()
{
	wcscpy_s(current_player, L"TARLYO");
	name_set = false;
	score = 0 ;
	level = 1.0f ;
	distance = 600.0f;
	level_skipped = false;
	
	Sun.left = 700.0f;
	Sun.right = 820.0f;
	Sun.top = 80.0f;
	Sun.bottom = 170.0f;
	
	//////////////////////////////////////

	need_background_left = false;
	need_background_right = false;

	if(!bBackgrounds.empty())
		for (int i = 0; i < bBackgrounds.size(); ++i)if (!FreeMem(&bBackgrounds[i]))
		{
			LogErr(L"Error releasing bBackgrounds in Init() function !");
			ErrExit(eWindow);
		}
	bBackgrounds.clear();
	
	for (float sx = -scr_width; sx < 2.0f * scr_width; sx += scr_width)
		bBackgrounds.push_back(indll::FIELD::create(static_cast<fields>(RandIt(0, 1)), sx, 0));

	if (!bGrounds.empty())
		for (int i = 0; i < bGrounds.size(); ++i)if (!FreeMem(&bGrounds[i]))
		{
			LogErr(L"Error releasing bGrounds in Init() function !");
			ErrExit(eWindow);
		}
	bGrounds.clear();
	
	for (float sx = -scr_width; sx < 2.0f * scr_width; sx += scr_width)
		bGrounds.push_back(indll::FIELD::create(fields::ground, sx, ground - 42.0f));

	if (!bPlatforms.empty())
		for (int i = 0; i < bPlatforms.size(); ++i)if (!FreeMem(&bPlatforms[i]))
		{
			LogErr(L"Error releasing bPlatforms in Init() function !");
			ErrExit(eWindow);
		}
	bPlatforms.clear();

	if (!bClouds.empty())
		for (int i = 0; i < bClouds.size(); ++i)if (!FreeMem(&bClouds[i]))
		{
			LogErr(L"Error releasing bClouds in Init() function !");
			ErrExit(eWindow);
		}
	bClouds.clear();

	delete Hero;
	Hero = new indll::HERO(30.0f, ground - 88.0f);


}








int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	bIns = hInstance;
	if (!bIns)
	{
		LogErr(L"Error obtaining main Windows hInstance !");
		ErrExit(eClass);
	}











	std::remove(tmp_file);
	ClearResources();

    return (int) bMsg.wParam;
}