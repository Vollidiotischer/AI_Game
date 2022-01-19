#include <iostream>
#include <chrono>
#include <vector> 
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "AI.h"

struct PP;
struct Player; 

void initGLFB();
void initVars(std::vector<AI>&);
void loop(std::vector<AI>&);
void draw(); 
void events(); 
void update(); 
void drawRect(const int x, const int y, const int w, const int h);
int isHitting(Player&, std::vector<PP>&);
void endGame(double&);
void fillPipeVec(); 
void shouldBeRunning(Player&); 
int nextPipe(); 

uint64_t getTime(); 

void run(); 

void makeDecision(); 

//Window
const int width = 1500;
const int height = 750;
GLFWwindow* windowFB;

//Gamestate
double friction = 0.99; 
double gravity = 2; 
bool gameRunning = true; 

//Player
std::vector<Player> players; 
std::vector<AI> aiList; 
std::vector<double> scores; 
constexpr int playerSize = 25; 
double spaceBoost = 15; 

//Pipes
constexpr int pipeWidth = 100; 
constexpr int gap = 150; 
std::vector<std::vector<PP>> pipes; 
double pipeSpeed = 15;
int maxPipeSize = 3; 

struct PP {
    double x, y;
    bool counted = false; 

    PP(double x, double y) {
        this->x = x;
        this->y = y;
    }
};

struct Player {
    double posX, posY; 
    double velX = 0, velY = 0;
    double score = 0; 
    bool alive = true; 
    bool insidePipe = false; 

    Player(double x, double y) {
        this->posX = x; 
        this->posY = y; 
    }
};

std::vector<double> runGame(std::vector<AI>& ais) {

    loop(ais);

    return scores; 
}

void ending() {
    glfwTerminate();
}

void loop(std::vector<AI>& ais) {

    initVars(ais);

    fillPipeVec(); 

    while (gameRunning && !glfwWindowShouldClose(windowFB)) {

        makeDecision(); 

        //events(); 

        update(); 

        draw(); 


        for (int i = 0; i < players.size(); i++) {
            if (players[i].alive) {
                shouldBeRunning(players[i]);
            }
        }



        bool stopGame = true;
        for (int i = 0; i < players.size(); i++) {
            if (players[i].alive) {
                stopGame = false; 
            }
        }
        gameRunning = !stopGame; 

        glfwPollEvents();
    }

    for (int i = 0; i < players.size(); i++) {
        scores[i] = players[i].score; 
    }

}

void makeDecision() {

    int closestPipe = nextPipe(); 

    for (int i = 0; i < players.size(); i++) {
        if (players[i].alive) {

            std::vector<double> out = aiList[i].input({ pipes[closestPipe][0].x - players[i].posX, players[i].posY - pipes[closestPipe][0].y, players[i].posY - pipes[closestPipe][1].y, players[i].velY });

            if (out[0] > out[1]) {
                players[i].velY -= spaceBoost;
            }
        }
    }

}

int nextPipe() {

    for (int i = 0; i < pipes.size(); i++) {
        if (pipes[i][0].x > players[0].posX) {
            return i;
        }
    }
    return 0; 
}

void events() {

    static bool pressedOnce = false; 

    if (glfwGetKey(windowFB, GLFW_KEY_SPACE) == GL_TRUE) {
        if (!pressedOnce) {
            players[0].velY -= spaceBoost;
            pressedOnce = true;
        }
    }
    else {
        pressedOnce = false; 
    }

}

void update() {

    for (int i = 0; i < pipes.size(); i++) {
        if (pipes[i][0].x < players[0].posX) {
            if (!pipes[i][0].counted) {
                pipes[i][0].counted = true;

                for (int i2 = 0; i2 < players.size(); i2++) {
                    if (players[i2].alive) {
                        players[i2].score++; 
                    }
                }

            }


        }
    }

    for (int i = 0; i < pipes.size(); i++) {
        pipes[i][0].x -= pipeSpeed; 
        pipes[i][1].x = pipes[i][0].x; 
        
    }

    for (int i = 0; i < pipes.size(); i++) {
        if (pipes[i][0].x < - (pipeWidth + 100)) {
            pipes.erase(pipes.begin() + i);  

        }
    }

    if (pipes.size() < maxPipeSize) {
        PP a(width, rand() % (height - gap - 200));
        pipes.push_back({ a, PP(width, a.y + gap) }); 
    }

    players[0].posX += players[0].velX;
    players[0].posY += players[0].velY;

    players[0].velY += gravity;

    players[0].velX *= friction;
    players[0].velY *= friction;

    for (int i = 1; i < players.size(); i++) {
        if (players[i].alive) {
            players[i].posX += players[i].velX;
            players[i].posY += players[i].velY;

            players[i].velY += gravity;

            players[i].velX *= friction;
            players[i].velY *= friction;
        }
    }

    /*
    pos[0] += vel[0]; 
    pos[1] += vel[1]; 

    vel[1] += gravity; 

    vel[0] *= friction; 
    vel[1] *= friction; 
    */
}

void draw() {

    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < players.size(); i++) {
        if (players[i].alive) {
            drawRect(players[i].posX, players[i].posY, playerSize, playerSize);
        }
    }

    for (int i = 0; i < pipes.size(); i++) {
        drawRect(pipes[i][0].x, 0, pipeWidth, pipes[i][0].y); 

        drawRect(pipes[i][0].x, pipes[i][1].y, pipeWidth, height - pipes[i][1].y);
    }

    glfwSwapBuffers(windowFB);
}

int isHitting(Player& player, std::vector<PP>& hindernis) {
    if ((double)player.posX + (double)playerSize > hindernis[0].x && player.posX < hindernis[0].x + (double)pipeWidth) {
        if (player.posY < hindernis[0].y || (double)player.posY + playerSize > hindernis[1].y) {
            return 1; 
        }
        return 2; 
    }

    return 0; 
}

void drawRect(const int x, const int y, const int w, const int h) {
    glBegin(GL_QUADS); 
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd(); 
}

void endGame(double& sc) {
    sc += (1 / pipes[nextPipe()][0].x);
    std::cout << "Final score is: " << sc << std::endl; 
}

void shouldBeRunning(Player& p) {

        if (p.posY < -playerSize || p.posY >(double)height + (double)playerSize) {
            endGame(p.score);
            p.alive = false;
            return;
        }
   
    
        for (int i = 0; i < pipes.size(); i++) {
            int temp = isHitting(p, pipes[i]);

            if (temp == 1) {
                endGame(p.score);
                p.alive = false;
            }
        }

}

void fillPipeVec() {
    pipes.clear(); 
    for (int i = 0; i < maxPipeSize; i++) {
        PP a((double)(((double)width / 1.5) + (double)i * ((double)width / (double)maxPipeSize)), rand() % (height - gap - 200));
        pipes.push_back({ a, PP(a.x, a.y + gap) });
    }
}

uint64_t getTime() {
    using namespace std::chrono; 
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void initGLFB() {

    /* Initialize the library */
    if (!glfwInit()) {
        throw "Failed to initialize";
    }

    /* Create a windowed mode window and its OpenGL context */
    windowFB = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!windowFB){
        glfwTerminate();
        throw "Failed to create window"; 
    }


    /* Make the window's context current */
    glfwShowWindow(windowFB);
    glfwMakeContextCurrent(windowFB);

    glewInit();

    glOrtho(0.f, width, height, 0.f, 0.f, 1.f);

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
}

void initVars(std::vector<AI>& ais) {
    srand(time(NULL)); 

    gameRunning = true; 

    aiList.clear(); 
    scores.clear();
    players.clear();

    aiList = ais; 

    for (int i = 0; i < aiList.size(); i++) {
        players.push_back(Player(500.0, 250.0)); 
        scores.push_back(0); 
    }

}

/* 

games: 
- Pong

*/