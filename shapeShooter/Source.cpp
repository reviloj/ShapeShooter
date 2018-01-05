#include <SFML/Graphics.hpp>
#include <windows.h>
#include <iostream>
#include <vector>
#include <time.h>   
#include <cmath>
#include <Mmsystem.h> //Audio
#pragma comment(lib, "winmm.lib") //Audio
#define PI 3.14159265;

//Classes
class Game{
	bool mouseRelease = FALSE;
	bool mousePress = FALSE;
	bool keyRelease = FALSE;
	bool keyPress;
	int level = 0;
	std::vector <std::vector<int>> enemies;
protected:
	bool pause = FALSE;
public:
	Game(){
		enemies = { { 5, 0, 0, 0, 0 }, { 15, 0, 0, 0, 0 }, { 15, 10, 0, 0, 0 }, { 10, 0, 6, 0, 0 }, { 0, 10, 10, 0, 0 }, { 5, 0, 0, 4, 0 }, { 0, 10, 0, 8, 0 }, { 0, 0, 0, 15, 0 }, { 0, 0, 0, 0, 15 }, { 8, 0, 0, 8, 8 }, { 10, 10, 10, 10, 10 } };
	}
	bool pauseGame(int buttonX, int buttonY, int buttonSizeX, int buttonSizeY){
		if (mouseReleased() == TRUE && getMousePos().x > buttonX && getMousePos().x < buttonX + buttonSizeX && getMousePos().y > buttonY && getMousePos().y < buttonY + buttonSizeY)
			pause = !pause;
		if (keyReleased(sf::Keyboard::P))
			pause = !pause;
		return pause;
	}
	bool getPause(){
		return pause;
	}
	sf::Vector2i getMousePos(){
		return sf::Mouse::getPosition();
	}
	bool mouseReleased(){
		mouseRelease = FALSE;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			mousePress = TRUE;
		if (mousePress == TRUE && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			mouseRelease = TRUE;
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
			mousePress = FALSE;
		return mouseRelease;
	}
	bool mousePressed(){
		mousePress = FALSE;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			mousePress = TRUE;
		return mousePress;
	}
	bool keyReleased(sf::Keyboard::Key letter){
		keyRelease = FALSE;
		if (sf::Keyboard::isKeyPressed(letter))
			keyPress = TRUE;
		if (keyPress == TRUE && !sf::Keyboard::isKeyPressed(letter))
			keyRelease = TRUE;
		if (!sf::Keyboard::isKeyPressed(letter))
			keyPress = FALSE;
		return keyRelease;
	}
	bool keyPressed(sf::Keyboard::Key letter){
		keyPress = FALSE;
		if (sf::Keyboard::isKeyPressed(letter))
			keyPress = TRUE;
		return keyPress;
	}
	int levelUp(){
		return level += 1;
	}
	int getLevel(){
		return level;
	}
	std::vector<int> getEnemies(){
		return enemies[level];
	}
};
//Generic time handling class
sf::Clock myClock;
class Clock{
	int miliseconds = 0;
	int lastTimeC = 0;
public:
	int updateClock(int asMiliseconds){
		return asMiliseconds - lastTimeC;
	}
	int reset(int asMiliseconds){
		lastTimeC = asMiliseconds;
		return 0;
	}
};
//Frame per seconds regulating class
class FPS : public Clock{
	float currentTime;
	float fps;
	float timePerFrame = 0.02f;
	float delay;
	float lastTime;
public:
	int lastDraw = 0;
	float timePerFrameCalc(int asMiliseconds){
		fps = 1.f / (((float)updateClock(asMiliseconds) / 1000));
		lastTime = (float)updateClock(asMiliseconds) / 1000;
		delay = timePerFrame - lastTime;
		return 0;
	}
	float getDelay(){
		return delay;
	}
	float getfps(){
		return fps;
	}
	float getTimePerFrame(){
		return lastTime;
	}
};
//Physic constants
class Physics{
protected:
	const float MAXVELOCITY = 10;
	const float ACCELERATION = 0.4f;
	const float DEACCELERATION = ACCELERATION / 2.f;
	const float SHOOTINGCOOLDOWN = 0.8f;
};
//Generic shape class
class Shapes{
protected:
	int sides;
	float height;
	float width;
	int sideLength;
	int direction;
	bool alive = TRUE;
	sf::Vector2f pos;
	int randomSpawn(float windowX, float windowY){
		switch (rand() % 4){
		case 0:
			pos.x = (float)(rand() % (int)(windowX - width));
			pos.y = 0.f;
			break;
		case 1:
			pos.y = (float)(rand() % (int)(windowY - height));
			pos.x = (float)(windowX - width);
			break;
		case 2:
			pos.x = (float)(rand() % (int)(windowX - width));
			pos.y = (float)(windowY - height);
			break;
		case 3:
			pos.y = (float)(rand() % (int)(windowY - height));
			pos.x = 0.f;
			break;
		}
		return TRUE;
	}
public:
	int lives;
	int setShapes(float width, float height, float posX, float posY){
		this->width = width;
		this->height = height;
		this->pos.x = posX;
		this->pos.y = posY;
		return 0;
	}
	int heal(){
		return lives += 1;
	}
	bool lifeChange(){
		return alive = !alive;
	}
	bool getAlive(){
		return alive;
	}
	int getLives(){
		return lives;
	}
	int liveDrop(){
		return lives -= 1;
	}
	int getSides(){
		return sides;
	}
	float getHeight(){
		return height;
	}
	float getWidth(){
		return width;
	}
	int getDirection(){
		return direction;
	}
	int setDirection(int direction){
		return this->direction = direction;
	}
};
//Movement class
class Movement : public Physics, public Shapes{
protected:
	sf::Vector3f velocity;
	float speedBonus = 1;
public:
	bool collision(Movement shape1, Movement shape2){
		if (shape1.getPosition().x + shape1.getWidth() / 2 >= shape2.getPosition().x - shape2.getWidth() / 2
			&& shape1.getPosition().x - shape1.getWidth() / 2 <= shape2.getPosition().x + shape2.getWidth() / 2
			&& shape1.getPosition().y + shape1.getHeight() / 2 >= shape2.getPosition().y - shape2.getHeight() / 2
			&& shape1.getPosition().y - shape1.getHeight() <= shape2.getPosition().y + shape2.getHeight() / 2)
			return TRUE;
		return FALSE;
	}
	sf::Vector2f newPosition(Game game){
		if (!game.getPause()){
			pos.x = pos.x += velocity.x;
			pos.y = pos.y += velocity.y;
		}
		return pos;
	}
	sf::Vector2f getPosition(){
		return pos;
	}
	float keyAcceleration1P(Game game, Shapes shape, sf::Keyboard::Key key){
		if (!game.getPause()){
			if (key == sf::Keyboard::S){
				if (!game.getPause() && shape.getLives() > 0)
					if (velocity.y + ACCELERATION < MAXVELOCITY)
						velocity.y += ACCELERATION;
			}
			if (key == sf::Keyboard::W){
				if (!game.getPause() && shape.getLives() > 0)
					if (velocity.y - ACCELERATION > -MAXVELOCITY)
						velocity.y -= ACCELERATION;
			}
			if (key == sf::Keyboard::A){
				if (!game.getPause() && shape.getLives() > 0)
					if (velocity.x - ACCELERATION > -MAXVELOCITY)
						velocity.x -= ACCELERATION;
			}
			if (key == sf::Keyboard::D){
				if (!game.getPause() && shape.getLives() > 0)
					if (velocity.x + ACCELERATION < MAXVELOCITY)
						velocity.x += ACCELERATION;
			}
		}
		return 0;
	}
	float deaccelerate1PX(Game game){
		if (game.getPause() == FALSE)
			if (velocity.x > ACCELERATION || velocity.x < -ACCELERATION)
				velocity.x -= DEACCELERATION * (velocity.x / abs(velocity.x));
			else
				if (velocity.x < ACCELERATION && velocity.x > -ACCELERATION)
					velocity.x = 0;
		return velocity.x;
	}
	float deaccelerate1PY(Game game){
		if (game.getPause() == FALSE)
			if (velocity.y > ACCELERATION || velocity.y < -ACCELERATION)
				velocity.y -= DEACCELERATION * (velocity.y / abs(velocity.y));
			else
				if (velocity.y < ACCELERATION && velocity.y > -ACCELERATION)
					velocity.y = 0;
		return velocity.y;
	}
	int bounce(Movement shape, Movement wallT, Movement wallR, Movement wallB, Movement wallL){
		int posDirection = direction % 360;
		if (direction < 0)
			posDirection = 360 - abs(direction) % 360;
		if (collision(shape, wallR)){
			if (posDirection < 90){
				direction = 360 - direction;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
			if (posDirection < 180){
				direction = 270 - (direction - 90);
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
		}
		if (collision(shape, wallL)){
			if (posDirection < 270 && posDirection > 180){
				direction = 270 - direction + 90;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
			if (posDirection < 360 && posDirection > 180){
				direction = 360 - direction;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
		}
		if (collision(shape, wallT)){
			if (posDirection < 90){
				direction = 180 - direction;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
			if (posDirection > 270){
				direction = 360 - direction + 180;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
		}
		if (collision(shape, wallB)){
			if (posDirection < 180 && posDirection > 90){
				direction = 180 - direction;
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
			if (posDirection < 270 && posDirection > 90){
				direction = 360 - (direction - 180);
				velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
				return direction;
			}
		}
		return 0;	
	}
	bool setShipDirectionVelocity(Movement ship){
		direction = (int)(atan(abs(ship.getPosition().y - pos.y) / abs(ship.getPosition().x - pos.x)) / 3.14 * 180);
		if (ship.getPosition().x >= pos.x && ship.getPosition().y > pos.y)
			direction;
		if (ship.getPosition().x < pos.x && ship.getPosition().y >= pos.y)
			direction = 180 - direction;
		if (ship.getPosition().x <= pos.x && ship.getPosition().y < pos.y)
			direction += 180;
		if (ship.getPosition().x > pos.x && ship.getPosition().y <= pos.y)
			direction = 360 - direction;
		velocity.x = (float)(cos((float)(direction) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(direction) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		return TRUE;
	}
	float getMouseDirection(Game game){
		direction = (int)(atan(abs(game.getMousePos().y - pos.y) / abs(game.getMousePos().x - pos.x)) / 3.14 * 180);
		if (game.getMousePos().x >= pos.x && game.getMousePos().y > pos.y)
			direction += 90;
		if (game.getMousePos().x < pos.x && game.getMousePos().y >= pos.y)
			direction = 180 - direction + 90;
		if (game.getMousePos().x <= pos.x && game.getMousePos().y < pos.y)
			direction += 270;
		if (game.getMousePos().x > pos.x && game.getMousePos().y <= pos.y)
			direction = 360 - direction + 90;
		return direction;
	}
};
//User controlled ship class
class Ship : public Clock, public Movement{
	friend class Shot;
	float shotCooldown;
	float respawnCooldown = 0;
public:
	Ship(float x, float y, int s, float w, float h, int lives, Game game){
		pos.x = x;
		pos.y = y;
		velocity.x = 0;
		velocity.y = 0;
		sides = s;
		width = w;
		height = h;
		shotCooldown = 200.f;
		this->lives = lives;
		direction = 0;
	}
	int shotFired(int asMiliseconds){
		reset(asMiliseconds);
		return 0;
	}
	bool rapidFire(bool on_off){
		if (on_off)
			shotCooldown = 100.f;
		else
			shotCooldown = 200.f;
		return 0;
	}
	float shotCooldownCount(int asMiliseconds){
		if (shotCooldown <= updateClock(asMiliseconds))
			return shotCooldown - (float)updateClock(asMiliseconds);
		return 0;
	}
	bool respawn(int asMiliseconds, float windowX, float windowY){
		if (!getAlive() && lives > 0 && respawnCooldown <= 0){
			lives -= 1;
			respawnCooldown = 3000;
			reset(asMiliseconds);
			pos.x = windowX / 2;
			pos.y = windowY / 2;
			velocity.x = 0;
			velocity.y = 0;
		}
		if (respawnCooldown > 0){
			respawnCooldown -= updateClock(asMiliseconds);
			reset(asMiliseconds);
			if (respawnCooldown <= 0 && lives > 0){
				lifeChange();
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}
	sf::Vector2f enter(sf::Vector2f window){
		pos.x = window.x / 2;
		pos.y = window.y / 2;
		return pos;
	}
};
//Shots class
class Shot : public Movement{
public:
	float bounceSpeedSlow(){
		speedBonus = 0.4;
		velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		return speedBonus;
		
	}
	Shot(Ship ship){
		speedBonus = 1.3;
		velocity.x = (float)(cos((float)(ship.direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(ship.direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		pos.x = ship.pos.x + velocity.x * 2;
		pos.y = ship.pos.y + velocity.y * 2;
		width = 2;
		height = 10;
		direction = ship.direction;
	}
	Shot(Ship ship, int change, int onPowerUp){
		speedBonus = 1.2;
		velocity.x = (float)(cos((float)(ship.direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(ship.direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		pos.x = ship.pos.x + velocity.x * 2;
		pos.y = ship.pos.y + velocity.y * 2;
		width = 2;
		height = 10;
		direction = ship.direction;
		switch (onPowerUp){
		case 1:
			direction += change;
			velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
			velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
			break;
		}
	}
	Shot &operator = (const Shot& other){
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		direction = other.direction;
		return *this;
	}
};
//Enemey classes
class Square : public Movement{
public:
	Square(float windowX, float windowY){
		sides = 4;
		width = 20;
		height = 20;
		randomSpawn(windowX, windowY);
		direction = rand() % 360;
		while (direction % 90 == 0)
			direction = rand() % 360;
		speedBonus = 0.5;
		velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
	}
	Square &operator = (const Square& other){
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		sides = other.sides;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class Pentagon : public Movement{
public:
	Pentagon(float windowX, float windowY, Ship ship){
		sides = 5;
		width = 20;
		height = 20;
		randomSpawn(windowX, windowY);
		speedBonus = 0.33;
		setShipDirectionVelocity(ship);
	}
	Pentagon &operator = (const Pentagon& other){
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		sides = other.sides;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class Circle : public Movement{
public:
	Circle(float windowX, float windowY, Ship ship){
		lives = 3;
		width = 16;
		height = 16;
		randomSpawn(windowX, windowY);
		speedBonus = 0.4;
		setShipDirectionVelocity(ship);
	}
	Circle &operator = (const Circle& other){
		lives = other.lives;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class Hexagon :public Movement{
public:
	Hexagon(float windowX, float windowY, Ship ship){
		lives = 4;
		width = 24;
		height = 24;
		randomSpawn(windowX, windowY);
		direction = rand() % 360;
		while (direction % 90 == 0)
			direction = rand() % 360;
		speedBonus = 0.4;
		velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
	}
	Hexagon &operator = (const Hexagon& other){
		lives = other.lives;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class BabyHexagon :public Movement{
public:
	BabyHexagon(float windowX, float windowY, float x, float y, Ship ship){
		lives = 2;
		width = 17;
		height = 17;
		pos.x = x;
		pos.y = y;
		speedBonus = 0.4;
		setShipDirectionVelocity(ship);
	}
	BabyHexagon &operator = (const BabyHexagon& other){
		lives = other.lives;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class Elipse :public Movement, public Clock{
private:
	int directionChangeTime = 1500;
public:
	bool directionChangeTimerReset(int currentTimeM){
		return reset(currentTimeM);
	}
	int dirrectionChange(int currentTimeM){
		if (directionChangeTime <= updateClock(currentTimeM)){
			direction = rand() % 360;
			velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
			velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
			return direction;
		}
		return 0;
	}
	Elipse(float windowX, float windowY){
		width = 24;
		height = 16;
		lives = 2;
		randomSpawn(windowX, windowY);
		direction = rand() % 360;
		while (direction % 90 == 0)
			direction = rand() % 360;
		speedBonus = 1.0;
		velocity.x = (float)(cos((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
		velocity.y = (float)(sin((float)(direction - 90) / 180 * 3.14) * (MAXVELOCITY * speedBonus));
	}
	Elipse &operator = (const Elipse& other){
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		direction = other.direction;
		speedBonus = other.speedBonus;
		velocity.x = other.velocity.x;
		velocity.y = other.velocity.y;
		width = other.width;
		height = other.height;
		return *this;
	}
};
class PowerUps : public Clock, public Movement{
	int powerUpCooldown = 15000;
	bool powerUpOn = FALSE;
	int powerUp;
	int storedPowerUp = NULL;
	int onPowerUp = NULL;
	int shotCount;
	int bulletShieldCount = 0;
public:
	int shotCountFired(){
		if (shotCount == 0)
			onPowerUp = NULL;
		return shotCount -= 1;
	}
	bool extraLife(Ship ship){
		ship.heal();
		return TRUE;
	}
	bool storePowerUp(){
		storedPowerUp = powerUp;
		return TRUE;
	}
	int getOnPowerUp(){
		return onPowerUp;
	}
	bool useStoredPowerUp(){
		switch (storedPowerUp){
		case 1:
			onPowerUp = storedPowerUp;
			shotCount = 20;
			break;
		case 2:
			onPowerUp = storedPowerUp;
			shotCount = 40;
			break;
		case 3:
			onPowerUp = storedPowerUp;
			bulletShieldCount = 5;
			break;
		}
		storedPowerUp = NULL;
		return 0;
	}
	int getBulletShield(){
		return bulletShieldCount;
	}
	int bulletShieldHit(){
		return bulletShieldCount -= 1;
	}
	int getStoredPowerUp(){
		return storedPowerUp;
	}
	bool appear(int currentTimeM, sf::Vector2u window){
		if (powerUpCooldown <= updateClock(currentTimeM) && powerUpOn == FALSE){
			reset(currentTimeM);
			powerUpOn = TRUE;
			pos.x = rand() % (int)window.x;
			pos.y = rand() % (int)window.y;
			powerUp = rand() % 4;
			return TRUE;
		}
		if (powerUpOn)
			return TRUE;
		return FALSE;
	}
	int getPowerUp(){
		return powerUp;
	}
	bool used(int currentTimeM, Ship ship, Movement powerUpBox){
		if (collision(ship, powerUpBox) && powerUpOn == TRUE){
			reset(currentTimeM);
			powerUpOn = FALSE;
			return TRUE;
		}
		return FALSE;
	}
};

int main()
{
	std::cout << "INSTRUCTIONS:" << std::endl <<
		"You are the triangle at the centre of the screen. Use the WASD keys to move" << std::endl <<
		"around and the mouse to turn and shoot. Each level there will be a number of" << std::endl <<
		"shapes that will move around the screen, destroy them all to move on. A variety" << std::endl <<
		"of shapes with different actions will destroy you upon hitting you. Beware! Your" <<
		"own shots will bounce off the walls and destroy you as well if you hit them. " << std::endl <<
		"Going off the edge of the screen will also result in destruction. Picking up" << std::endl <<
		"power ups will store them in the top right area of the screen if they aren't" << std::endl <<
		"instant use, press the right mouse button to activate them. Press 'P' to pause" << std::endl <<
		"the game." << std::endl << std::endl <<
		"Press '1' then the enter key to start" << std::endl;
	float x = 0;
	while (x != 1)
		std::cin >> x;
	
	PlaySoundA("Oliver Game (Prod. Rees).wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	
	// Display the list of all the video modes available for fullscreen
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	for (std::size_t i = 0; i < modes.size(); ++i)
	{
		sf::VideoMode mode = modes[i];
		//std::cout << "Mode #" << i << ": " << mode.width << "x" << mode.height <5< " - "
			//<< mode.bitsPerPixel << " bpp" << std::endl;
	}
	// Create a window with fullscreen dimensions
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height, desktop.bitsPerPixel), "SFML window", sf::Style::Fullscreen);

	srand(time(NULL));

	//Objects
	Game game;
	sf::RectangleShape pauseButtonS(sf::Vector2f(60, 30));
	pauseButtonS.setFillColor(sf::Color(100, 100, 100));
	pauseButtonS.setPosition(sf::Vector2f(window.getSize().x - pauseButtonS.getSize().x - 10, 10));

	Ship ship((float)window.getSize().x / 2, (float)window.getSize().y / 2, 3, 14, 21, 6, game);
	sf::ConvexShape shipS;
	shipS.setPointCount(3);
	shipS.setPoint(0, sf::Vector2f(ship.getWidth() / 2, 0.f));
	shipS.setPoint(1, sf::Vector2f(ship.getWidth(), ship.getHeight()));
	shipS.setPoint(2, sf::Vector2f(0.f, ship.getHeight()));
	shipS.setOrigin(sf::Vector2f(ship.getWidth() / 2, ship.getHeight() / 2));
	shipS.setPosition(sf::Vector2f(ship.getPosition().x, ship.getPosition().y));

	//Boundaries
	Movement wallL;
	wallL.setShapes(20.f, (float)window.getSize().y, -10.f, (float)window.getSize().y / 2);
	Movement wallR;
	wallR.setShapes(20.f, (float)window.getSize().y, (float)window.getSize().x + 10, (float)window.getSize().y / 2);
	Movement wallT;
	wallT.setShapes((float)window.getSize().x, 20.f, (float)window.getSize().x / 2, -10.f);
	Movement wallB;
	wallB.setShapes((float)window.getSize().x, 20.f, (float)window.getSize().x / 2, (float)window.getSize().y + 10);

	std::vector<Shot> shotV;
	sf::RectangleShape shotS(sf::Vector2f(0, 0));
	shotS.setFillColor(sf::Color::Black);
	shotS.setOrigin(sf::Vector2f(1, 5));

	sf::CircleShape livesS(8);
	livesS.setFillColor(sf::Color(50, 50, 50));

	FPS fps;

	PowerUps powerUpBox;
	powerUpBox.setShapes(24, 24, NULL, NULL);
	sf::RectangleShape powerUpBoxS;
	powerUpBoxS.setSize(sf::Vector2f(powerUpBox.getWidth(), powerUpBox.getHeight()));
	powerUpBoxS.setFillColor(sf::Color(0, 0, 0));
	powerUpBoxS.setOutlineThickness(-2);
	powerUpBoxS.setOutlineColor(sf::Color(255, 255, 255));
	powerUpBoxS.setOrigin(sf::Vector2f(12, 12));

	sf::ConvexShape extraLifeHeartLS;
	extraLifeHeartLS.setPointCount(20);
	extraLifeHeartLS.setPoint(0, sf::Vector2f(0, 0));
	for (int i = 1; i < 12; i++){
		extraLifeHeartLS.setPoint(i, sf::Vector2f(0 - (1 - cos((float)(i - 1) * (3.14 / 8))) * 4, -14 - sin((float)(i - 1) * (3.14 / 8)) * 4));
	}
	extraLifeHeartLS.setFillColor(sf::Color(255, 0, 0));
	sf::ConvexShape extraLifeHeartRS;
	extraLifeHeartRS.setPointCount(20);
	extraLifeHeartRS.setPoint(0, sf::Vector2f(0, 0));
	for (int i = 1; i < 12; i++){
		extraLifeHeartRS.setPoint(i, sf::Vector2f(0 - (1 - cos((float)(i - 1) * (3.14 / 8))) * 4, -14 - sin((float)(i - 1) * (3.14 / 8)) * 4));
	}
	extraLifeHeartRS.setScale(-1, 1);
	extraLifeHeartRS.setFillColor(sf::Color(255, 0, 0));

	sf::RectangleShape trippleShot1S(sf::Vector2f(2, 14));
	trippleShot1S.setOrigin(1, 16);
	trippleShot1S.setFillColor(sf::Color(255, 255, 255));
	trippleShot1S.setRotation(345);
	sf::RectangleShape trippleShot2S(sf::Vector2f(2, 14));
	trippleShot2S.setOrigin(1, 16);
	trippleShot2S.setFillColor(sf::Color(255, 255, 255));
	trippleShot2S.setRotation(0);
	sf::RectangleShape trippleShot3S(sf::Vector2f(2, 14));
	trippleShot3S.setOrigin(1, 16);
	trippleShot3S.setFillColor(sf::Color(255, 255, 255));
	trippleShot3S.setRotation(15);

	sf::RectangleShape rapidShot1S(sf::Vector2f(1, 6));
	rapidShot1S.setFillColor(sf::Color(255, 255, 255));
	rapidShot1S.setOrigin(0, 6);
	rapidShot1S.setRotation(45);
	sf::RectangleShape rapidShot2S(sf::Vector2f(1, 6));
	rapidShot2S.setFillColor(sf::Color(255, 255, 255));
	rapidShot2S.setOrigin(0, 6);
	rapidShot2S.setRotation(45);
	sf::RectangleShape rapidShot3S(sf::Vector2f(1, 6));
	rapidShot3S.setFillColor(sf::Color(255, 255, 255));
	rapidShot3S.setOrigin(0, 6);
	rapidShot3S.setRotation(45);

	sf::CircleShape bulletShieldS(7);
	bulletShieldS.setFillColor(sf::Color::Transparent);
	bulletShieldS.setOutlineThickness(-2);
	bulletShieldS.setOrigin(7, 7);
	bulletShieldS.setOutlineColor(sf::Color(153, 255, 255));

	std::vector<Square> squareV;
	squareV.push_back(Square((float)window.getSize().x, (float)window.getSize().y));
	sf::RectangleShape squareS;
	squareS.setSize(sf::Vector2f(squareV[0].getWidth(), squareV[0].getHeight()));
	squareS.setFillColor(sf::Color(50, 155, 50));
	squareS.setOrigin(sf::Vector2f(squareS.getSize().x / 2, squareS.getSize().y / 2));
	squareV.clear();

	std::vector<Pentagon> pentagonV;
	pentagonV.push_back(Pentagon((float)window.getSize().x, (float)window.getSize().y, ship));
	sf::ConvexShape pentagonS(5);
	pentagonS.setPoint(0, sf::Vector2f(0, 0));
	pentagonS.setPoint(1, sf::Vector2f(10, 7));
	pentagonS.setPoint(2, sf::Vector2f(6, 18));
	pentagonS.setPoint(3, sf::Vector2f(-6, 18));
	pentagonS.setPoint(4, sf::Vector2f(-10, 7));
	pentagonS.setFillColor(sf::Color(51, 51, 255));
	pentagonS.setOrigin(sf::Vector2f(pentagonV[0].getHeight(), pentagonV[0].getWidth()));
	pentagonV.clear();

	std::vector<Circle> circleV;
	circleV.push_back(Circle((float)window.getSize().x, (float)window.getSize().y, ship));
	sf::CircleShape circleS(circleV[0].getHeight() / 2);
	circleS.setFillColor(sf::Color(255, 0, 0));
	circleS.setOrigin(sf::Vector2f(circleV[0].getHeight() / 2, circleV[0].getWidth() / 2));
	circleV.clear();

	std::vector<Hexagon> hexagonV;
	hexagonV.push_back(Hexagon((float)window.getSize().x, (float)window.getSize().y, ship));
	sf::ConvexShape hexagonS(8);
	hexagonS.setPoint(0, sf::Vector2f(0, 0));
	hexagonS.setPoint(1, sf::Vector2f(8, 0));
	hexagonS.setPoint(2, sf::Vector2f(14, 6));
	hexagonS.setPoint(3, sf::Vector2f(14, 14));
	hexagonS.setPoint(4, sf::Vector2f(8, 20));
	hexagonS.setPoint(5, sf::Vector2f(0, 20));
	hexagonS.setPoint(6, sf::Vector2f(-6, 14));
	hexagonS.setPoint(7, sf::Vector2f(-6, 6));
	hexagonS.setFillColor(sf::Color(230, 75, 20));
	hexagonS.setFillColor(sf::Color(0, 0, 0));
	hexagonS.setOrigin(sf::Vector2f(hexagonV[0].getHeight() / 2, hexagonV[0].getWidth() / 2));
	hexagonV.clear();

	std::vector<BabyHexagon> babyHexagonV;
	babyHexagonV.push_back(BabyHexagon((float)window.getSize().x, (float)window.getSize().y, 0, 0, ship));
	sf::ConvexShape babyHexagonS(8);
	babyHexagonS.setOutlineThickness(-1);
	babyHexagonS.setOutlineColor(sf::Color(255, 255, 255));
	babyHexagonS.setPoint(0, sf::Vector2f(0, 0));
	babyHexagonS.setPoint(1, sf::Vector2f(7, 0));
	babyHexagonS.setPoint(2, sf::Vector2f(12, 5));
	babyHexagonS.setPoint(3, sf::Vector2f(12, 12));
	babyHexagonS.setPoint(4, sf::Vector2f(7, 17));
	babyHexagonS.setPoint(5, sf::Vector2f(0, 17));
	babyHexagonS.setPoint(6, sf::Vector2f(-5, 12));
	babyHexagonS.setPoint(7, sf::Vector2f(-5, 7));
	babyHexagonS.setFillColor(sf::Color(0, 0, 0));
	babyHexagonS.setOrigin(sf::Vector2f(babyHexagonV[0].getHeight() / 2, babyHexagonV[0].getWidth() / 2));
	babyHexagonV.clear();

	//Ellipse
	std::vector<Elipse> elipseV;
	elipseV.push_back(Elipse((float)window.getSize().x, (float)window.getSize().y));
	sf::CircleShape elipseS(elipseV[0].getHeight() / 2);
	elipseS.scale(1.5, 1);
	elipseS.setFillColor(sf::Color(160, 32, 240));
	elipseS.setOrigin(sf::Vector2f(elipseV[0].getHeight() / 2, elipseV[0].getWidth() / 2));
	elipseV.clear();

	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			switch (event.type){
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		//FPS
		fps.reset(myClock.getElapsedTime().asMilliseconds());

		//Draw
		window.clear(sf::Color(0, 255, 255));
		window.draw(pauseButtonS);

		//Pause
		game.pauseGame((int)pauseButtonS.getPosition().x, (int)pauseButtonS.getPosition().y, (int)pauseButtonS.getSize().x, (int)pauseButtonS.getSize().y);

		//Game start
		if (squareV.size() + pentagonV.size() + circleV.size() + hexagonV.size() + babyHexagonV.size() + elipseV.size() == 0){
			if (game.getLevel() < 11){
				shotV.clear();
				for (int i = 0; i < game.getEnemies().at(0); i++)
					squareV.push_back(Square((float)window.getSize().x, (float)window.getSize().y));
				for (int i = 0; i < game.getEnemies().at(1); i++)
					pentagonV.push_back(Pentagon((float)window.getSize().x, (float)window.getSize().y, ship));
				for (int i = 0; i < game.getEnemies().at(2); i++)
					circleV.push_back(Circle((float)window.getSize().x, (float)window.getSize().y, ship));
				for (int i = 0; i < game.getEnemies().at(3); i++)
					hexagonV.push_back(Hexagon((float)window.getSize().x, (float)window.getSize().y, ship));
				for (int i = 0; i < game.getEnemies().at(4); i++)
					elipseV.push_back(Elipse((float)window.getSize().x, (float)window.getSize().y));
				game.levelUp();
			}
			else{
				window.close();
				std::cout << std::endl << std::endl <<
					"CONGRATULATIONS YOU HAVE COMPLETED THE GAME" << std::endl <<
					"CREDITS:" << std::endl <<
					"Oliver Jang ~ Game Producer" << std::endl <<
					"Reese Morgan ~ Music Producer" << std::endl <<
					"Darren Hong, Carson Hoang ~ Quality Assurance Testers" << std::endl;
			}
		}

		//Ship Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			ship.keyAcceleration1P(game, ship, sf::Keyboard::W);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			ship.keyAcceleration1P(game, ship, sf::Keyboard::S);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			ship.keyAcceleration1P(game, ship, sf::Keyboard::A);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				ship.keyAcceleration1P(game, ship, sf::Keyboard::D);
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S) || !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			ship.deaccelerate1PX(game);
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) || !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			ship.deaccelerate1PY(game);
		shipS.setPosition(ship.newPosition(game));
		shipS.setRotation(ship.getMouseDirection(game));

		//Ship lives
		if (ship.getLives() > 0)
			window.draw(shipS);
		if (ship.getLives() == 0){
			shotV.empty();
			window.close();
			Sleep(1000);
			std::cout << std::endl << std::endl <<
				"Game Over. You have lost at level " << game.getLevel() << ". Try again for a better score!" <<std::endl;
		}

		if (ship.respawn(myClock.getElapsedTime().asMilliseconds(), (float)window.getSize().x, (float)window.getSize().y) != FALSE)
			shipS.setFillColor(sf::Color(50, 50, 50));
		else{
			if (ship.getAlive() && game.getPause() == FALSE){
				if (ship.getPosition().x < 0 || ship.getPosition().x > wallR.getPosition().x || ship.getPosition().y < 0 || ship.getPosition().y > wallB.getPosition().y){
					ship.lifeChange();
					std::cout << "Wall death" << std::endl;
				}
				for (unsigned int i = 0; i < squareV.size(); i++)
					if (ship.collision(ship, squareV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Square death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							squareV.erase(squareV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < pentagonV.size(); i++)
					if (ship.collision(ship, pentagonV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Pentagon death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							pentagonV.erase(pentagonV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < shotV.size(); i++)
					if (ship.collision(ship, shotV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Shot death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							shotV.erase(shotV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < circleV.size(); i++)
					if (ship.collision(ship, circleV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Circle death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							circleV.erase(circleV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < hexagonV.size(); i++)
					if (ship.collision(ship, hexagonV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Hexagon death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							hexagonV.erase(hexagonV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < babyHexagonV.size(); i++)
					if (ship.collision(ship, babyHexagonV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Baby Hexagon death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							babyHexagonV.erase(babyHexagonV.begin() + i);
						}
					}
				for (unsigned int i = 0; i < elipseV.size(); i++)
					if (ship.collision(ship, elipseV[i])){
						if (powerUpBox.getBulletShield() == 0){
							ship.lifeChange();
							std::cout << "Elipse death" << std::endl;
						}
						else{
							powerUpBox.bulletShieldHit();
							elipseV.erase(elipseV.begin() + i);
						}
					}
				shipS.setFillColor(sf::Color(255, 255, 255));
			}
		}

		//Shots
		if (ship.shotCooldownCount(myClock.getElapsedTime().asMilliseconds()) < 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left) && ship.getAlive() && game.getPause() == FALSE){
			shotV.push_back(Shot(ship));
			ship.shotFired(myClock.getElapsedTime().asMilliseconds());
			if (powerUpBox.shotCountFired() >= 0)
				switch (powerUpBox.getOnPowerUp()){
				case 1:
					shotV.push_back(Shot(ship, 10, powerUpBox.getOnPowerUp()));
					shotV.push_back(Shot(ship, -10, powerUpBox.getOnPowerUp()));
					break;
				case 2:
					ship.rapidFire(TRUE);
					break;
			}
			else
				ship.rapidFire(FALSE);
		}
		
		for (unsigned int i = 0; i < shotV.size(); i++){
			shotS.setSize(sf::Vector2f(shotV[i].getWidth(), shotV[i].getHeight()));
			shotS.setPosition(shotV[i].newPosition(game));
			shotS.setRotation((float)shotV[i].getDirection());
			window.draw(shotS);
			if (shotV[i].bounce(shotV[i], wallT, wallR, wallB, wallL) != 0)
				shotV[i].bounceSpeedSlow();
		}

		//Squares
		for (unsigned int i = 0; i < squareV.size(); i++){
			squareS.setPosition(squareV[i].newPosition(game));
			squareV[i].bounce(squareV[i], wallT, wallR, wallB, wallL);
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (squareV.size() > i){
					if (squareV[i].collision(squareV[i], shotV[x])){
						shotV.erase(shotV.begin() + x);
						squareV.erase(squareV.begin() + i);
						x -= 1;
						i -= 1;
					}
				}
			}
			window.draw(squareS);
		}

		//Pentagons
		for (unsigned int i = 0; i < pentagonV.size(); i++){
			pentagonV[i].setShipDirectionVelocity(ship);
			if (ship.getAlive())
				pentagonV[i].newPosition(game);
			pentagonS.setPosition(sf::Vector2f(pentagonV[i].getPosition().x, pentagonV[i].getPosition().y));
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (pentagonV.size() > i){
					if (pentagonV[i].collision(pentagonV[i], shotV[x])){
						shotV.erase(shotV.begin() + x);
						pentagonV.erase(pentagonV.begin() + i);
						x -= 1;
						i -= 1;
					}
				}
			}
			window.draw(pentagonS);
		}

		//Circles
		for (unsigned int i = 0; i < circleV.size(); i++){
			circleV[i].setShipDirectionVelocity(ship);
			if (ship.getAlive())
				circleV[i].newPosition(game);
			circleS.setPosition(sf::Vector2f(circleV[i].getPosition().x, circleV[i].getPosition().y));
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (circleV.size() > i){
					if (circleV[i].collision(circleV[i], shotV[x]) && circleV[i].getLives() == 0){
						shotV.erase(shotV.begin() + x);
						circleV.erase(circleV.begin() + i);
						x -= 1;
						i -= 1;
					}
					else
						if (circleV[i].collision(circleV[i], shotV[x]) && circleV[i].getLives() > 0)
							circleV[i].liveDrop();
				}
			}
			window.draw(circleS);
		}

		//Hexagons
		for (unsigned int i = 0; i < hexagonV.size(); i++){
			if (hexagonV[i].getPosition().x < 0 || hexagonV[i].getPosition().x > window.getPosition().x)
				
			if (hexagonV[i].getPosition().y < 0 || hexagonV[i].getPosition().y > window.getPosition().y)
				hexagonV[i].bounce(hexagonV[i], wallT, wallR, wallB, wallL);
			hexagonS.setPosition(hexagonV[i].newPosition(game));
			hexagonV[i].bounce(hexagonV[i], wallT, wallR, wallB, wallL);
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (hexagonV.size() > i){
					if (hexagonV[i].collision(hexagonV[i], shotV[x]) && hexagonV[i].getLives() == 0){
						for (int z = 0; z < 5; z++)
							babyHexagonV.push_back(BabyHexagon((float)window.getSize().x, (float)window.getSize().y, hexagonV[i].getPosition().x + rand() % 80 - 40, hexagonV[i].getPosition().y + rand() % 80 - 40, ship));
						shotV.erase(shotV.begin() + x);
						hexagonV.erase(hexagonV.begin() + i);
						x -= 1;
						i -= 1;
					}
					else
						if (hexagonV[i].collision(hexagonV[i], shotV[x]) && hexagonV[i].getLives() > 0){
							hexagonV[i].liveDrop();
							shotV.erase(shotV.begin() + x);
						}
				}
			}
			window.draw(hexagonS);
		}

		//Baby Hexagons
		for (unsigned int i = 0; i < babyHexagonV.size(); i++){
			babyHexagonV[i].setShipDirectionVelocity(ship);
			if (ship.getAlive())
				babyHexagonV[i].newPosition(game);
			babyHexagonS.setPosition(sf::Vector2f(babyHexagonV[i].getPosition().x, babyHexagonV[i].getPosition().y));
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (babyHexagonV.size() > i){
					if (babyHexagonV[i].collision(babyHexagonV[i], shotV[x]) && babyHexagonV[i].getLives() == 0){
						shotV.erase(shotV.begin() + x);
						babyHexagonV.erase(babyHexagonV.begin() + i);
						x -= 1;
						i -= 1;
					}
					else
						if (babyHexagonV[i].collision(babyHexagonV[i], shotV[x]) && babyHexagonV[i].getLives() > 0){
							babyHexagonV[i].liveDrop();
							shotV.erase(shotV.begin() + x);
						}
				}
			}
			window.draw(babyHexagonS);
		}

		//Ellipse
		for (unsigned int i = 0; i < elipseV.size(); i++){
			elipseS.setPosition(elipseV[i].newPosition(game));
			elipseV[i].bounce(elipseV[i], wallT, wallR, wallB, wallL);
			if (elipseV[i].dirrectionChange(myClock.getElapsedTime().asMilliseconds()) != 0)
				for (int x = 0; x < elipseV.size(); x++){
					elipseV[x].dirrectionChange(myClock.getElapsedTime().asMilliseconds());
					elipseV[x].directionChangeTimerReset(myClock.getElapsedTime().asMilliseconds());
				}
			
			for (unsigned int x = 0; x < shotV.size(); x++){
				if (elipseV.size() > i){
					if (elipseV[i].collision(elipseV[i], shotV[x])){
						shotV.erase(shotV.begin() + x);
						elipseV.erase(elipseV.begin() + i);
						x -= 1;
						i -= 1;
					}
				}
			}
			window.draw(elipseS);
		}

		//Power Ups
		if (powerUpBox.used(myClock.getElapsedTime().asMilliseconds(), ship, powerUpBox)){
			if (powerUpBox.getPowerUp() == 0){
				if (ship.lives < 10){
					powerUpBox.extraLife(ship);
					ship.lives += 1;
				}
			}else{
				powerUpBox.storePowerUp();
			}
		}
		if (powerUpBox.appear(myClock.getElapsedTime().asMilliseconds(),window.getSize())){
			powerUpBoxS.setPosition(sf::Vector2f(powerUpBox.getPosition().x, powerUpBox.getPosition().y));
			window.draw(powerUpBoxS);
			switch (powerUpBox.getPowerUp()){
			case 0:
				extraLifeHeartLS.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y + 10));
				extraLifeHeartRS.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y + 10));
				window.draw(extraLifeHeartLS);
				window.draw(extraLifeHeartRS);
				break;
			case 1:
				trippleShot1S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 3, powerUpBoxS.getPosition().y + 9));
				trippleShot2S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y + 9));
				trippleShot3S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x + 3, powerUpBoxS.getPosition().y + 9));
				window.draw(trippleShot1S);
				window.draw(trippleShot2S);
				window.draw(trippleShot3S);
				break;
			case 2:
				rapidShot1S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 8, powerUpBoxS.getPosition().y + 7));
				rapidShot2S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 2, powerUpBoxS.getPosition().y + 1));
				rapidShot3S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x + 4, powerUpBoxS.getPosition().y - 5));
				window.draw(rapidShot1S);
				window.draw(rapidShot2S);
				window.draw(rapidShot3S);
				break;
			case 3:
				bulletShieldS.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y));
				window.draw(bulletShieldS);
				break;
			}
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			powerUpBox.useStoredPowerUp();

		if (powerUpBox.getBulletShield() > 0){
			bulletShieldS.setRadius(14);
			bulletShieldS.setPosition(sf::Vector2f(shipS.getPosition().x - 7, shipS.getPosition().y - 7));
			window.draw(bulletShieldS);
			bulletShieldS.setRadius(7);
		}

		switch (powerUpBox.getStoredPowerUp()){
		case 1:
			powerUpBoxS.setPosition(sf::Vector2f(pauseButtonS.getPosition().x + pauseButtonS.getSize().x / 2, pauseButtonS.getPosition().y + pauseButtonS.getSize().y / 2));
			window.draw(powerUpBoxS);
			trippleShot1S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 3, powerUpBoxS.getPosition().y + 9));
			trippleShot2S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y + 9));
			trippleShot3S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x + 3, powerUpBoxS.getPosition().y + 9));
			window.draw(trippleShot1S);
			window.draw(trippleShot2S);
			window.draw(trippleShot3S);
			break;
		case 2:
			powerUpBoxS.setPosition(sf::Vector2f(pauseButtonS.getPosition().x + pauseButtonS.getSize().x / 2, pauseButtonS.getPosition().y + pauseButtonS.getSize().y / 2));
			window.draw(powerUpBoxS);
			rapidShot1S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 8, powerUpBoxS.getPosition().y + 7));
			rapidShot2S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x - 2, powerUpBoxS.getPosition().y + 1));
			rapidShot3S.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x + 4, powerUpBoxS.getPosition().y - 5));
			window.draw(rapidShot1S);
			window.draw(rapidShot2S);
			window.draw(rapidShot3S);
			break;
		case 3:
			powerUpBoxS.setPosition(sf::Vector2f(pauseButtonS.getPosition().x + pauseButtonS.getSize().x / 2, pauseButtonS.getPosition().y + pauseButtonS.getSize().y / 2));
			window.draw(powerUpBoxS);
			bulletShieldS.setPosition(sf::Vector2f(powerUpBoxS.getPosition().x, powerUpBoxS.getPosition().y));
			window.draw(bulletShieldS);
			break;
		}

		//Game Stuff
		for (int i = 0; i < ship.getLives(); i++){
			livesS.setPosition(sf::Vector2f(25.f + i * 25, 25.f));
			window.draw(livesS);
		}

		//FPS
		fps.timePerFrameCalc(myClock.getElapsedTime().asMilliseconds());
		if (fps.getDelay() > 0 || fps.lastDraw == 10){
			window.display();
			fps.lastDraw = 0;
		}
		else{
			fps.lastDraw++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			return 0;
		fps.timePerFrameCalc(myClock.getElapsedTime().asMilliseconds());
		if (fps.getDelay() > 0)
			Sleep(/*(DWORD)*/fps.getDelay() * 1000);
		fps.timePerFrameCalc(myClock.getElapsedTime().asMilliseconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tilde)){
			std::cout << "FPS:   " << fps.getfps() << "    Time Per Frame:    " << fps.getTimePerFrame() << std::endl;
			std::cout << fps.getDelay() << std::endl;
		}
	}
	return 0;
}