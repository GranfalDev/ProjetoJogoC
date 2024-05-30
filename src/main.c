// game.c
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef enum { MENU, GAME, GAME_OVER } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;

GameState gameState = MENU;
Difficulty difficulty = EASY;
int score = 0;
int shipX = WINDOW_WIDTH / 2;
int shipY = 50;
int shipSpeed = 10;
int bulletX = -1;
int bulletY = -1;
int bulletSpeed = 15;
int debrisX;
int debrisY;
int debrisSpeed;
int isBulletActive = 0;

void init();
void display();
void reshape(int width, int height);
void timer(int value);
void keyboard(unsigned char key, int x, int y);
void drawMenu();
void drawGame();
void drawGameOver();
void drawShip();
void drawBullet();
void drawDebris();
void spawnDebris();
void checkCollision();
void resetGame();

int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Space Shooter V1.0");
    
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (gameState == MENU) {
        drawMenu();
    } else if (gameState == GAME) {
        drawGame();
    } else if (gameState == GAME_OVER) {
        drawGameOver();
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
}

void timer(int value) {
    if (gameState == GAME) {
        if (isBulletActive) {
            bulletY += bulletSpeed;
            if (bulletY > WINDOW_HEIGHT) {
                isBulletActive = 0;
            }
        }
        debrisY -= debrisSpeed;
        if (debrisY < 0) {
            spawnDebris();
        }
        checkCollision();
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Aproximadamente 60 fps
}

void keyboard(unsigned char key, int x, int y) {
    if (gameState == MENU) {
        if (key == '1') {
            difficulty = EASY;
            gameState = GAME;
            spawnDebris();
        } else if (key == '2') {
            difficulty = MEDIUM;
            gameState = GAME;
            spawnDebris();
        } else if (key == '3') {
            difficulty = HARD;
            gameState = GAME;
            spawnDebris();
        }
    } else if (gameState == GAME) {
        if (key == 'a' || key == 'A') {
            shipX -= shipSpeed;
        } else if (key == 'd' || key == 'D') {
            shipX += shipSpeed;
        } else if (key == ' ') {
            if (!isBulletActive) {
                isBulletActive = 1;
                bulletX = shipX;
                bulletY = shipY + 10;
            }
        }
    } else if (gameState == GAME_OVER) {
        if (key == 'r' || key == 'R') {
            resetGame();
        }
    }
}

void drawMenu() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20);
    const char* msg = "1. Easy";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
    msg = "2. Medium";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 20);
    msg = "3. Hard";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawGame() {
    drawShip();
    if (isBulletActive) {
        drawBullet();
    }
    drawDebris();
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(10, WINDOW_HEIGHT - 20);
    for (const char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawGameOver() {
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20);
    const char* msg = "Game Over";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
    for (const char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 20);
    msg = "Press 'R' to restart";
    for (const char* c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawShip() {
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2i(shipX, shipY);
    glVertex2i(shipX - 10, shipY - 20);
    glVertex2i(shipX + 10, shipY - 20);
    glEnd();
}

void drawBullet() {
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(bulletX - 2, bulletY);
    glVertex2i(bulletX + 2, bulletY);
    glVertex2i(bulletX + 2, bulletY + 10);
    glVertex2i(bulletX - 2, bulletY + 10);
    glEnd();
}

void drawDebris() {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(debrisX - 10, debrisY - 10);
    glVertex2i(debrisX + 10, debrisY - 10);
    glVertex2i(debrisX + 10, debrisY + 10);
    glVertex2i(debrisX - 10, debrisY + 10);
    glEnd();
}

void spawnDebris() {
    debrisX = rand() % WINDOW_WIDTH;
    debrisY = WINDOW_HEIGHT;
    switch (difficulty) {
        case EASY:
            debrisSpeed = 5;
            break;
        case MEDIUM:
            debrisSpeed = 10;
            break;
        case HARD:
            debrisSpeed = 15;
            break;
    }
}

void checkCollision() {
    if (isBulletActive && bulletX >= debrisX - 10 && bulletX <= debrisX + 10 &&
        bulletY >= debrisY - 10 && bulletY <= debrisY + 10) {
        isBulletActive = 0;
        score += 10;
        spawnDebris();
    }

    if (shipX >= debrisX - 10 && shipX <= debrisX + 10 &&
        shipY >= debrisY - 10 && shipY <= debrisY + 10) {
        gameState = GAME_OVER;
    }
}

void resetGame() {
    score = 0;
    shipX = WINDOW_WIDTH / 2;
    shipY = 50;
    isBulletActive = 0;
    gameState = MENU;
}
