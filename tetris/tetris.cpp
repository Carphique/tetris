#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <time.h>
using namespace sf;
using namespace std;

const int M = 20; // playing field height высота игрового поля 
const int N = 14; // playing field width ширина игрового поля
const int FIGURES_COUNT = 7;

struct Point
{
	int x, y;
};

// Checking for going beyond the boundaries of the playing field
// Проверка на выход за границы игрового поля
bool check(Point a[4], int field[M][N])
{
	for (int i = 0; i < 4; i++)
	{
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0; // out of bounds выход за границы
		else if (field[a[i].y][a[i].x]) return 0; // occupied cell занятая клетка
	}
	return 1; // valid position
};

void deleteLineIfNeeded(int field[M][N], int& score, Text& textScore, char message[200])
{
	int k = M - 1;
	for (int i = M - 1; i > 0; i--)
	{
		int count = 0; // how many cells are occupied in a line сколько занято клеток в строке
		for (int j = 0; j < N; j++)
		{
			if (field[i][j])
			{
				count++;
			}

			field[k][j] = field[i][j];
		}
		if (count < N)
			k--; //no need to delete не надо удалять строку
		else
		{
			score = score + 10; // add points!
			sprintf_s(message, 200, "your score: %d", score);
			textScore.setString(message); // update message text
		}
	}
}

void drawField(int field[M][N], Sprite& sprite, RenderWindow& window)
{
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
		{
			if (field[i][j] != 0)
			{
				sprite.setTextureRect(IntRect((field[i][j] - 1) * 18, 0, 18, 18));
				sprite.setPosition(j * 18.f, i * 18.f);
				sprite.move(29, 32); // displacement смещение
				window.draw(sprite);
			}
		}
}

void drawTetramino(Point a[4], Sprite& sprite, int colorNum, RenderWindow& window)
{
	for (int i = 0; i < 4; i++)
	{
		// Draw tetramino Разукрашиваем тетрамино
		sprite.setTextureRect(IntRect((colorNum - 1) * 18, 0, 18, 18));

		// Set position of every peace of tetramino Устанавливаем позицию каждого кусочка тетрамино
		sprite.setPosition(a[i].x * 18.f, a[i].y * 18.f);

		sprite.move(29, 32); // displacement смещение

		// Draw sprite Отрисовка спрайта
		window.draw(sprite);

	}
}

void resetGame(int field[M][N], int& score, Point a[4], Point b[4], int& colorNum, int& n, bool& end, float& timer, float& seconds, int& figureCount, bool& beginGame) {
	// Clear playing field Очистить игровое поле
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			field[i][j] = 0;
		}
	}

	// Resetting points Сброс очков
	score = 0;

	// Timer reset Сброс таймера
	timer = 0;
	seconds = 1.0f;

	// Resetting figure variables Сброс переменных фигуры
	n = rand() % FIGURES_COUNT;
	colorNum = 1 + rand() % 8;
	for (int i = 0; i < 4; i++) {
		a[i].x = 0;
		a[i].y = 0;
		b[i] = a[i];
	}

	// Resetting the game state Сброс состояния игры
	end = false;
	figureCount = 0;
	beginGame = true;
}


int main()
{
	// An array of tetramino figures Массив фигурок-тетрамино
	const int FIGURES[7][4] =
	{
		1,3,5,7, // I
		2,4,5,7, // Z
		3,5,4,6, // S
		3,5,4,7, // T
		2,3,5,7, // L
		3,5,7,6, // J
		2,3,4,5, // O
	};
	char message[200];
	int score = 0;
	int field[M][N] = { 0 }; // playing field игровое поле

	Point a[4] = { 0 }, b[4] = { 0 };

	srand((unsigned int)time(0));

	RenderWindow window(VideoMode(320, 480), "Tetris");

	Texture texture, texture_background, texture_frame, texture_cloud, texture_pause, texture_play, texture_menu;
	texture.loadFromFile("../../images/tiles1.png");
	texture_background.loadFromFile("../../images/background2.png");
	texture_frame.loadFromFile("../../images/frame2.png");
	texture_cloud.loadFromFile("../../images/cloud320.png");
	texture_pause.loadFromFile("../../images/pause.png");
	texture_play.loadFromFile("../../images/play.png");
	texture_menu.loadFromFile("../../images/menu.png");

	// Creating sprite Создание спрайта
	Sprite sprite(texture), sprite_background(texture_background), sprite_frame(texture_frame), sprite_cloud(texture_cloud), sprite_pause(texture_pause), sprite_menu(texture_menu);

	// Cut a separate 18x18 pixel square from the sprite
	// Вырезаем из спрайта отдельный квадратик размером 18х18 пикселей
	sprite.setTextureRect(IntRect(0, 0, 18, 18));

	// Variables for horizontal movement and rotation
	// Переменные для горизонтального перемещения и вращения
	int dx = 0;
	bool rotate = 0;
	int colorNum = 1 + rand() % 8;
	bool beginGame = true;
	int n = rand() % FIGURES_COUNT;
	int figureCount = 0;
	float seconds = 1.0f;

	// Variables for timer and delay
	// Переменные для таймера и задержки
	float timer = 0, delay = seconds;

	// Variables for the pause button
	// Переменные для кнопки паузы
	bool isPaused = false;
	float pauseButtonX = 285;
	float pauseButtonY = 40;

	// Menu variables
	// Переменные меню
	bool isMenuActive = false; //  Menu active or not Меню активно или нет
	RectangleShape screenOverlay; // Screen darkening Затемнение экрана
	screenOverlay.setSize(Vector2f(320, 480));
	screenOverlay.setFillColor(Color(0, 0, 0, 150));
	float menuButtonX = 285;
	float menuButtonY = 5; // Menu button position Позиция кнопки меню


	// Clock (timer) Часы (таймер)
	Clock clock;
	// Font Шрифт 
	Font font;
	font.loadFromFile("../../images/arial.ttf");

	Text textScore;
	textScore.setFont(font);
	textScore.setFillColor(Color::Black);

	sprintf_s(message, 200, "your score: %d", score);
	textScore.setString(message);


	Text textGameOver("Game Over", font);
	textGameOver.setFillColor(Color::Red);

	Text textStartAgain("Press S to play again :)", font);
	textStartAgain.setFillColor(Color::Green);

	bool end = false;

	// variables for cloud coordinates
	// переменные для координат облака
	float cloud_x1 = 0;
	float cloud_x2 = -500;

	// The main application loop. Runs while the window is open
	// Главный цикл приложения. Выполняется, пока открыто окно
	while (window.isOpen())
	{
		// Get the time elapsed since the start of the countdown and convert it to seconds
		// Получаем время, прошедшее с начала отсчета, и конвертируем его в секунды
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		if (!isPaused || !isMenuActive)
		{
			timer += time;
		} // The timer is only updated if the game is not paused Таймер обновляется только если игра не на паузе

		// Process the event queue in a loop Обрабатываем очередь событий в цикле
		Event event;
		while (window.pollEvent(event))
		{
			// The user clicked on the “cross” and wants to close the window?
			// Пользователь нажал на «крестик» и хочет окно закрыть?
			if (event.type == Event::Closed)
				// so close it
				// тогда закрываем его
				window.close();

			// Was a button on the keyboard pressed? Была нажата кнопка на клавиатуре?
			if (event.type == Event::KeyPressed) {
				// That button is the up arrow...? Эта кнопка – стрелка вверх?… 
				if (event.key.code == Keyboard::Up && !isPaused && !isMenuActive) rotate = true;
				// ...Or is it a left arrow?...? …или же стрелка влево?…
				else if (event.key.code == Keyboard::Left && !isPaused && !isMenuActive) dx = -1;
				// ...well, then maybe it's a right arrow? …ну тогда может это стрелка вправо?
				else if (event.key.code == Keyboard::Right && !isPaused && !isMenuActive) dx = 1;
			}
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left) {
					// Menu button processing Обработка кнопки меню
					if (sprite_menu.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
						isMenuActive = !isMenuActive; // Switching the menu status Переключение состояния меню
						if (isMenuActive) {
							isPaused = true; // Activate pause Активируем паузу
						}
					}
					// Handling the pause button, but only if the menu is not active
					// Обработка кнопки паузы, но только если меню не активно
					else if (!isMenuActive && sprite_pause.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
						isPaused = !isPaused; // Switching the pause state Переключаем состояние паузы
						sprite_pause.setTexture(isPaused ? texture_play : texture_pause); // Update texture Обновляем текстуру
					}
				}
			}
			if (event.key.code == Keyboard::S) {
				resetGame(field, score, a, b, colorNum, n, end, timer, seconds, figureCount, beginGame);
			}
		}

		if (!isPaused && !isMenuActive) {
			//// Horizontal movement ////
			//// Горизонтальное перемещение ////
			for (int i = 0; i < 4; i++)
			{
				b[i] = a[i];
				a[i].x += dx;
			}

			// Out of the field after moving? Then return the old coordinates 
			// Вышли за пределы поля после перемещения? Тогда возвращаем старые координаты 
			if (!check(a, field))
			{
				for (int i = 0; i < 4; i++)
					a[i] = b[i];
			}

			//// Rotation ////
			//// Вращение ////
			if (rotate)
			{
				Point p = a[1]; // set the center of rotation задаем центр вращения
				for (int i = 0; i < 4; i++)
				{
					int x = a[i].y - p.y; //y-y0
					int y = a[i].x - p.x; //x-x0
					a[i].x = p.x - x;
					a[i].y = p.y + y;
				}
				// Out of the field after turning? Then return the old coordinates 
				// Вышли за пределы поля после поворота? Тогда возвращаем старые координаты 
				if (!check(a, field))
				{
					for (int i = 0; i < 4; i++) {
						a[i] = b[i];
					}
				}
			}

			// Fast falling
			// Ускоренное падение
			if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05f;

			// Downward tetramino movement (Timer Tick) ////
			//// Движение тетрамино вниз (Тик таймера) ////
			if (timer > delay)
			{
				for (int i = 0; i < 4; i++)
				{
					b[i] = a[i];
					a[i].y += 1;
				}

				if (!check(a, field)) { // When you want to seal an old figure and add a new one Когда нужно запечатать старую фигуру и добавить новую
					if (!end)
					{
						figureCount++;
						// Increase the speed every 5 figures
						// Увеличиваем скорость каждые 5 фигур
						if (figureCount % 5 == 0 && seconds > 0.2f) {
							seconds -= 0.2f;
						}

						for (int i = 0; i < 4; i++)
							field[b[i].y][b[i].x] = colorNum; // Here in the field we're sealing the old figure Здесь в field мы запечатываем старую фигуру

						score += 1;
						sprintf_s(message, 200, "your score: %d", score);
						textScore.setString(message);

						// If the figure reaches the top of the screen
						// Если фигура дошла до верхней части экрана
						if (a[0].y < 3)
							end = true;
						else
						{
							// new figure Новая фигура
							colorNum = 1 + rand() % 8;
							n = rand() % FIGURES_COUNT;
							for (int i = 0; i < 4; i++)
							{
								a[i].x = FIGURES[n][i] % 2 + N / 2 - 1;
								a[i].y = FIGURES[n][i] / 2;
							}
						}
					}
				}
				timer = 0;
			}
			// cloud animation анимация облаков
			if (!end)
			{
				cloud_x1 += 0.008;
				if ((int)cloud_x1 == 150)
				{
					cloud_x2 = -290;
				}

				if (cloud_x2 >= -290)
				{
					cloud_x2 += 0.008;
				}

				if ((int)cloud_x2 == 150)
				{
					cloud_x1 = -290;
				}
			}
		}

		if (!isPaused || !isMenuActive) {
			deleteLineIfNeeded(field, score, textScore, message);
		}

		// Tetramino's first appearance on the field? Первое появление тетрамино на поле?
		if (beginGame)
		{
			beginGame = false;
			n = rand() % FIGURES_COUNT;
			for (int i = 0; i < 4; i++)
			{
				a[i].x = FIGURES[n][i] % 2 + N / 2 - 1;
				a[i].y = FIGURES[n][i] / 2;
			}
		}
		dx = 0; rotate = 0; delay = seconds;

		//---DRAWING---//
		//----ОТРИСОВКА----//

		window.clear();
		window.draw(sprite_background);

		// drawing figures that falled
		// рисует фигуры которые уже прилетели
		drawField(field, sprite, window);

		if (!end)
			drawTetramino(a, sprite, colorNum, window); // figure that flying фигура которая летит

		sprite_menu.setPosition(menuButtonX, menuButtonY);
		window.draw(sprite_menu);

		textScore.setPosition(0, 445);
		window.draw(textScore);

		sprite_cloud.setPosition(cloud_x1, 45);
		window.draw(sprite_cloud);

		sprite_cloud.setPosition(cloud_x2, 45);
		window.draw(sprite_cloud);

		window.draw(sprite_frame);

		// pause position устанавливаем позицию паузы
		sprite_pause.setPosition(pauseButtonX, pauseButtonY);
		window.draw(sprite_pause);

		if (end)
		{
			textGameOver.setPosition(70, 180);
			window.draw(textGameOver);
		}

		if (isMenuActive) {
			// Screen darkening Затемнение экрана
			window.draw(screenOverlay);
			sprite_pause.setTexture(texture_play);
			if (end) {
				textStartAgain.setPosition(5, 90);
				window.draw(textStartAgain);
				if (Keyboard::isKeyPressed(Keyboard::S)) {
					resetGame(field, score, a, b, colorNum, n, end, timer, seconds, figureCount, beginGame);
				}
			}
		}
		else {
			// Continued gameplay Продолжение игрового процесса
			window.draw(sprite_frame);
			sprite_pause.setPosition(pauseButtonX, pauseButtonY);
			window.draw(sprite_pause);
		}

		// Draw window Отрисовка окна
		window.display();
	}
}
