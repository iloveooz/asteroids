#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <list>
#include <iostream>
#include <vector>

const int W = 1200;
const int H = 800;

int boss = 0;

float DEGTORAD = 0.017453f;

class Animation {
public:
	float Frame, speed;
	sf::Sprite sprite;
	std::vector<sf::IntRect> frames;

	Animation(){}

	Animation (sf::Texture &t, int x, int y, int w, int h, int count, float Speed) {
		Frame = 0;
		speed = Speed;

		for (int i = 0; i < count; i++)
			frames.push_back(sf::IntRect(x + i * w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}

	void update() {
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) 
			Frame -= n;
		if (n > 0) 
			sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd() {
		return Frame + speed >= frames.size();
	}
};

class Entity {
public:
	float x, y, dx, dy, R, angle;
	bool life;
	bool move;
	int hitPoints;

	static int countEntity;

	std::string name;
	Animation anim;

	Entity() {
		life = 1;
	}

	void settings(Animation &a ,int X, int Y, float Angle = 0, int radius = 1) {
		anim = a;
		x = X; 
		y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update(){};

	void draw(sf::RenderWindow &app) {
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		sf::CircleShape circle(R);
		circle.setFillColor(sf::Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
	}

	virtual ~Entity(){};
};

class asteroidBig : public Entity {
public:
	asteroidBig() {
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroidBig";
		countEntity++;
	}

	~asteroidBig() {
		countEntity--;
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

class asteroidSmall : public Entity {
public:
	asteroidSmall() {
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroidSmall";
		countEntity++;
	}

	~asteroidSmall() {
		countEntity--;
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

class bullet: public Entity {
public:
	bullet() {
		name = "bullet";
	}

	void update() {
		dx = cos(angle * DEGTORAD) * 10;
		dy = sin(angle * DEGTORAD) * 10;
  
		x += dx;
		y += dy;

		if (x > W || x < 0 || y > H || y < 0) 
			life = 0;
	}
};


class player: public Entity {
public:
	bool thrust;
	bool decelerate;

	player() {
		name = "player";
	}

	void update() {
		if (thrust) { 
			dx += cos(angle * DEGTORAD) * 0.2;
			dy += sin(angle * DEGTORAD) * 0.2; 
		}
		else {
			dx *= 0.99;
			dy *= 0.99;
		}

		if (decelerate)	{
			dx *= 0.96;
			dy *= 0.96;
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
	}
};

class Enemy : public Entity {
public:
	Enemy() {
		dx = rand() % 6 - 3;
		dy = rand() % 6 - 3;
		name = "enemy";
		countEntity++;
	}

	~Enemy() {
		countEntity--;
	}

	void update() {
		if (move) {
			dx += sqrt(angle * DEGTORAD) * 0.2;
			dy += sqrt(angle * DEGTORAD) * 0.2;
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

		//x += dx;
		//y += dy;

		// проверка на выход за границы окна
		if (x > W) x = 0;
		if (x < 0) x = W;
		if (y > H) y = 0;
		if (y < 0) y = H;
	}
};

bool isCollide(Entity *a, Entity *b) {
	return (b->x - a->x) * (b->x - a->x) + (b->y - a->y) * (b->y - a->y) < (a->R + b->R) * (a->R + b->R);
}

int	Entity::countEntity = 0;

int main() {
	srand(time(0));
	sf::RenderWindow app(sf::VideoMode(W, H), "Asteroids!");
	app.setFramerateLimit(60);

	sf::Texture t1, t2, t3, t4, t5, t6, t7, t8;

	t1.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\spaceship.png");
	t2.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\background.jpg");
	t3.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\explosions\\type_C.png");
	t4.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\rock.png");
	t5.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\fire_blue.png");
	t6.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\rock_small.png");
	t7.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\explosions\\type_B.png");
	t8.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\images\\enemy.png");

	t1.setSmooth(true);
	t2.setSmooth(true);

	sf::Sprite background(t2);

	sf::Font font;
	font.loadFromFile("C:\\git\\2017\\Asteroids\\Debug\\fonts\\times.ttf");

	sf::Text text("", font, 20);

	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sEnemy(t8, 0, 0, 224, 154, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

	std::list<Entity*> entities;

	for(int i = 0; i < 3; i++) {
		switch (rand() % 2) {
		case 0: {
			Entity *aB = new asteroidBig();
			aB->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
			entities.push_back(aB);
			break;
		}
		case 1: {
			Entity *aS = new asteroidSmall();
			aS->settings(sRock_small, rand() % W, rand() % H, rand() % 360, 15);
			entities.push_back(aS);
			break;
		}
		default:
			break;
		}
	}

	player *p = new player();
	p->settings(sPlayer, 200, 200, 0, 20);
	entities.push_back(p);

	/////main loop/////
	while (app.isOpen()) {
		sf::Event event;
		while (app.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				app.close();

			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Space) {

					bullet *bul1 = new bullet();
					bullet *bul2 = new bullet();

					int X1 = p->x + 15 * sin(p->angle);
					int Y1 = p->y + 15 * cos(p->angle);

					int X2 = p->x + 15 * sin(p->angle + 180);
					int Y2 = p->y + 15 * cos(p->angle + 180);

					bul1->settings(sBullet, X1, Y1, p->angle, 10);
					bul2->settings(sBullet, X2, Y2, p->angle, 10);

					entities.push_back(bul1);
					entities.push_back(bul2);
				}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			p->angle += 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			p->angle -= 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			p->thrust = true;
		else 
			p->thrust = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			p->decelerate = true;

		for(auto a:entities)
			for(auto b:entities) {
				if ((a->name == "asteroidBig" && b->name == "bullet") || (a->name == "asteroidSmall" && b->name == "bullet")) {
					if (isCollide(a, b)) {
						a->life = false;
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						for (int i = 0; i < 1; i++) { // 5 - количество астероидов
							if (a->R == 5 || a->name == "asteroidSmall") continue;
							Entity *e = new asteroidSmall();
							e->settings(sRock_small, a->x + rand() % 50, a->y + rand() % 50, rand() % 360, 15);
							entities.push_back(e);
						}
					}
				}

				if ((a->name == "player" && b->name == "asteroidBig") || (a->name == "player" && b->name == "asteroidSmall")) {
					if (isCollide(a, b)) {
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						p->settings(sPlayer, W / 2, H / 2, 0, 20);
						p->dx = 0;
						p->dy = 0;
					}
				}

				if (a->name == "asteroidBig" && b->name == "asteroidSmall") {
					if (isCollide(a, b)) {
						a->life = false;
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);
					}
				}

				if (a->name == "enemy" && b->name == "bullet") {
					if (isCollide(a, b)) {
						a->hitPoints--;

						if (a->hitPoints == 0)
							a->life = false;

						std::cout << "Boss hitpoints: " << a->hitPoints << '\n';
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion, a->x - 112 + rand() % 224, a->y - 77 + rand() % 154);
						e->name = "explosion";
						entities.push_back(e);
					}
				}

				if (a->name == "player" && b->name == "enemy") {
					if (isCollide(a, b)) {
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						p->settings(sPlayer, W / 2, H / 2, 0, 20);
						p->dx = 0;
						p->dy = 0;
					}
				}

			}

		if (p->thrust)
			p->anim = sPlayer_go;
		else   
			p->anim = sPlayer;

		for(auto e:entities)
			if (e->name == "explosion")
				if (e->anim.isEnd())
					e->life = 0;

		if (rand() % 150 == 0 && Entity::countEntity > 1) {
			Entity *a = new asteroidBig();
			a->settings(sRock, 0, rand() % H, rand() % 360, 25);
			entities.push_back(a);
		}

		//if (rand)

		for(auto i = entities.begin(); i != entities.end();) {
			Entity *e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) {
				i = entities.erase(i); 
				delete e;
				std::cout << Entity::countEntity << '\n';
			}
			else 
				i++;
		}

		if (Entity::countEntity == 0 && boss == 0) {
			Enemy *en = new Enemy();
			en->settings(sEnemy, 1000, 400, 90, 20);
			en->hitPoints = 500;
			entities.push_back(en);
			boss = 1;
		}

		for (auto a : entities) {
			if (boss && a->name == "enemy") {
				int dd = rand() % 2;
				switch (dd) {
				case 1: 
					//a->angle += rand() % 1; 
					break;
				case 2: 
					//a->angle -= rand() % 10; 
					break;
				default:
					break;
				}
				a->move = true;
			}
			else {
				a->move = false;
			}
		}

		//////draw//////
		app.draw(background);

		for(auto i:entities)
			i->draw(app);

		app.display();
	}

	return 0;
}
