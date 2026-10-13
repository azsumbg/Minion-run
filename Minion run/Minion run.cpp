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
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
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

dirs nature_dir = dirs::stop;

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
ID2D1Bitmap* bmpIntro[18]{ nullptr };
ID2D1Bitmap* bmpPause[17]{ nullptr };

ID2D1Bitmap* bmpAngry[4]{ nullptr };
ID2D1Bitmap* bmpCaveman[8]{ nullptr };
ID2D1Bitmap* bmpZombie[53]{ nullptr };

ID2D1Bitmap* bmpMinion[16]{ nullptr };

///////////////////////////////////////////////////////////

indll::RANDIT RandIt{};

indll::FIELD* Intro{ nullptr };
indll::FIELD* Pause{ nullptr };

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

	for (int i = 0; i < 18; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing D2D1 bmpIntro image set!");
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
	
	Intro = indll::FIELD::create(fields::intro, 0, 0);
	Pause = indll::FIELD::create(fields::pause, 0, 0);

	Sun.left = 700.0f;
	Sun.right = 820.0f;
	Sun.top = 80.0f;
	Sun.bottom = 170.0f;
	
	//////////////////////////////////////

	nature_dir = dirs::stop;

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

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(bIcon));
		return true;

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDOK:
			if (GetDlgItemTextW(hwnd, IDC_NAME, current_player, 16) < 1)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				wcscpy_s(current_player, L"TARLYO");

				MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
				EndDialog(hwnd, IDCANCEL);
				break;
			}
			EndDialog(hwnd, IDOK);
			break;
		}
		break;
	}

	return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_CREATE:
		if (bIns)
		{
			SetTimer(hwnd, bTimer, 500, NULL);

			bBar = CreateMenu();
			bMain = CreateMenu();
			bStore = CreateMenu();

			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

			AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
			AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
			AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bMain, MF_STRING, mExit, L"Изход");

			AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
			AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
			AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

			SetMenu(hwnd, bBar);
			InitGame();
		}
		break;

	case WM_CLOSE:
		pause = true;
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		if (MessageBox(hwnd, L"Ако излезеш, губиш настоящата игра !\n\nНаистина ли излизаш ?", L"Изход",
			MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
		{
			pause = false;
			break;
		}
		GameOver();
		break;

	case WM_SETCURSOR:
		GetCursorPos(&cur_pos);
		ScreenToClient(hwnd, &cur_pos);
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (!in_client)
			{
				in_client = true;
				pause = false;
			}

			if (cur_pos.y * y_scale <= 50)
			{
				if (cur_pos.x * x_scale >= b1Rect.left && cur_pos.x * x_scale <= b1Rect.right)
				{
					if (!b1Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = true;
						b2Hglt = false;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b2Rect.left && cur_pos.x * x_scale <= b2Rect.right)
				{
					if (!b2Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = true;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b3Rect.left && cur_pos.x * x_scale <= b3Rect.right)
				{
					if (!b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = true;
					}
				}
				else
				{
					if (b1Hglt || b2Hglt || b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = false;
					}
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}
			
			SetCursor(bCursor);

			return true;
		}
		else
		{
			if (in_client)
			{
				in_client = false;
				pause = true;
			}

			if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
		}
		break;

	case WM_TIMER:
		if (pause)break;
		--distance;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case mNew:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако рестартираш, губиш настоящата игра !\n\nНаистина ли рестартираш ?", L"Рестарт",
				MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			InitGame();
			break;

		case mLvl:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако прескочиш нивото, губиш бонусите за него !\n\nНаистина ли прескачаш ниво ?", 
				L"Следващо ниво", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			//LevelUp();
			break;

		case mExit:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;




		}
		break;





	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return(LRESULT)(FALSE);
}

void CreateResources()
{
	int result{ 0 };
	CheckFile(Ltmp_file, &result);

	if (result == FILE_EXIST)ErrExit(eStarted);
	else
	{
		std::wofstream start{ Ltmp_file };
		start << L"Game started at : " << std::chrono::system_clock::now();
		start.close();
	}
	
	int win_x{ (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f)) };
	int win_y = 10;

	if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) ||
		GetSystemMetrics(SM_CYSCREEN) < win_y + (int)(scr_height))ErrExit(eScreen);

	bIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
	if (!bIcon)ErrExit(eIcon);

	bCursor = LoadCursorFromFileW(L".\\res\\main.ani");
	outCursor = LoadCursorFromFileW(L".\\res\\out.ani");
	if (!bCursor || !outCursor)ErrExit(eCursor);

	bWinClass.lpszClassName = bWinClassName;
	bWinClass.hInstance = bIns;
	bWinClass.lpfnWndProc = &WinProc;
	bWinClass.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	bWinClass.hIcon = bIcon;
	bWinClass.hCursor = bCursor;
	bWinClass.style = CS_DROPSHADOW;

	if (!RegisterClass(&bWinClass))ErrExit(eClass);

	bHwnd = CreateWindow(bWinClassName, L"БЯГАЩИ МИНЬОНИ", WS_CAPTION | WS_SYSMENU, win_x, win_y, (int)(scr_width),
		(int)(scr_height), NULL, NULL, bIns, NULL);

	if (!bHwnd)ErrExit(eWindow);
	else
	{
		ShowWindow(bHwnd, SW_SHOWDEFAULT);

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
		if (hr != S_OK)
		{
			LogErr(L"Error creating main D2D1 Factory !");
			ErrExit(eD2D);
		}

		if (iFactory)
			hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
				D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
		if (hr != S_OK)
		{
			LogErr(L"Error creating main D2D1 HwndRenderTarget !");
			ErrExit(eD2D);
		}

		if (Draw)
		{
			RECT ScrRect{};
			D2D1_SIZE_F DIPRect{ Draw->GetSize() };

			GetClientRect(bHwnd, &ScrRect);

			x_scale = DIPRect.width / (ScrRect.right - ScrRect.left);
			y_scale = DIPRect.height / (ScrRect.bottom - ScrRect.top);

			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Navy), &statBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Chartreuse), &txtBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &hgltBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SteelBlue), &inactBrush);

			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 SolidColorBrushes !");
				ErrExit(eD2D);
			}

			D2D1_GRADIENT_STOP gStops[2]{};
			gStops[0].position = 0;
			gStops[0].color = D2D1::ColorF(D2D1::ColorF::SandyBrown);
			gStops[1].position = 1.0;
			gStops[1].color = D2D1::ColorF(D2D1::ColorF::SaddleBrown);

			ID2D1GradientStopCollection* gColl{ nullptr };
			hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 GradientStopCollection for RadialGradientBrushes !");
				ErrExit(eD2D);
			}

			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left
					+ (b1Rect.right - b1Rect.left) / 2.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2.0f, 20.0f), 
					gColl, &b1BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left
					+ (b2Rect.right - b2Rect.left) / 2.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2.0f, 20.0f),
					gColl, &b2BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left
					+ (b3Rect.right - b3Rect.left) / 2.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2.0f, 20.0f),
					gColl, &b3BckgBrush);

				if (hr != S_OK)
				{
					LogErr(L"Error creating main D2D1 RadialGradientBrushes !");
					ErrExit(eD2D);
				}

				if (!FreeMem(&gColl))
				{
					LogErr(L"Error releasing GradientStopCollection for button background brushes !");
					ErrExit(eD2D);
				}
			}

			D2D1_GRADIENT_STOP gSunStops[5]{};
			gSunStops[0].position = 0;
			gSunStops[0].color = D2D1::ColorF(D2D1::ColorF::LightYellow);
			gSunStops[1].position = 0.25f;
			gSunStops[1].color = D2D1::ColorF(D2D1::ColorF::Yellow);
			gSunStops[2].position = 0.50f;
			gSunStops[2].color = D2D1::ColorF(D2D1::ColorF::Gold);
			gSunStops[3].position = 0.75f;
			gSunStops[3].color = D2D1::ColorF(D2D1::ColorF::Goldenrod);
			gSunStops[4].position = 1.0f;
			gSunStops[4].color = D2D1::ColorF(D2D1::ColorF::Orange);

			hr = Draw->CreateGradientStopCollection(gSunStops, 5, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 GradientStopCollection for SunGradientBrush !");
				ErrExit(eD2D);
			}

			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(Sun.left
					+ (Sun.right - Sun.left) / 2.0f, Sun.top + (Sun.bottom - Sun.top) / 2.0f),
					D2D1::Point2F(0, 0), (Sun.right - Sun.left) / 2.0f, (Sun.bottom - Sun.top) / 2.0f),
					gColl, &SunBrush);
				
				if (hr != S_OK)
				{
					LogErr(L"Error creating main D2D1 RadialGradientBrushes !");
					ErrExit(eD2D);
				}

				if (!FreeMem(&gColl))
				{
					LogErr(L"Error releasing GradientStopCollection for Sun brush !");
					ErrExit(eD2D);
				}
			}

			bmpAcid = Load(L".\\res\\img\\assets\\acid.png", Draw);
			if (!bmpAcid)
			{
				LogErr(L"Error loading bmpAcid image !");
				ErrExit(eD2D);
			}
			bmpArmor = Load(L".\\res\\img\\assets\\armor.png", Draw);
			if (!bmpArmor)
			{
				LogErr(L"Error loading bmpArmor image !");
				ErrExit(eD2D);
			}
			bmpChest = Load(L".\\res\\img\\assets\\chest.png", Draw);
			if (!bmpChest)
			{
				LogErr(L"Error loading bmpChest image !");
				ErrExit(eD2D);
			}
			bmpOpenChest = Load(L".\\res\\img\\assets\\chest_open.png", Draw);
			if (!bmpOpenChest)
			{
				LogErr(L"Error loading bmpOpenChest image !");
				ErrExit(eD2D);
			}
			bmpLife = Load(L".\\res\\img\\assets\\life.png", Draw);
			if (!bmpLife)
			{
				LogErr(L"Error loading bmpLife image !");
				ErrExit(eD2D);
			}
			bmpSpit = Load(L".\\res\\img\\spit.png", Draw);
			if (!bmpSpit)
			{
				LogErr(L"Error loading bmpSpit image !");
				ErrExit(eD2D);
			}

			bmpCastle = Load(L".\\res\\img\\field\\castle.png", Draw);
			if (!bmpCastle)
			{
				LogErr(L"Error loading bmpCastle image !");
				ErrExit(eD2D);
			}
			bmpCloud1 = Load(L".\\res\\img\\field\\Cloud1.png", Draw);
			if (!bmpCloud1)
			{
				LogErr(L"Error loading bmpCloud1 image !");
				ErrExit(eD2D);
			}
			bmpCloud2 = Load(L".\\res\\img\\field\\Cloud2.png", Draw);
			if (!bmpCloud2)
			{
				LogErr(L"Error loading bmpCloud2 image !");
				ErrExit(eD2D);
			}
			bmpCloud3 = Load(L".\\res\\img\\field\\Cloud3.png", Draw);
			if (!bmpCloud3)
			{
				LogErr(L"Error loading bmpCloud3 image !");
				ErrExit(eD2D);
			}
			bmpGround = Load(L".\\res\\img\\field\\Ground.png", Draw);
			if (!bmpGround)
			{
				LogErr(L"Error loading bmpGround image !");
				ErrExit(eD2D);
			}
			bmpPlatform1 = Load(L".\\res\\img\\field\\Platform1.png", Draw);
			if (!bmpPlatform1)
			{
				LogErr(L"Error loading bmpPlatform1 image !");
				ErrExit(eD2D);
			}
			bmpPlatform2 = Load(L".\\res\\img\\field\\Platform2.png", Draw);
			if (!bmpPlatform2)
			{
				LogErr(L"Error loading bmpPlatform2 image !");
				ErrExit(eD2D);
			}
			bmpPlatform3 = Load(L".\\res\\img\\field\\Platform3.png", Draw);
			if (!bmpPlatform3)
			{
				LogErr(L"Error loading bmpPlatform3 image !");
				ErrExit(eD2D);
			}
			bmpRIP = Load(L".\\res\\img\\field\\RIP.png", Draw);
			if (!bmpRIP)
			{
				LogErr(L"Error loading bmpRIP image !");
				ErrExit(eD2D);
			}

			bmpLogo = Load(L".\\res\\img\\logos\\logo.png", Draw);
			if (!bmpLogo)
			{
				LogErr(L"Error loading bmpLogo image !");
				ErrExit(eD2D);
			}
			bmpWin = Load(L".\\res\\img\\logos\\win.png", Draw);
			if (!bmpWin)
			{
				LogErr(L"Error loading bmpWin image !");
				ErrExit(eD2D);
			}
			bmpLoose = Load(L".\\res\\img\\logos\\loose.png", Draw);
			if (!bmpLoose)
			{
				LogErr(L"Error loading bmpLoose image !");
				ErrExit(eD2D);
			}
			bmpLevel = Load(L".\\res\\img\\logos\\level.png", Draw);
			if (!bmpLevel)
			{
				LogErr(L"Error loading bmpLevel image !");
				ErrExit(eD2D);
			}
			bmpRecord = Load(L".\\res\\img\\logos\\record.png", Draw);
			if (!bmpRecord)
			{
				LogErr(L"Error loading bmpRecord image !");
				ErrExit(eD2D);
			}

			for (int i = 0; i < 6; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\background1\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpBackground1[i] = Load(name, Draw);
				if (!bmpBackground1[i])
				{
					LogErr(L"Error loading bmpBackground1 image set !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 3; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\background2\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpBackground2[i] = Load(name, Draw);
				if (!bmpBackground2[i])
				{
					LogErr(L"Error loading bmpBackground2 image set !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 18; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\intro\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpIntro[i] = Load(name, Draw);
				if (!bmpIntro[i])
				{
					LogErr(L"Error loading bmpIntro image set !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 17; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\pause\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpPause[i] = Load(name, Draw);
				if (!bmpPause[i])
				{
					LogErr(L"Error loading bmpPause image set !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\angry\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpAngry[i] = Load(name, Draw);
				if (!bmpAngry[i])
				{
					LogErr(L"Error loading bmpAngry image set !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\caveman\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpCaveman[i] = Load(name, Draw);
				if (!bmpCaveman[i])
				{
					LogErr(L"Error loading bmpCaveman image set !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 53; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\zombie\\0" };
				wchar_t add[5]{ L"\0" };

				if (i < 10)wcscat_s(name, L"0");

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpZombie[i] = Load(name, Draw);
				if (!bmpZombie[i])
				{
					LogErr(L"Error loading bmpZombie image set !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 16; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\minion\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpMinion[i] = Load(name, Draw);
				if (!bmpMinion[i])
				{
					LogErr(L"Error loading bmpMinion image set !");
					ErrExit(eD2D);
				}
			}
		}

		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>
			(&iWriteFactory));
		if (hr != S_OK)
		{
			LogErr(L"Error creating main D2D1 WriteFactory !");
			ErrExit(eD2D);
		}
		
		if (iWriteFactory)
		{
			hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &nrmText);
			hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"", &midText);
			hr = iWriteFactory->CreateTextFormat(L"GNABRI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigText);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 WriteFactory text formats !");
				ErrExit(eD2D);
			}
		}


	}
	
	PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_ASYNC);

	for (int i = 0; i < 260; ++i)
	{
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpIntro[Intro->get_frame()], Intro->rect);
		Draw->DrawBitmap(bmpLogo, Intro->rect);
		Draw->EndDraw();
	}

	PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	bIns = hInstance;
	if (!bIns)
	{
		LogErr(L"Error obtaining main Windows hInstance !");
		ErrExit(eClass);
	}

	CreateResources();



	while (bMsg.message != WM_QUIT)
	{
		if ((bRet = PeekMessage(&bMsg, NULL, NULL, NULL, PM_REMOVE)) != 0)
		{
			if (bRet == -1)ErrExit(eMsg);

			TranslateMessage(&bMsg);
			DispatchMessage(&bMsg);
		}

		if (pause)
		{
			if (show_help)continue;

			if (bigText && txtBrush)
			{
				Draw->BeginDraw();
				Draw->DrawBitmap(bmpPause[Pause->get_frame()], Pause->rect);
				Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(scr_width / 2.0f - 100.0f, scr_height / 2.0f - 50.0f,
					scr_width, scr_height), txtBrush);
				Draw->EndDraw();
				continue;
			}
		}

	/////////////////////////////////////////////////////////////

















	// DRAW THINGS **********************************************

		Draw->BeginDraw();

		if (statBrush && inactBrush && txtBrush && hgltBrush && b1BckgBrush && b2BckgBrush && b3BckgBrush && nrmText)
		{
			if (!bBackgrounds.empty())
			{
				for (int i = 0; i < bBackgrounds.size(); ++i)
				{
					switch (bBackgrounds[i]->get_type())
					{
					case fields::background1:
						Draw->DrawBitmap(bmpBackground1[bBackgrounds[i]->get_frame()], bBackgrounds[i]->rect);
						break;
				
					case fields::background2:
						Draw->DrawBitmap(bmpBackground2[bBackgrounds[i]->get_frame()], bBackgrounds[i]->rect);
						break;
					}
					
				}
			}
			
			Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), statBrush);
			Draw->FillRectangle(D2D1::RectF(0, ground, scr_width, scr_height), statBrush);

			if (!bGrounds.empty())
				for (int i = 0; i < bGrounds.size(); ++i)Draw->DrawBitmap(bmpGround, bGrounds[i]->rect);
			
			Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(Sun.left + (Sun.right - Sun.left) / 2.0f,
				Sun.top + (Sun.bottom - Sun.top) / 2.0f), (Sun.right - Sun.left) / 2.0f,
				(Sun.bottom - Sun.top) / 2.0f), SunBrush);
			
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, 10.0f, 15.0f), b1BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, 10.0f, 15.0f), b2BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, 10.0f, 15.0f), b3BckgBrush);

			if (name_set)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, inactBrush);
			else
			{
				if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, txtBrush);
				else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, hgltBrush);
			}
			if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, txtBrush);
			else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, hgltBrush);
			if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, txtBrush);
			else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, hgltBrush);
		}


	//////////////////////////////////////////////////////////////

		Draw->EndDraw();

	}

	std::remove(tmp_file);
	ClearResources();

    return (int) bMsg.wParam;
}