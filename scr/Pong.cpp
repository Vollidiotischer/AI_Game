#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <time.h>
#include <windows.h>
//#include <chrono>
//#include <thread>

#include "iwd_GLI.h"; 
#include "AI.h"


void drawRect1(const int, const int, const int, const int);
int maxAtIndex(std::vector<double>&); 
int sync(int);

int height = 700; //1250
int width = 1300; //1500
GLFWwindow* window;



bool trainingActive = false; 



struct Player {
	int x, y;
	int w = 50, h = 150;
	int score = 0; 
	double speed = 13;

	Player() {
		x = y = 0;
	}

	Player(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;

		this->w = w;
		this->h = h;
	}
};


struct Ball {
	double speed = 20.0; 
	double x = width / 2.0, y = height / 2.0;
	int w = 25, h = 25; 
	double velY = speed * std::sin(rand() / RAND_MAX), velX = speed * std::cos(rand() / RAND_MAX);
};



class GL_Draw {
public:

	int windowHeight = height; 
	int windowWidth = width;
	GLFWwindow* GLwindow = window;

	GL_Draw() {
		std::cout << "initializing GL..." << std::endl;
		initGL();//
	}

	void draw() {
		glClear(GL_COLOR_BUFFER_BIT);



		glfwSwapBuffers(GLwindow);
 	}

	~GL_Draw() {
		std::cout << "Destructing GL..." << std::endl;
		glfwTerminate(); //
	}
};


class Pong{
public: 
	 
	bool gameAlive = true; 
	std::vector<Player> player;// = std::vector<Player>(2);
	Ball ball;

	bool ceilCol = false; 
	bool paddleCol = false; 

	GL_Draw* gl;

	Pong(GL_Draw* gl) {
		this->gl = gl; 
		initVars(); 
	}



	void update() {

		ball.x += ball.velX; 
		ball.y += ball.velY;

		
		if (ball.x < (double)player[0].x + (double)player[0].w + 50.0 || ball.x >(double)player[1].x - 50.0) {
			for (int i = 0; i < player.size(); i++) {

				if (ball.x < (double)player[i].x + (double)player[i].w && ball.x + ball.w > player[i].x) {

					if (ball.y + ball.h > player[i].y && ball.y < (double)player[i].y + (double)player[i].h) {

						player[i].score++; 

						ball.x = (double)player[i].x + player[i].w - player[i].w * 1.5 * i; 

						int relative = (ball.y + ball.h / 2) - player[i].y - player[i].h / 2;

						double angle = ((double)relative / ((double)player[i].h / 2.0)) * ((5.0 * 3.141592) / 12.0);

						if (i == 0) {
							if (trainingActive){
								angle = (double)rand() / RAND_MAX;
							}

							ball.velY = ball.speed * -std::sin(angle);
							ball.velX = ball.speed * std::cos(angle);// * -((double)ball.velX / abs((double)ball.velX));
						}
						else {
							ball.velX = -ball.speed * std::cos(angle);
							ball.velY = ball.speed * -std::sin(angle);
						}
					}
				}

			}
		}

		if (ball.y < 10) {
			ball.velY *= -1; 
			ball.y = 10; 
		}
		else {
			if (ball.y + ball.h > height - 10.0) {
				ball.velY *= -1;
				ball.y = height - 10.0 - ball.h;
			}
		}


		if (ball.x < 0 || ball.x + ball.w > width) {
			gameAlive = false; 
		}


	}

	void initVars() {
		player.push_back(Player()); 
		player.push_back(Player()); 

		player[0].x = 25;
		player[0].y = 0; // (gl->windowHeight - player[0].h) / 2; 

		if (trainingActive) {
			player[0].h = height; 
		}

		player[1].x = gl->windowWidth - player[1].w - 25;
		player[1].y = (gl->windowHeight - player[1].h) / 2;
	}

};

GL_Draw gl;

std::vector<double> runPong(std::vector<AI>& ais) {
	

	std::vector<Pong> pongs; 
	std::vector<double> scores; 

	for (int i = 0; i < ais.size(); i++) {
		pongs.push_back(Pong(&gl)); 
		scores.push_back(0); 
	}
	bool instancesAlive = false; 
	bool running = true; 
	
	while (running && !glfwWindowShouldClose(window)) {// 

		//int syncVar = 
		sync(60); 
		//if (syncVar != -1) {
		//	std::cout << syncVar << std::endl; 
		//}

		instancesAlive = false; 

		///*
		if (!trainingActive) {

			pongs[0].player[0].y += -pongs[0].player[0].speed * (glfwGetKey(window, GLFW_KEY_UP) || glfwGetKey(window, GLFW_KEY_W)) + pongs[0].player[0].speed * (glfwGetKey(window, GLFW_KEY_DOWN) || glfwGetKey(window, GLFW_KEY_S));

		}
		//*/

		glClear(GL_COLOR_BUFFER_BIT);//

		for (int i = 0; i < pongs.size(); i++) {
			if (pongs[i].gameAlive) {
				instancesAlive = true; 

				std::vector<double> output = ais[i].input(
					{
						(double)(pongs[i].ball.y - pongs[i].player[1].y), 
						(double)((pongs[i].ball.y + pongs[i].ball.h) - ((double)pongs[i].player[1].y + pongs[i].player[1].h)), 
						(double)(pongs[i].ball.y),
						(double)(pongs[i].ball.velY),
						(double)(pongs[i].ball.velX),
						(double)(pongs[i].player[1].x - (pongs[i].ball.x + pongs[i].ball.w))
					});

				int pos = maxAtIndex(output); 
				if (pos == 0) {
					pongs[i].player[1].y += pongs[i].player[1].speed; 
				}
				else {
					if (pos == 1) {
						pongs[i].player[1].y -= pongs[i].player[1].speed;;
					}
				}

				pongs[i].update();

				drawRect1(pongs[i].player[0].x, pongs[i].player[0].y, pongs[i].player[0].w, pongs[i].player[0].h);
				drawRect1(pongs[i].player[1].x, pongs[i].player[1].y, pongs[i].player[1].w, pongs[i].player[1].h);
//////
				drawRect1(pongs[i].ball.x, pongs[i].ball.y, 25, 25);

				
			}
		}

		glfwSwapBuffers(window);//

		glfwPollEvents();//

		running = instancesAlive; 
	}

	for (int i = 0; i < pongs.size(); i++) {
		scores[i] = pongs[i].player[1].score; 
	}

	return scores; 

}



int maxAtIndex(std::vector<double>& vec) {
	double max = vec[0];
	int pos = 0;
	for (int i = 1; i < vec.size(); i++) {
		if (vec[i] > max) {
			max = vec[i];
			pos = i;
		}
	}

	return pos;
}

void drawRect1(const int x, const int y, const int w, const int h) {
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

int sync(int fps) {
	static int timeInterval = std::floor((1.0 / (double)fps) * 500); 

	static int frames = 0; 

	static unsigned int startTime = clock(); 
	

	if (clock() - startTime > 1000) {
		startTime = clock(); 
		int f = frames; 
		frames = 0; 
		return f;
	}


	frames++; 
	Sleep(timeInterval); 
	//std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval));
	return -1; 
}

/*
NN Inputs: 
- distance: ballY, userY(top)
- distance: ballY, userY(bot)
- ballY
- ballVelY
- ballVelX
- distance: ballX, userX
*/