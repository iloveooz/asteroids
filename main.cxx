
	#include <SFML/Graphics.hpp>
	#include <cmath>
	#include <list>
	#include <vector>
	#include <iostream>

	const int W = 1200;
	const int H = 800;

	float DEGTORAD = 0.017453f;


	class Animation 
	{
		public:
		float Frame; 
		float speed;
		
		sf::Sprite sprite;
		std::vector <sf::IntRect> frames;
	
		Animation () {}
		
		Animation (sf::Texture &t, int x, int y, int w, int h, int count, float Speed)
		{
			Frame = 0;
			speed = Speed;
				 
			for (int i = 0; i < count; i++) 
				frames.push_back(sf::IntRect(x + i * w, y, w, h));
			
			sprite.setTexture(t);
			sprite.setOrigin(w / 2, h / 2);
			sprite.setTextureRect(frames[0]);
		}
		
		void update()
		{
			Frame += speed;
			int n = frames.size();
			if (Frame >= n) Frame -= n;
			if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
		}
		
		bool isEnd()
		{
			return Frame + speed >= frames.size();
		}
	};


	class Entity 
	{
		public:
		float x, y;
		float dx, dy;
		float R, angle;
		
		bool life = 1;
		
		// ускорение
		bool thrust;
		
		// торможение
		bool brake;
		
		std::string name;
		Animation anim;
		
		Entity () 
		{
			life = 1;
		}
		
		void settings (Animation &a, int X, int Y, float Angle = 0, int radius = 1) 
		{
			x = X;
			y = Y;
			anim = a;
			angle = Angle;
			R = radius;
		}
		
		virtual void update() {}
		
		void draw(sf::RenderWindow &app) 
		{
			anim.sprite.setPosition(x, y);
			anim.sprite.setRotation(angle + 90);
			app.draw(anim.sprite);
		}
		
		virtual ~Entity() {}
	};


	class asteroid : public Entity 
	{
		public:
		asteroid () 
		{
			dx = rand() % 8 - 4;
			dy = rand() % 8 - 4;
			name = "asteroid";
		}
		
		void update()
		{
			x += dx;
			y += dy;
			
			if (x > W) x = 0;
			if (x < 0) x = W;
			
			if (y > H) y = 0;
			if (y < 0) y = H;
		}
			
	};

	class Player : public Entity
	{
		public:
		Player() 
		{
			name = "player";
			x = 300;
			y = 300;
			dx = 0;
			dy = 0;
			
			angle = 0;
		}
		
		void update() 
		{
			// космический корабль - ускорение	
			if (thrust) 
			{
				dx += cos(angle * DEGTORAD) * 0.2;
				dy += sin(angle * DEGTORAD) * 0.2; 
			}
			else 
			{
				dx *= 0.99;
				dy *= 0.99;
			}
			
			// космический корабль торможение и движение назад
			if (brake) 
			{
				dx -= cos(angle * DEGTORAD) * 0.2;
				dy -= sin(angle * DEGTORAD) * 0.2; 
			}		
			
			int maxSpeed = 15;
			float speed = sqrt(dx * dx + dy * dy);
			
			if (speed > maxSpeed)
			{
				dx *= maxSpeed / speed;
				dy *= maxSpeed / speed;
			}
			
			x += dx;
			y += dy;
			
			if (x > W) x = 0;
			if (x < 0) x = W;
			
			if (y > H) y = 0;
			if (y < 0) y = H;
		}
	};

	class bullet : public Entity 
	{
		public:
		bullet()
		{
			name = "bullet";
		}
		
		void update()
		{
			dx = cos(angle * DEGTORAD) * 6;
			dy = sin(angle * DEGTORAD) * 6;
			x += dx;
			y += dy;
			if (x > W || x < 0 || y > H || y < 0) life = 0;
		}
		
	};

	bool isCollide(Entity *a, Entity *b)
	{ 
		return 	(b->x - a->x) * (b->x - a->x) + (b->y - a->y) * (b->y - a->y) < (a->R + b->R) * (a->R + b->R);
	} 


	int main()
	{
		
		srand(time(NULL));
		
		std::cout << "START!" << '\n';
		
		// создаём окно
		sf::RenderWindow app(sf::VideoMode(W, H), "Asteroids!");
		app.setFramerateLimit(60);
		
		std::vector <int> vec;
		
		// создаём текстуры
		sf::Texture t1, t2, t3, t4, t5, t6;
		
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
		sf::Sprite sPlayer(t1);
		sf::Sprite sBackground(t2);
						
		Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
		
		// отобразить кусок текстуры
		sPlayer.setTextureRect(sf::IntRect(40, 0, 40, 40));
		
		// точка, вокруг которой происходят все трансформации объекта
		sPlayer.setOrigin(20, 20);
		
		// анимированный камень в космосе
		Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
		
		// анимированный маленький камень в космосе
		Animation sRockSmall(t6, 0, 0, 64, 64, 16, 0.4);
		
		// анимированный взрыв
		Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.6);
		
		// двусвязный список сущностей
		std::list <Entity *> entities; 
		
		// создание астероидов
		for (int i = 0; i < 25; i++) 
		{
			asteroid *a = new asteroid();
			asteroid *sr = new asteroid();
			a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
			sr->settings(sRockSmall, rand() % W, rand() % H, rand() % 360, 15);
			// вставка элемента в конец контейнера
			entities.push_back(a);
			entities.push_back(sr);
		}
		
		Player *spaceShip = new Player;
		
		entities.push_back(spaceShip);
		
		// игровой цикл, пока открыто окно
		while (app.isOpen()) 
		{
			// событие
			sf::Event event;
			// пока приложение не закрыто
			while (app.pollEvent(event)) 
			{
				if (event.type == sf::Event::Closed)
				app.close();
				
				if (event.type == sf::Event::KeyPressed) 
					if (event.key.code == sf::Keyboard::Space)
					{
						bullet *b = new bullet();
						b->settings(sBullet, x, y, angle, 10);
						entities.push_back(b);
					}
				
			}

			// поворот вправо
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  
				spaceShip->angle += 3;
			 // поворот влево
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
				spaceShip->angle -= 3;
			 // движение прямо
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				spaceShip->thrust = true;
			else
				spaceShip->thrust = false;
			
			// движение назад и торможение
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				spaceShip->brake = true;
			else
				spaceShip->brake = false;
			
			// цикл проверки на столкновение
			for (auto a:entities) 
				for (auto b:entities) 
					if (a->name == "asteroid" && b->name == "bullet")
						if (isCollide(a, b)) 
						{
							a->life = false;
							b->life = false;
							
							Entity *ex = new Entity();
							ex->settings(sExplosion, a->x, a->y);
							ex->name = "explosion";
							entities.push_back(ex);
							
						}
			
			for (auto ex:entities)
				if (ex->name == "explosion")
					if (ex->anim.isEnd())
					{
						ex->life = 0;
					}
							
			sPlayer.setPosition(x, y);
			sPlayer.setRotation(angle + 90);

			// отображение
			
			// цикл поиска и удаления погибших сущностей
			for (auto i = entities.begin(); i != entities.end();) 
			{
				Entity *e = *i;
				e->update();
				e->anim.update();
				if (e->life == false) 
				{
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
			
			// отображение всех сущностей
			for (auto i:entities) i->draw(app);
			
			// отображает экран
			app.display();
		}
		
		return 0;
	}
