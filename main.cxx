
	#include <SFML/Graphics.hpp>
	
	using namespace sf;
	
	const int W = 1200;
	const int H = 800;
	
	float DEGTORAD = 0.017453f;
	
	int main() {
		
		// создаём окно
		RenderWindow app(VideoMode(W, H), "Asteroids!");
		app.setFramerateLimit(60);
		
		// создаём текстуры
		Texture t1, t2;
		
		// загружаем текстуры из файлов
		t1.loadFromFile("images/spaceship.png");
		t2.loadFromFile("images/background.jpg");
		
		// создаём спрайты
		Sprite sPlayer(t1), sBackground(t2);
		// отобразить кусок текстуры
		sPlayer.setTextureRect(IntRect(40, 0, 40, 40));
		// точка, вокруг которой происходят все трансформации объекта
		sPlayer.setOrigin(20, 20);
		
		// игровой цикл, пока открыто окно
		while (app.isOpen()) {
			// событие
			Event event;
			// пока приложение не закрыто
			while (app.pollEvent(event)) {
				if (event.type == Event::Closed)
				app.close();
			}
			
			/// отображение /// 
			app.clear();
			app.draw(sBackground);
			app.draw(sPlayer);
			app.display();
		}
		
		
		return 0;
	}
