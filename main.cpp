#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <cstdio>


const int windowWidth = 800;
const int windowHeight = 600;

// Paddle
float paddleX = 350, paddleY = 50, paddleWidth = 100, paddleHeight = 20;

// Ball
float ballX = 400, ballY = 300, ballRadius = 10;
float ballDX = 4, ballDY = 4;

// Bricks
const int brickRows = 5, brickCols = 10;
float brickWidth = 70, brickHeight = 20;
std::vector<std::vector<bool>> bricks(brickRows, std::vector<bool>(brickCols, true));

bool gameOver = false;
int score = 0;
int lives = 3;

void drawRect(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i++) {
        float angle = i * 3.14159f / 180;
        glVertex2f(cx + std::cos(angle) * r, cy + std::sin(angle) * r);
    }
    glEnd();
}

void drawText(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    for (int i = 0; str[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Paddle color changes with lives
    if (lives == 3) glColor3f(0, 1, 0);
    else if (lives == 2) glColor3f(1, 0.5f, 0);
    else glColor3f(1, 0, 0);
    drawRect(paddleX, paddleY, paddleWidth, paddleHeight);

    glColor3f(1, 1, 0); // Ball
    drawCircle(ballX, ballY, ballRadius);

    // Bricks
    for (int i = 0; i < brickRows; i++) {
        for (int j = 0; j < brickCols; j++) {
            if (bricks[i][j]) {
                glColor3f(1, 0.2f * i, 0.1f * j); // colorful
                drawRect(j * (brickWidth + 10) + 35, i * (brickHeight + 10) + 400, brickWidth, brickHeight);
            }
        }
    }

    // Display Score
    char scoreStr[30];
    sprintf(scoreStr, "Score: %d", score);
    glColor3f(1, 1, 1);
    drawText(700, 570, scoreStr);

    // Display Lives
    char livesStr[30];
    sprintf(livesStr, "Lives: %d", lives);
    drawText(20, 570, livesStr);

    // Display Game Over or Win
    if (gameOver) {
        glColor3f(1, 0, 0);
        if (lives <= 0)
            drawText(300, 300, "Game Over! Press R to Restart");
        else
            drawText(310, 300, "You Win! Press R to Restart");
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        ballX += ballDX;
        ballY += ballDY;

        // Wall collision
        if (ballX - ballRadius < 0 || ballX + ballRadius > windowWidth)
            ballDX *= -1;
        if (ballY + ballRadius > windowHeight)
            ballDY *= -1;

        // Paddle collision
        if (ballX > paddleX && ballX < paddleX + paddleWidth &&
            ballY - ballRadius < paddleY + paddleHeight) {
            ballDY *= -1;
            ballY = paddleY + paddleHeight + ballRadius;
        }

        // Brick collision
        for (int i = 0; i < brickRows; i++) {
            for (int j = 0; j < brickCols; j++) {
                if (bricks[i][j]) {
                    float bx = j * (brickWidth + 10) + 35;
                    float by = i * (brickHeight + 10) + 400;
                    if (ballX > bx && ballX < bx + brickWidth &&
                        ballY > by && ballY < by + brickHeight) {
                        bricks[i][j] = false;
                        ballDY *= -1.05f; // slightly increase speed
                        score++;
                    }
                }
            }
        }

        // Lose life if ball goes below screen
        if (ballY < 0) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
            } else {
                ballX = 400;
                ballY = 300;
                ballDX = 4;
                ballDY = 4;
                paddleX = 350;
            }
        }

        // Win condition
        bool allBricksBroken = true;
        for (int i = 0; i < brickRows; i++)
            for (int j = 0; j < brickCols; j++)
                if (bricks[i][j]) allBricksBroken = false;

        if (allBricksBroken) {
            gameOver = true;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'a' && paddleX > 0) paddleX -= 20;
    if (key == 'd' && paddleX + paddleWidth < windowWidth) paddleX += 20;

    if (key == 'r' || key == 'R') {
        // Reset game fully
        ballX = 400;
        ballY = 300;
        ballDX = 4;
        ballDY = 4;
        paddleX = 350;
        score = 0;
        lives = 3;
        gameOver = false;

        for (int i = 0; i < brickRows; i++)
            for (int j = 0; j < brickCols; j++)
                bricks[i][j] = true;

        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
    }
}

void mouseMove(int x, int y) {
    paddleX = x - paddleWidth / 2;
    if (paddleX < 0) paddleX = 0;
    if (paddleX + paddleWidth > windowWidth) paddleX = windowWidth - paddleWidth;
}

void init() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, 0, windowHeight);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Brick Breaker");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
