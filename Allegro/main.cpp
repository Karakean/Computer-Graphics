#include <math.h>
#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;

int main()
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	bool redraw = true;

	if (!al_init()) {
		fprintf(stderr, "Nie zainicjalizowano allegro!\n");
		return -1;
	}
	
	display = al_create_display(SCREEN_W, SCREEN_H);
	timer = al_create_timer(1.0 / FPS);
	al_install_keyboard();
	event_queue = al_create_event_queue();
	
	al_init_primitives_addon();

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	ALLEGRO_COLOR yellow = al_map_rgb(255, 255, 0);
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR blue = al_map_rgb(0, 0, 255);
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);

	const int N = 7;
	const int r = 50;
	const int R = 50;
	float dx[N];
	float dy[N];
	float alfa = 2 * ALLEGRO_PI / N;
	for (int i = 0; i < N; i++) {
		float angle = alfa * i;
		dx[i] = r * cos(angle);
		dy[i] = r * sin(angle);
	}

	float points[2*N];

	float directionx = 0;
	float directiony = 0;
	float directionx_change = 5;
	float directiony_change = 5;

	double fi=0.0, dfi=0.1, sinfi, cosfi;

	al_start_timer(timer);
	
	while(true)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			break;
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) 
		{
			redraw = false;
			al_clear_to_color(black);

			int xm = SCREEN_W / 2;
			int ym = SCREEN_H / 2;

			if ((xm + directionx >= SCREEN_W - r) || (xm + directionx <= r))
				directionx_change *= -1;
			if ((ym + directiony >= SCREEN_H - r) || (ym + directiony <= r))
				directiony_change *= -1;

			sinfi = sin(fi);
			cosfi = cos(fi);
			for (int i = 0; i < N; i++)
			{
				points[2 * i] = (dx[i] * cosfi - dy[i] * sinfi) + xm + directionx;
				points[2 * i + 1] = (dx[i] * sinfi + dy[i] * cosfi) + ym + directiony;
			}
			fi += dfi;
			directionx += directionx_change;
			directiony += directiony_change;

			al_draw_circle(xm, ym, R, yellow, 2);
			al_draw_circle(xm + 2 * R * sinfi, ym + 2 * R * cosfi, r, yellow, 2);

			al_draw_polygon(points, N, 1, white, 2 , 1);

			al_flip_display();
		}
	}

	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	return 0;
}
