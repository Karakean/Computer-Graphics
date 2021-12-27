#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
using namespace Gdiplus;

POINT car[13];
int chosen;
bool isChosen = false;

Point curve_points[20];
int counter = 0;
bool isClosed = false;
Point last_click;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void createLetter(POINT* first, POINT* second)
{
	first[0] = { 50,50 };
	first[1] = { 60,50 };
	first[2] = { 60,10 };
	first[3] = { 90,40 };
	first[4] = { 120,10 };
	first[5] = { 120,50 };
	first[6] = { 130,50 };
	first[7] = { 130,0 };
	first[8] = { 120,0 };
	first[9] = { 90,30 };
	first[10] = { 60,0 };
	first[11] = { 50,0 };

	second[0] = { 120,50 };
	second[1] = { 130,50 };
	second[2] = { 130,10 };
	second[3] = { 120,10 };
}

void setSize(POINT* points, int points_num, float dx, float dy, float scaleX, float scaleY)
{
	for (int i = 0; i < points_num; i++)
	{
		points[i].x *= scaleX;
		points[i].y *= scaleY;
		points[i].x += dx;
		points[i].y += dy;
	}
}

void createCar(POINT* car) {
	car[0] = { 40,60 };
	car[1] = { 90,60 };
	car[2] = { 100,60 };
	car[3] = { 150,60 };
	car[4] = { 160,55 };
	car[5] = { 155,30 };
	car[6] = { 100, 15 };
	car[7] = { 90, 15 };
	car[8] = { 80, 20 };
	car[9] = { 70, 30 };
	car[10] = { 5,30 };
	car[11] = { 0,50 };
	car[12] = { 40,60 };
}

void movePoint(POINT point, float x, float y) {
	point.x = x;
	point.y = y;
}

 int WINAPI WinMain(HINSTANCE hInstance,
               HINSTANCE hPrevInstance,
               LPSTR     lpCmdLine,
               int       nCmdShow)
{
	MSG message;
	WNDCLASS nasza_klasa;
	HWND okno;
	static char nazwa_klasy[] = "Podstawowa";
	
	GdiplusStartupInput gdiplusParametry;
	ULONG_PTR	gdiplusToken;
   
	GdiplusStartup(&gdiplusToken, &gdiplusParametry, NULL);

	nasza_klasa.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	nasza_klasa.lpfnWndProc   = WndProc;
 	nasza_klasa.cbClsExtra    = 0 ;
	nasza_klasa.cbWndExtra    = 0 ;
	nasza_klasa.hInstance     = hInstance;
	nasza_klasa.hIcon         = 0;
	nasza_klasa.hCursor       = LoadCursor(0, IDC_ARROW);
	nasza_klasa.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
	nasza_klasa.lpszMenuName  = "Menu" ;
	nasza_klasa.lpszClassName = nazwa_klasy;

    RegisterClass (&nasza_klasa);
	
 	okno = CreateWindow(nazwa_klasy, "Grafika komputerowa", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	
   	ShowWindow (okno, nCmdShow) ;
    
	UpdateWindow (okno) ;

	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	GdiplusShutdown(gdiplusToken);
	
	return (int)message.wParam;
}

LRESULT CALLBACK WndProc (HWND okno, UINT kod_meldunku, WPARAM wParam, LPARAM lParam)
{
	HMENU mPlik, mInfo, mGlowne;
    	
	switch (kod_meldunku) 
	{
	case WM_CREATE:
		mPlik = CreateMenu();
		AppendMenu(mPlik, MF_STRING, 100, "&Zapiszcz...");
		AppendMenu(mPlik, MF_SEPARATOR, 0, "");
		AppendMenu(mPlik, MF_STRING, 101, "&Koniec");
		mInfo = CreateMenu();
		AppendMenu(mInfo, MF_STRING, 200, "&Autor...");
		mGlowne = CreateMenu();
		AppendMenu(mGlowne, MF_POPUP, (UINT_PTR) mPlik, "&Plik");
		AppendMenu(mGlowne, MF_POPUP, (UINT_PTR) mInfo, "&Informacja");
		SetMenu(okno, mGlowne);
		DrawMenuBar(okno);
		createCar(car);
		setSize(car, 13, 600, 400, 2, 2);

	case WM_COMMAND:
		switch (wParam)
		{
		case 100: if(MessageBox(okno, "Zapiszczeć?", "Pisk", MB_YESNO) == IDYES)
					MessageBeep(0);
                  break;
		case 101: DestroyWindow(okno);
        		  break;
		case 200: MessageBox(okno, "Imię i nazwisko:\nNumer indeksu: ", "Autor", MB_OK);
		}
		return 0;
	
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (!isChosen) {
				for (int i = 0; i < 13; i++) {
					if ((x - 5 <= car[i].x) && (x + 5 >= car[i].x) && (y - 5 <= car[i].y) && (y + 5 >= car[i].y)) {
						chosen = i;
						isChosen = true;
					}
				}
			}
			else {
				isChosen = false;
			}

			return 0;
		}

	case WM_RBUTTONDOWN:
		{	
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (counter < 20) {
				curve_points[counter] = { x, y };
				counter++;
			}
			InvalidateRect(okno, NULL, true);
			return 0;
		}
	
	case WM_RBUTTONDBLCLK:
		{
			isClosed = true;
			InvalidateRect(okno, NULL, true);
			return 0;
		}

	case WM_RBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if ((!isClosed) && (counter < 20)) {
				curve_points[counter] = { x,y };
				counter++;
			}
			InvalidateRect(okno, NULL, true);
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (isChosen) {
				if (chosen == 0 || chosen == 12) {
					car[0].x = car[12].x = x;
					car[0].y = car[12].y = y;
				}
				else {
					car[chosen].x = x;
					car[chosen].y = y;
				}
				InvalidateRect(okno, NULL, true);
			}

		}
	case WM_PAINT:
		{
			HDC kontekst;
			PAINTSTRUCT paint;
			
			kontekst = BeginPaint(okno, &paint);
		
			HBRUSH pedzle[6] = { CreateSolidBrush(RGB(0, 0, 255)), CreateSolidBrush(RGB(0, 255, 255)),
				CreateSolidBrush(RGB(255, 0, 255)), CreateSolidBrush(RGB(255, 0, 0)),
				CreateSolidBrush(RGB(255, 100, 150)), CreateSolidBrush(RGB(125, 125, 150)) };


			SelectObject(kontekst, pedzle[5]);
			Pie(kontekst, 200, 200, 400, 400, 400, 300, 300, 200);
			DeleteObject(pedzle[5]);
			SelectObject(kontekst, pedzle[4]);
			Pie(kontekst, 200, 200, 400, 400, 300, 200, 200, 300);
			DeleteObject(pedzle[4]);
			SelectObject(kontekst, pedzle[3]);
			Pie(kontekst, 200, 200, 400, 400, 200, 300, 300, 400);
			DeleteObject(pedzle[3]);
			SelectObject(kontekst, pedzle[2]);
			Pie(kontekst, 200, 200, 400, 400, 300, 400, 400, 300);
			DeleteObject(pedzle[2]);

			POINT first[12], second[4];
			createLetter(first, second);
			setSize(first, 12, 164, 250, 1.5, 1.5);
			setSize(second, 4, 164, 250, 1.5, 1.5);
			SelectObject(kontekst, pedzle[1]);
			Polygon(kontekst, first, 12);
			DeleteObject(pedzle[1]);
			SelectObject(kontekst, pedzle[0]);
			Polygon(kontekst, second, 4);

			PolyBezier(kontekst, car, 13);
			for (int i = 0; i < 13; i++) {
				Ellipse(kontekst, car[i].x - 3, car[i].y - 3, car[i].x + 3, car[i].y + 3);
			}
			Ellipse(kontekst, 685, 500, 735, 550);
			Ellipse(kontekst, 810, 500, 860, 550);

			Graphics grafika(kontekst);
			Pen pen(Color::Blue, 3);

			for (int i = 0; i < counter; i++) {
				Ellipse(kontekst, curve_points[i].X - 3, curve_points[i].Y - 3, curve_points[i].X + 3, curve_points[i].Y + 3);
			}
			if (!isClosed) {
				grafika.DrawCurve(&pen, curve_points, counter);
			}
			else {
				grafika.DrawClosedCurve(&pen, curve_points, counter);
			}
			DeleteObject(pedzle[0]);
			
			FontFamily  fontFamily(L"Times New Roman");
			Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
			PointF      pointF(100.0f, 400.0f);
			SolidBrush  solidBrush(Color(255, 0, 0, 255));

			grafika.DrawString(L"To jest tekst napisany za pomocą GDI+.", -1, &font, pointF, &solidBrush);
			PointF pointXD(100.0f, 500.0f);

			EndPaint(okno, &paint);

			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0;
    
	default:
		return DefWindowProc(okno, kod_meldunku, wParam, lParam);
	}
}
