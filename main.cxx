
	#include <SFML/Graphics.hpp>
	
	using namespace sf;
	
	const int W = 1200;
	const int H = 800;
	
	
	int main() {
		
		RenderWindow app(VideoMode(W, H), "Asteroids!");
		app.setFramerateLimit(60);
		
		Texture t1, t2;
		
		t1.loadFromFile("images/spaceship.png");
		t2.loadFromFile("images/background.jpg");
		
		
		return 0;
	}
