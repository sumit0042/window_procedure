// ui_program.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<iostream>
#include<Windows.h>
#include<windowsx.h >
#include<fstream>

using namespace std;

bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
	uint32_t Width = Area.right - Area.left;
	uint32_t Height = Area.bottom - Area.top;

	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = BitsPerPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	char* Pixels = NULL;
	HDC MemDC = CreateCompatibleDC(Context);
	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)& Pixels, 0, 0);
	DeleteObject(SelectObject(MemDC, Section));
	BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
	DeleteDC(MemDC);

	std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
	if (hFile.is_open())
	{
		hFile.write((char*)& Header, sizeof(Header));
		hFile.write((char*)& Info.bmiHeader, sizeof(Info.bmiHeader));
		hFile.write(Pixels, ((((int)BitsPerPixel * Width + 31) & ~31) / 8) * Height);
		hFile.close();
		DeleteObject(Section);
		return true;
	}

	DeleteObject(Section);
	return false;
}

/*
0 : waiting for left click
1 : left clicked, 1st pt
2 : left clicked. 2nd pt
3 : left clicked, 3rd pt - DRAW
*/
int drawstate = 0;
int xx1 = 0, yy1 = 0, xx2 = 0, yy2 = 0, xx3 = 0, yy3 = 0;

WPARAM wparam = 0;
WPARAM mparam = 0;
WPARAM pparam = 0;
COLORREF _color = 0x000000;

void loadBMP(HDC hdc, HDC hdcsrc, HBITMAP bmp, LPCWSTR file)
{
	SelectObject(hdcsrc, bmp);
	BitBlt(hdc, 90, 90, 510, 510, hdcsrc, 0, 0, SRCCOPY);
}

void loadHRC(HDC hdc, LPCWSTR file)
{
	HDC hdcsrc;
	HBITMAP bmp;
	hdcsrc = CreateCompatibleDC(NULL);
	bmp = (HBITMAP)LoadImageW(NULL, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	loadBMP(hdc, hdcsrc, bmp, file);
	DeleteObject(bmp);
	DeleteDC(hdcsrc);
}

int paintwind(HWND h)
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(h, &ps);

	if (mparam == 10) //load
	{
		loadHRC(hdc, L"ellipse.bmp");
		HDCToFile("ellipse_tmp.bmp", hdc, { 90,90,510,510 });
		EndPaint(h, &ps);
		mparam = 0;
		return 0;
	}

	if (mparam == 100)//clear
	{
		EndPaint(h, &ps);
		pparam = 0;
		mparam = 0;
		return 0;
	}

	HPEN hpen = CreatePen(PS_DASHDOTDOT, 20, _color); //BGR
	SelectObject(hdc, hpen);
	switch (pparam)
	{
	case 300:
	{
		loadHRC(hdc, L"ellipse_tmp.bmp");
		Ellipse(hdc, 100, 100, 500, 500);
		HDCToFile("ellipse_tmp.bmp", hdc, { 90,90,510,510 });
		break;
	}
	case 301:
	{
		loadHRC(hdc, L"ellipse_tmp.bmp");
		Ellipse(hdc, 100, 100, 500, 400);
		HDCToFile("ellipse_tmp.bmp", hdc, { 90,90,510,510 });
		break;
	}
	default:
		break;
	}

	if (mparam == 500)
	{
		cout << "Saving.. " << endl;
		HDCToFile("ellipse.bmp", hdc, { 90,90,510,510 });
		cout << "Saved" << endl;
		mparam = 0;
	}
	EndPaint(h, &ps);
	return 0;

}

LRESULT CALLBACK w(HWND h, UINT i, WPARAM wp, LPARAM lp)
{
	switch (i)
	{

	case WM_PAINT:
	{
		paintwind(h);
	}

	case WM_LBUTTONUP:
	{
		int xPos = GET_X_LPARAM(lp);
		int yPos = GET_Y_LPARAM(lp);
		cout << "released at : ";
		cout << xPos << " " << yPos << endl;
	}
	case WM_LBUTTONDOWN:
	{
		//break;
	}

	case WM_COMMAND:
	{
		switch (wp)
		{
		case 300:
		{
			//circ
			pparam = wp;
			InvalidateRect(h, NULL, true);
			break;
		}
		case 301:
		{
			pparam = wp;
			//elli
			InvalidateRect(h, NULL, true);
			break;
		}
		case 200:
		{
			_color = 0x000000;
			break;
		}
		case 201:
		{
			_color = 0x0000FF;
			break;
		}
		case 202:
		{
			_color = 0x00FF00;
			//UpdateWindow(h);
			//RedrawWindow(h, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case 203:
		{
			_color = 0xFF0000;
			break;
		}
		case 500://save
		{
			mparam = 500;
			InvalidateRect(h, NULL, true);
			break;
		}
		case 100://clear
		{
			mparam = 100;
			InvalidateRect(h, NULL, true);
			break;
		}
		case 10://load
		{
			mparam = 10;
			InvalidateRect(h, NULL, true);
			break;
		}
		default:
			break;
		}
		//cout << "From WM_COMMAND" << h << " " << i << " " << wp << " " << lp << endl;
	}

	break;
	default:
		return DefWindowProc(h, i, wp, lp);
	}
	return 0;
}

int main()
{
	MSG msg;

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = w;
	wcex.lpszClassName = L"ABC";
	wcex.hbrBackground = GetSysColorBrush(COLOR_3DSHADOW);
	RegisterClassExW(&wcex);

	//RegisterClassExW(&wcex);

	HWND h = CreateWindowExW(0L, L"ABC", L"window", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0, 1000, 1000, NULL, NULL, NULL, NULL);
	//SetParent((HWND)0x00B707A2, h);
	HWND h_save = CreateWindowExW(0L, L"button", L"Save ", WS_VISIBLE | WS_CHILDWINDOW, 900, 600, 50, 50, h, (HMENU)500, NULL, NULL);
	HWND h_load = CreateWindowExW(0L, L"button", L"Load ", WS_VISIBLE | WS_CHILDWINDOW, 700, 600, 50, 50, h, (HMENU)10, NULL, NULL);
	HWND h_clear = CreateWindowExW(0L, L"button", L"Clear", WS_VISIBLE | WS_CHILDWINDOW, 800, 600, 50, 50, h, (HMENU)100, NULL, NULL);

	HWND h_col_B = CreateWindowExW(0L, L"button", L"Black", WS_VISIBLE | WS_CHILDWINDOW, 900, 200, 50, 50, h, (HMENU)200, NULL, NULL);
	HWND h_col_r = CreateWindowExW(0L, L"button", L"Red  ", WS_VISIBLE | WS_CHILDWINDOW, 900, 260, 50, 50, h, (HMENU)201, NULL, NULL);
	HWND h_col_g = CreateWindowExW(0L, L"button", L"Green", WS_VISIBLE | WS_CHILDWINDOW, 900, 320, 50, 50, h, (HMENU)202, NULL, NULL);
	HWND h_col_b = CreateWindowExW(0L, L"button", L"Blue ", WS_VISIBLE | WS_CHILDWINDOW, 900, 380, 50, 50, h, (HMENU)203, NULL, NULL);

	HWND h_col_cl = CreateWindowExW(0L, L"button", L"Circle ", WS_VISIBLE | WS_CHILDWINDOW, 800, 200, 50, 50, h, (HMENU)300, NULL, NULL);
	HWND h_col_el = CreateWindowExW(0L, L"button", L"Ellipse", WS_VISIBLE | WS_CHILDWINDOW, 800, 260, 50, 50, h, (HMENU)301, NULL, NULL);
	//cout << "h=" << h << endl;


	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		//std::cout << "From while msg " << msg.message << std::endl;
		DispatchMessage(&msg);
	}

	return 0;
}
