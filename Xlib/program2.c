#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int red, green, blue, yellow, black, white;
unsigned long foreground, background;


//*************************************************************************************************************************
//funkcja przydzielania kolorow

int AllocNamedColor(char *name, Display* display, Colormap colormap)
  {
    XColor col;
    XParseColor(display, colormap, name, &col);
    XAllocColor(display, colormap, &col);
    return col.pixel;
  } 

//*************************************************************************************************************************
// inicjalizacja zmiennych globalnych okreslajacych kolory

int init_colors(Display* display, int screen_no, Colormap colormap)
{
  background = WhitePixel(display, screen_no);  //niech tlo bedzie biale
  foreground = BlackPixel(display, screen_no);  //niech ekran bedzie czarny
  black=AllocNamedColor("black", display, colormap);
  white=AllocNamedColor("white", display, colormap);
  red=AllocNamedColor("red", display, colormap);
  green=AllocNamedColor("green", display, colormap);
  blue=AllocNamedColor("blue", display, colormap);
  yellow=AllocNamedColor("yellow", display, colormap);
}

int draw_name_letter(Display* display, Window win, GC gc, double dx, double dy, float scale, int color1, int color2, float przedluzenie_dol) {
	XPoint points[] = {
	{20,80 + przedluzenie_dol}, {25,85 + przedluzenie_dol}, {30,80 + przedluzenie_dol}, {30,30}, {50,50}, {70,30}, {70,80 + przedluzenie_dol}, 
	{75,85 + przedluzenie_dol}, {80,80 + przedluzenie_dol}, {80,20}, {75,15}, {70,20}, {50,40}, {30,20}, {25,15}, {20,20}
	};
	for (int i=0; i<16; i++){
		points[i].x *= scale;
		points[i].x += dx;
		points[i].y *= scale;
		points[i].y += dy;
	}
	XSetForeground(display,gc,color1);
	XFillPolygon(display, win, gc, points, 16, Complex, CoordModeOrigin);
	XSetForeground(display,gc,color2);
	XDrawLines(display, win, gc, points, 16, CoordModeOrigin);
	XDrawLine(display, win, gc, points[15].x, points[15].y, points[0].x, points[0].y);
}
int draw_digit(Display* display, Window win, GC gc, double dx, double dy, float scale, int color1, int color2, float przedluzenie_dol) {
	XPoint points[] = {
	{120,80 + przedluzenie_dol}, {170,80 + przedluzenie_dol}, {175,85 + przedluzenie_dol}, {180,80 + przedluzenie_dol}, 
	{180,50 + przedluzenie_dol}, {130,50 + przedluzenie_dol}, {130,30}, {180,30}, {180,20}, {175, 15}, {170, 20}, {120, 20},
	{120,60 + przedluzenie_dol}, {170,60 + przedluzenie_dol}, {170,70 + przedluzenie_dol}, {120,70 + przedluzenie_dol}
	};
	for (int i=0; i<16; i++){
		points[i].x *= scale;
		points[i].x += dx;
		points[i].y *= scale;
		points[i].y += dy;
	}
	XSetForeground(display,gc,color1);
	XFillPolygon(display, win, gc, points, 16, Complex, CoordModeOrigin);
	XSetForeground(display,gc,color2);
	XDrawLines(display, win, gc, points, 16, CoordModeOrigin);
	XDrawLine(display, win, gc, points[15].x, points[15].y, points[0].x, points[0].y);
}
int draw_shield(Display* display, Window window, GC gc, float x, float y, float dx, float dy, float scale, int color){
	XSetForeground(display,gc,color);
	XPoint points[] = {
	{400,0}, {0, 200}, {400, 800}, {800,200}
	};
	XSetForeground(display,gc,color);
	XFillPolygon(display, window, gc, points, 4, Complex, CoordModeOrigin);
	XSetForeground(display,gc,black);
	XDrawLines(display, window, gc, points, 4, CoordModeOrigin);
	XDrawLine(display, window, gc, points[3].x, points[3].y, points[0].x, points[0].y);
        //XFillArc(display,window,gc,x * scale + dx,y * scale + dy,100 * scale,100 * scale,0,360 * 64);
}

//*************************************************************************************************************************
// Glowna funkcja zawierajaca petle obslugujaca zdarzenia */

int main(int argc, char *argv[])
{
  char            icon_name[] = "Grafika";
  char            title[]     = "Grafika komputerowa";
  Display*        display;    //gdzie bedziemy wysylac dane (do jakiego X servera)
  Window          window;     //nasze okno, gdzie bedziemy dokonywac roznych operacji
  GC              gc;         //tu znajduja sie informacje o parametrach graficznych
  XEvent          event;      //gdzie bedziemy zapisywac pojawiajace sie zdarzenia
  KeySym          key;        //informacja o stanie klawiatury 
  Colormap        colormap;
  int             screen_no;
  XSizeHints      info;       //informacje typu rozmiar i polozenie ok
  
  double dx[2] = {200,200};
  double dy[2] = {100,100};
  float scale[2] = {2,2};
  
  char            buffer[8];  //gdzie bedziemy zapamietywac znaki z klawiatury
  int             hm_keys;    //licznik klawiszy
  int             to_end;

  display    = XOpenDisplay("");                //otworz polaczenie z X serverem pobierz dane od zmiennej srodowiskowej DISPLAY ("")
  screen_no  = DefaultScreen(display);          //pobierz domyslny ekran dla tego wyswietlacza (0)
  colormap = XDefaultColormap(display, screen_no);
  init_colors(display, screen_no, colormap);

  //okresl rozmiar i polozenie okna
  info.x = 100;
  info.y = 150;
  info.width = 800;
  info.height = 600;
  info.flags = PPosition | PSize;

  //majac wyswietlacz, stworz okno - domyslny uchwyt okna
  window = XCreateSimpleWindow(display, DefaultRootWindow(display),info.x, info.y, info.width, info.height, 7/* grubosc ramki */, foreground, background);
  XSetStandardProperties(display, window, title, icon_name, None, argv, argc, &info);
  //utworz kontekst graficzny do zarzadzania parametrami graficznymi (0,0) domyslne wartosci
  gc = XCreateGC(display, window, 0, 0);
  XSetBackground(display, gc, background);
  XSetForeground(display, gc, foreground);

  //okresl zdarzenia jakie nas interesuja, np. nacisniecie klawisza
  XSelectInput(display, window, (KeyPressMask | ExposureMask | ButtonPressMask| ButtonReleaseMask | Button1MotionMask));
  XMapRaised(display, window);  //wyswietl nasze okno na samym wierzchu wszystkich okien
      
  to_end = FALSE;

 /* petla najpierw sprawdza, czy warunek jest spelniony
     i jesli tak, to nastepuje przetwarzanie petli
     a jesli nie, to wyjscie z petli, bez jej przetwarzania */
  float timer = 0;
  float przedluzenie = 0;
  int time;
  while (to_end == FALSE)
  {
  
  while (XPending(display) == 0){
  	time++;
  	if (time%5000 == 0){
  	dy[0]+=1;
        dy[1]+=1;
        if (dy[0] >= 600){
        dy[0]=0;
        dy[1]=0;
        }
        XSetForeground(display,gc,white);
		XFillRectangle(display,window,gc,0,0,800,600);
		draw_shield(display,window,gc,100, 0, dx[1],dy[1],scale[1], yellow);
        	draw_name_letter(display,window,gc,dx[0],dy[0],scale[0],green,black,przedluzenie);
        	draw_digit(display,window,gc,dx[1],dy[1],scale[1],red,black,przedluzenie);
        }
        }
    XNextEvent(display, &event);  // czekaj na zdarzenia okreslone wczesniej przez funkcje XSelectInput
    
   

    switch(event.type)
    {
      case Expose:
        if (event.xexpose.count == 0)
        {
        	XSetForeground(display,gc,white);
		XFillRectangle(display,window,gc,0,0,800,600);
		//draw_circle(display,window,gc,0, 0, dx[0],dy[0],scale[0], blue);
		draw_shield(display,window,gc,100, 0, dx[1],dy[1],scale[1], yellow);
        	draw_name_letter(display,window,gc,dx[0],dy[0],scale[0],green,black,0);
        	draw_digit(display,window,gc,dx[1],dy[1],scale[1],red,black,0);
		//draw_circle(display,window,gc,100, 0, dx[1],dy[1],scale[1], green);
        	//draw_surname_letter(display,window,gc,dx[1],dy[1],scale[1],red,black);
        }
        break;

      case MappingNotify:
        XRefreshKeyboardMapping(&event.xmapping); // zmiana ukladu klawiatury - w celu zabezpieczenia sie przed taka zmiana trzeba to wykonac
        break;

      case ButtonPress:
        if (event.xbutton.button == Button1)  // sprawdzenie czy wcisnieto lewy przycisk		
        {
   		
        }
        break;
	
	case MotionNotify:
		//dx[1] = event.xmotion.x - 150 * scale[1];
        	//dy[1] = event.xmotion.y - 50 * scale[1];
        	//scale[1] = dy[1] * 0.005;
        	przedluzenie = event.xmotion.y/2 - 100;
		XSetForeground(display,gc,white);
		XFillRectangle(display,window,gc,0,0,800,600);
		draw_shield(display,window,gc,100, 0, dx[1],dy[1],scale[1], yellow);
		//draw_circle(display,window,gc,0, 0, dx[0],dy[0],scale[0], blue);
        	draw_name_letter(display,window,gc,dx[0],dy[0],scale[0],green,black, przedluzenie);
        	draw_digit(display,window,gc,dx[1],dy[1],scale[1],red,black,przedluzenie);
		//draw_circle(display,window,gc,100, 0, dx[1],dy[1],scale[1], green);
        	//draw_surname_letter(display,window,gc,dx[1],dy[1],scale[1],red,black);
		break;
      case KeyPress:
        hm_keys = XLookupString(&event.xkey, buffer, 8, &key, 0);
        if (hm_keys == 1)
        {
          if (buffer[0] == 'q') to_end = TRUE;        // koniec programu
          else if (buffer[0] == 'z'){
          timer+=0.1;
          dx[1] = 50 * scale[1] * 3.14 * cos(timer) + 200;
          dy[1] = 50 * scale[1] * 3.14 * sin(timer) + 300;
          XSetForeground(display,gc,white);
		XFillRectangle(display,window,gc,0,0,800,600);
		//draw_circle(display,window,gc,0, 0, dx[0],dy[0],scale[0], blue);
        	//draw_name_letter(display,window,gc,dx[0],dy[0],scale[0],red,black);
		//draw_circle(display,window,gc,100, 0, dx[1],dy[1],scale[1], green);
        	//draw_surname_letter(display,window,gc,dx[1],dy[1],scale[1],red,black);
          }
          else if (buffer[0] == 's'){
        dy[0]+=3;
        dy[1]+=3;
        if (dy[0] >= 600){
        dy[0]=0;
        dy[1]=0;
        }
        XSetForeground(display,gc,white);
		XFillRectangle(display,window,gc,0,0,800,600);
		draw_shield(display,window,gc,100, 0, dx[1],dy[1],scale[1], yellow);
        	draw_name_letter(display,window,gc,dx[0],dy[0],scale[0],green,black,0);
        	draw_digit(display,window,gc,dx[1],dy[1],scale[1],red,black,0);
        }
        }

      default:
        break;
    }
  }

  XFreeGC(display, gc);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}