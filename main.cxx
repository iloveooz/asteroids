
	#include <SFML/Graphics.hpp>
	#include <cmath>
	#include <list>
	#include <iostream>
		
	using namespace sf;

	const int W = 1200;
	const int H = 800;

	float DEGTORAD = 0.017453f;

	class Animation {
		public:
		float Frame; 
		float speed;
		
		Sprite sprite;
		std::vector <IntRect> frames;
	
		Animation () {}
		
		Animation (Texture &t, int x, int y, int w, int h, int count, float Speed) {
			Frame = 0;
			speed = Speed;
				 
			for (int i = 0; i < count; i++) 
				frames.push_back(IntRect(x + i * w, y, w, h));
			
			sprite.setTexture(t);
			sprite.setOrigin(w / 2, h / 2);
			sprite.setTextureRect(frames[0]);
		}
		
		void update() {
			Frame += speed;
			int n = frames.size();
			if (Frame >= n) Frame -= n;
			if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
		}
		
		bool isEnd() {
			return Frame + speed >= frames.size();
		}
	};

	class Entity {
		public:
		float x, y;
		float dx, dy;
		float R, angle;
		
		bool life = 1;
		
		std::string name;
		Animation anim;
		
		Entity () { life = 1; }
		
		void settings (Animation &a, int X, int Y, float Angle = 0, int radius = 1) {
			x = X;
			y = Y;
			anim = a;
			angle = Angle;
			R = radius;
		}
		
		virtual void update() { }
		
		void draw(RenderWindow &app) {
			anim.sprite.setPosition(x, y);
			anim.sprite.setRotation(angle + 90);
			app.draw(anim.sprite);
			
		}
		
		virtual ~Entity() { }
	};

	class asteroid : public Entity {
		public:
		asteroid () {
			dx = rand() % 8 - 4;
			dy = rand() % 8 - 4;
			name = "asteroid";
		}
		
		void update() {
			x += dx;
			y += dy;
			
			if (x > W) x = 0;
			if (x < 0) x = W;
			
			if (y > H) y = 0;
			if (y < 0) y = H;
		}
			
	};

	class bullet : public Entity {
		public:
		bullet() {
			name = "bullet";
		}
		
		void update() {
			dx = cos(angle * DEGTORAD) * 6;
			dy = sin(angle * DEGTORAD) * 6;
			x += dx;
			y += dy;
			if (x > W || x < 0 || y > H || y < 0) life = 0;
		}
		
	};

	bool isCollide(Entity *a, Entity *b) { 
		return 	(b->x - a->x) * (b->x - a->x) + 
				(b->y - a->y) * (b->y - a->y) < (a->R + b->R) * (a->R + b->R);
	} 


	int main() {
		
		srand(time(NULL));
		
		std::cout << "START!" << '\n';
		
		// создаём окно
		RenderWindow app(VideoMode(W, H), "Asteroids!");
		app.setFramerateLimit(60);
		
		std::vector <int> vec;
		
		// создаём текстуры
		Texture t1, t2, t3, t4, t5, t6;
		
		// загружаем текстуры из файлов
		t1.loadFromFile("images/spaceship.png");
		t2.loadFromFile("images/background.jpg");
		t3.loadFromFile("images/explosions/type_C.png");
		t4.loadFromFile("images/rock.png");
		t5.loadFromFile("images/fire_blue.png");
		t6.loadFromFile("images/rock_small.png");

		t1.setSmooth(true);
		t2.setSmooth(true);
		
		// создаём спрайты
		Sprite sPlayer(t1);
		Sprite sBackground(t2);
		// Sprite sExplosion(t3);
				
		Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
		
		// отобразить кусок текстуры
		sPlayer.setTextureRect(IntRect(40, 0, 40, 40));
		// точка, вокруг которой происходят все трансформации объекта
		sPlayer.setOrigin(20, 20);
		
		// анимированный камень в космосе
		Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
		// анимированный маленький камень в космосе
		Animation sRockSmall(t6, 0, 0, 64, 64, 16, 0.4);
		// анимированный взрыв
		Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.6);
		
		//sRock.sprite.setPosition(400, 400);
		
		// sExplosion.setPosition(300, 300);
		
		// двусвязный список сущностей
		std::list <Entity *> entities; 
		
		// создание астероидов
		for (int i = 0; i < 25; i++) {
			asteroid *a = new asteroid();
			asteroid *sr = new asteroid();
			a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
			sr->settings(sRockSmall, rand() % W, rand() % H, rand() % 360, 15);
			// вставка элемента в конец контейнера
			entities.push_back(a);
			entities.push_back(sr);
		}
		
		// float Frame = 0;
		// float animSpeed = 0.4;
		// int frameCount = 20;
				
		float x = 300, y = 300;
		float dx = 0, dy = 0, angle = 0;
		
		bool thrust;
		
		// игровой цикл, пока открыто окно
		while (app.isOpen()) {
			// событие
			Event event;
			// пока приложение не закрыто
			while (app.pollEvent(event)) {
				if (event.type == Event::Closed)
				app.close();
				
				if (event.type == Event::KeyPressed) 
					if (event.key.code == Keyboard::Space) {
						bullet *b = new bullet();
						b->settings(sBullet, x, y, angle, 10);
						entities.push_back(b);
					}
				
			}
			
			// пример спрайтовой анимации
			// Frame += animSpeed;
			
			// if (Frame > frameCount) Frame -= frameCount;
			// sExplosion.setTextureRect(IntRect(int(Frame) * 50, 0, 50, 50));
			
			// поворот вправо
			if (Keyboard::isKeyPressed(Keyboard::Right)) angle += 3;
			// поворот влево
			if (Keyboard::isKeyPressed(Keyboard::Left)) angle -= 3;
			// движение прямо
			if (Keyboard::isKeyPressed(Keyboard::Up))
				thrust = true;
			else
				thrust = false;
			
			// цикл проверки на столкновение
			for (auto a:entities) 
				for (auto b:entities) 
					if (a->name == "asteroid" && b->name == "bullet")
						if (isCollide(a, b)) {
							a->life = false;
							b->life = false;
							
							Entity *ex = new Entity();
							ex->settings(sExplosion, a->x, a->y);
							ex->name = "explosion";
							entities.push_back(ex);
							
						}
			
			for (auto ex:entities)
				if (ex->name == "explosion")
					if (ex->anim.isEnd()) {
						ex->life = 0;
					}
							
			// if (Keyboard::isKeyPressed(Keyboard::Down)) thrust = false;
			
			// космический корабль - движение//	
			if (thrust) {
				dx += cos(angle * DEGTORAD) * 0.2;
				dy += sin(angle * DEGTORAD) * 0.2; 
			}
			else {
				dx *= 0.99;
				dy *= 0.99;
			}
			
			int maxSpeed = 15;
			float speed = sqrt(dx * dx + dy * dy);
			
			if (speed > maxSpeed) {
				dx *= maxSpeed / speed;
				dy *= maxSpeed / speed;
			}
			
			x += dx;
			y += dy;
			
			if (x > W) x = 0;
			if (x < 0) x = W;
			
			if (y > H) y = 0;
			if (y < 0) y = H;
			
			sPlayer.setPosition(x, y);
			sPlayer.setRotation(angle + 90);

			// sRock.update();
			/// отображение /// 
			
			// цикл поиска и удаления погибших сущностей
			for (auto i = entities.begin(); i != entities.end();) {
				Entity *e = *i;
				e->update();
				e->anim.update();
				if (e->life == false) {
					i = entities.erase(i);
					delete e;
				}
				else
					i++;
			}
						
			// очистка экрана
			app.clear();
			
			// отображение фона
			app.draw(sBackground);
			
			// отображение игрока
			app.draw(sPlayer);
			// отображение взрыва
			// app.draw(sExplosion);
			
			// отображение камней
			// app.draw(sRock.sprite);
			for (auto i:entities) i->draw(app);
			
			// отображает экран
			app.display();
		}
		
		return 0;
	}
