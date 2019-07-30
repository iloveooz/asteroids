#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
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

	Animation() {}

	Animation(sf::Texture &t, int x, int y, int w, int h, int count, float Speed) {
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
	double x, y, dx, dy, R, angle;
	bool life;
	bool move;
	int hitPoints;

	static int countEntity;

	std::string name;
	Animation anim;

	Entity() {
		life = 1;
	}

	void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1) {
		anim = a;
		x = X;
		y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update() {};

	void draw(sf::RenderWindow &app) {
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		sf::CircleShape circle(R);
		circle.setFillColor(sf::Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}
	
	virtual ~Entity() {};
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

class bullet : public Entity {
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

class rotateBullet : public Entity {
	double rotateBulletRadius;
	int x0;
	int y0;
public:
	rotateBullet() {
		name = "rotateBullet";
		rotateBulletRadius = 0;
		x0 = x;
		y0 = y;
	}

	void update() {
		rotateBulletRadius += 0.2;
		dx = x0 + rotateBulletRadius * cos(angle++ * DEGTORAD);
		dy = y0 + rotateBulletRadius * sin(angle++ * DEGTORAD);

		int maxSpeed = 15;

		float speed = sqrt(dx * dx + dy * dy);

		if (speed > maxSpeed) {
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x > W || x < 0 || y > H || y < 0)
			life = 0;
	}
};

class Player : public Entity {
public:
	bool thrust;
	bool decelerate;

	Player() {
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

		if (decelerate) {
			dx *= 0.97;
			dy *= 0.97;
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
	Player *player_;
public:
	Enemy(Player* player) : player_(player) {
		name = "enemy";
		countEntity++;
	}

	~Enemy() {
		countEntity--;
	}

	void update() {
		if (move) {
			double distance = sqrt((player_->getX() - x) * (player_->getX() - x) + (player_->getY() - y) * (player_->getY() - y));

			dx += (player_->getX() - x) / distance;
			dy += (player_->getY() - y) / distance;

			angle = (atan2((player_->getY() - y) * DEGTORAD, (player_->getX() - x) * DEGTORAD) * 180 / 3.14159265);
		}

		int maxSpeed = 1;

		float speed = sqrt(dx * dx + dy * dy);

		if (speed > maxSpeed) {
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

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
	srand(time(nullptr));
	sf::RenderWindow app(sf::VideoMode(W, H), "Asteroids!");
	app.setFramerateLimit(60);

	sf::Texture t1, t2, t3, t4, t5, t6, t7, t8;

	int amountBullets = 5;

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
	t8.setSmooth(true);

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
	Animation sEnemy(t8, 0, 0, 154, 224, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

	std::list<Entity*> entities;

	for (int i = 1; i < 1; i++) {
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

	Player *player = new Player();
	player->settings(sPlayer, 200, 200, 0, 20);
	entities.push_back(player);

	/////main loop/////
	while (app.isOpen()) {
		sf::Event event;
		while (app.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				app.close();

			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Space) {
					for (unsigned int i = 0; i < 2; i++) {
						Entity *bul = new bullet();
						int X = 0;
						int Y = 0;
						switch (i) {
						case 0:
							X = player->x + 15 * cos((player->angle - 90) * DEGTORAD);
							Y = player->y + 15 * sin((player->angle - 90) * DEGTORAD);
							break;
						case 1:
							X = player->x + 15 * cos((player->angle + 90) * DEGTORAD);
							Y = player->y + 15 * sin((player->angle + 90) * DEGTORAD);
							break;
						}
						bul->settings(sBullet, X, Y, player->angle, 10);
						entities.push_back(bul);
					}
				}

			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::S) {

					for (unsigned int i = 0; i < amountBullets; i++) {
						Entity *rBul = new rotateBullet();
						int X = player->x + 30 * cos((player->angle + i * (360 / amountBullets)) * DEGTORAD);
						int Y = player->y + 30 * sin((player->angle + i * (360 / amountBullets)) * DEGTORAD);
						rBul->settings(sBullet, X, Y, (player->angle + 90 + i * (360 / amountBullets)), 10);
						entities.push_back(rBul);
					}
				}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			player->angle += 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player->angle -= 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			player->thrust = true;
		else
			player->thrust = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			player->decelerate = true;

		for (auto a : entities)
			for (auto b : entities) {
				if ((a->name == "asteroidBig" && b->name == "bullet") || (a->name == "asteroidSmall" && b->name == "bullet") ||
					(a->name == "asteroidBig" && b->name == "rotateBullet") || (a->name == "asteroidSmall" && b->name == "rotateBullet")) {
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

						player->settings(sPlayer, W / 2, H / 2, 0, 20);
						player->dx = 0;
						player->dy = 0;
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

				if ((a->name == "enemy" && b->name == "bullet") || (a->name == "enemy" && b->name == "rotateBullet")) {
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

						player->settings(sPlayer, W / 2, H / 2, 0, 20);
						player->dx = 0;
						player->dy = 0;
					}
				}

			}

		if (player->thrust)
			player->anim = sPlayer_go;
		else
			player->anim = sPlayer;

		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd())
					e->life = 0;

		if (rand() % 150 == 0 && Entity::countEntity > 1) {
			Entity *a = new asteroidBig();
			a->settings(sRock, 0, rand() % H, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();) {
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
			Entity *enemy = new Enemy(player);
			enemy->settings(sEnemy, 1000, 400, 0, 20);
			enemy->hitPoints = 10;
			entities.push_back(enemy);
			boss = 1;
		}

		for (auto a : entities) {
			if (boss && a->name == "enemy") {
				a->move = true;
			}
		}

		//////draw//////
		app.draw(background);

		for (auto i : entities)
			i->draw(app);

		app.display();
	}

	return 0;
}
