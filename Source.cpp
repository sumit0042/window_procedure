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

WPARAM wparam = 0;
WPARAM mparam = 0;
WPARAM pparam = 0;
COLORREF _color = 0x000000;

LRESULT CALLBACK w(HWND h, UINT i, WPARAM wp, LPARAM lp)
{
	RECT rect;
	rect.left = 100;
	rect.top = 500;
	rect.right = 500;
	rect.bottom = 100;
	switch (i)
	{

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		HDC hdc, hdcsrc;
		HBITMAP bmp;

		hdc = BeginPaint(h, &ps);
		if (mparam == 10) //load
		{
			LPCWSTR file = L"ellipse.bmp";
			hdcsrc = CreateCompatibleDC(NULL);
			bmp = (HBITMAP)LoadImageW(NULL, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			SelectObject(hdcsrc, bmp);
			BitBlt(hdc, 90, 90, 510, 510, hdcsrc, 0, 0, SRCCOPY);
			DeleteObject(bmp);
			DeleteDC(hdcsrc);
			EndPaint(h, &ps);
			mparam = 0;
			return 0;
		}

		if (mparam == 100)//clear
		{
			EndPaint(h, &ps);
			mparam = 0;
			return 0;
		}

		HPEN hpen = CreatePen(PS_DASHDOTDOT, 20, _color); //BGR
		

			SelectObject(hdc, hpen);
			switch (pparam)
			{
			case 300:
			{
				Ellipse(hdc, 100, 100, 500, 500);
				break;
			}
			case 301:
			{
				Ellipse(hdc, 100, 100, 500, 400);
				break;
			}
			case 302:
			{
				DrawFocusRect(hdc, new RECT{100,100,500,500});
				break;
			}
			case 303:
			{
				DrawFocusRect(hdc, new RECT{ 100,100,500,400});
				break;
			}
			default:
				break;
			}

		if (mparam == 500)
		{

			std::ifstream ifn;
			cout << "Saving.. " << endl;
			HDCToFile("ellipse.bmp", hdc, { 90,90,510,510 });
			cout << "Saved" << endl;
			mparam = 0;
		}
		EndPaint(h, &ps);
		return 0;

	}

	case WM_LBUTTONUP:
	{

	}
	case WM_LBUTTONDOWN:
	{

		int xPos = GET_X_LPARAM(lp);
		int yPos = GET_Y_LPARAM(lp);
		cout << "From WM_LBUTTONDOWN, mouse pressed at : ";
		cout << xPos << " " << yPos << endl;
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
		case 302:
		{
			pparam = wp;
			//sq
			UpdateWindow(h);
			RedrawWindow(h, NULL, NULL, RDW_INVALIDATE);
			break;
		}
		case 303:
		{
			pparam = wp;
			//rec
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
		cout << "From WM_COMMAND" << h << " " << i << " " << wp << " " << lp << endl;
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
	HWND h_col_sq = CreateWindowExW(0L, L"button", L"Square ", WS_VISIBLE | WS_CHILDWINDOW, 800, 320, 50, 50, h, (HMENU)302, NULL, NULL);
	HWND h_col_rc = CreateWindowExW(0L, L"button", L"Rect   ", WS_VISIBLE | WS_CHILDWINDOW, 800, 380, 50, 50, h, (HMENU)303, NULL, NULL);
	//cout << "h=" << h << endl;


	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		std::cout << "From while msg " << msg.message << std::endl;
		DispatchMessage(&msg);
	}

	return 0;
}
