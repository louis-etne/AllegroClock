#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define M_PI 3.14159265358979323846
#define M_2PI 2 * M_PI

#define FPS 1
#define WIDTH 600
#define HEIGHT 600


struct Clock
{
	int hours{ 0 };
	int minutes{ 0 };
	int seconds{ 0 };

	Clock()
	{
		refresh();
	}

	void refresh()
	{
		std::chrono::system_clock::time_point now{ std::chrono::system_clock::now() };
		time_t tt{ std::chrono::system_clock::to_time_t(now) };
		tm local_tm;
		localtime_s(&local_tm, &tt);

		hours = local_tm.tm_hour;
		minutes = local_tm.tm_min;
		seconds = local_tm.tm_sec;
	}

	void draw(double x, double y, double r)
	{
		refresh();

		// Draw dial
		al_draw_filled_circle(x, y, r, al_map_rgb(255, 255, 255));

		// Draw light graduations
		for (double i{ 0.0 }; i < M_2PI; i += M_2PI / 60) {
			al_draw_line(
				x + std::cos(i) * r
				, y + std::sin(i) * r
				, x + std::cos(i) * (r - double(10))
				, y + std::sin(i) * (r - double(10))
				, al_map_rgb(0, 0, 0)
				, 3
			);
		}

		// Draw bold graduations
		for (double i{ 0.0 }; i < M_2PI; i += M_2PI / 12) {
			al_draw_line(
				x + std::cos(i) * r
				, y + std::sin(i) * r
				, x + std::cos(i) * (r - double(20))
				, y + std::sin(i) * (r - double(20))
				, al_map_rgb(0, 0, 0)
				, 4
			);
		}

		// Draw hours hand
		al_draw_line(
			x - std::sin(hours * (M_2PI / 12)) * 20
			, y + std::cos(hours * (M_2PI / 12)) * 20
			, x + std::sin(hours * (M_2PI / 12)) * (r - double(80))
			, y - std::cos(hours * (M_2PI / 12)) * (r - double(80))
			, al_map_rgb(255, 0, 0)
			, 10
		);

		// Draw minutes hand
		al_draw_line(
			x - std::sin(minutes * (M_2PI / 60)) * 20
			, y + std::cos(minutes * (M_2PI / 60)) * 20
			, x + std::sin(minutes * (M_2PI / 60)) * (r - double(20))
			, y - std::cos(minutes * (M_2PI / 60)) * (r - double(20))
			, al_map_rgb(0, 255, 0)
			, 5
		);

		// Draw seconds hand
		al_draw_line(
			x - std::sin(seconds * (M_2PI / 60)) * 20
			, y + std::cos(seconds * (M_2PI / 60)) * 20
			, x + std::sin(seconds * (M_2PI / 60)) * (r - double(5))
			, y - std::cos(seconds * (M_2PI / 60)) * (r - double(5))
			, al_map_rgb(0, 0, 255)
			, 2
		);

		// Draw center circle
		al_draw_filled_circle(x, y, 10, al_map_rgb(0, 0, 0));
	}

};


int main()
{
	bool doexit{ false };
	bool redraw{ true };
	

	if (!al_init()) {
		std::cerr << "Failed to initialize Allegro" << std::endl;
		return -1;
	}


	if (!al_install_keyboard()) {
		std::cerr << "Failed to initialize the keyboard!" << std::endl;
		return -1;
	}


	if (!al_init_primitives_addon()) {
		std::cerr << "Failed to initialize Allegro primitives addon" << std::endl;
		return -1;
	}


	/*if (!al_init_font_addon()) {
		std::cerr << "Failed to initialize Allegro font addon" << std::endl;
		return -1;
	}*/


	// DISPLAY
	ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);

	if (!display) {
		std::cerr << "Failed to create display" << std::endl;
		al_destroy_display(display);
		return -1;
	}


	// TIMER
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	if (!timer) {
		std::cerr << "Failed to create timer" << std::endl;
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}


	// EVENTS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

	if (!event_queue) {
		std::cerr << "Failed to create event_queue" << std::endl;
		al_destroy_event_queue(event_queue);
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	// DRAW
	Clock clock;

	al_clear_to_color(al_map_rgb(0, 0, 0));
	clock.draw(WIDTH / 2, HEIGHT / 2, HEIGHT / 2);
	al_flip_display();

	al_start_timer(timer);

	while (!doexit) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);


		if (event.type == ALLEGRO_EVENT_TIMER) {
			clock.refresh();
			redraw = true;
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				doexit = true;
				break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, 0));
			clock.draw(WIDTH / 2, HEIGHT / 2, HEIGHT / 2);
			al_flip_display();
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}