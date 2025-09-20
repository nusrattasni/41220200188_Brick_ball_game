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
int brickRows = 5, brickCols = 10;
float brickWidth = 70, brickHeight = 20;
std::vector<std::vector<int>> bricks; // health: 0 = broken, 1 = weak, 2 = medium, 3 = strong

bool gameOver = false;
int score = 0;
int lives = 3;
int level = 1;

void initBricks() {
    bricks.assign(brickRows, std::vector<int>(brickCols, 3)); // every brick starts with 3 hits
}

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

void drawBackground() {
    glBegin(GL_QUADS);
        glColor3f(0.05f, 0.05f, 0.2f); // bottom dark blue
        glVertex2f(0, 0);
        glVertex2f(windowWidth, 0);

        glColor3f(0.2f, 0.2f, 0.5f); // top lighter blue
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0, windowHeight);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background
    drawBackground();

    // Paddle color changes with lives
    if (lives == 3) glColor3f(0, 1, 0);
    else if (lives == 2) glColor3f(1, 0.5f, 0);
    else glColor3f(1, 0, 0);
    drawRect(paddleX, paddleY, paddleWidth, paddleHeight);

    // Ball
    glColor3f(1, 1, 0);
    drawCircle(ballX, ballY, ballRadius);

    // Bricks (all shades of red)
    for (int i = 0; i < brickRows; i++) {
        for (int j = 0; j < brickCols; j++) {
            int health = bricks[i][j];
            if (health > 0) {
                if (health == 3) glColor3f(1, 0.0f, 0.0f);   // strong red
                else if (health == 2) glColor3f(1, 0.4f, 0.4f); // medium red
                else if (health == 1) glColor3f(1, 0.7f, 0.7f); // light red

                drawRect(j * (brickWidth + 10) + 35, i * (brickHeight + 10) + 400,
                         brickWidth, brickHeight);
            }
        }
    }

    // Display Score
    char scoreStr[30];
    sprintf(scoreStr, "Score: %d", score);
    glColor3f(1, 1, 1);
    drawText(650, 570, scoreStr);

    // Display Lives
    char livesStr[30];
    sprintf(livesStr, "Lives: %d", lives);
    drawText(20, 570, livesStr);

    // Display Level
    char levelStr[30];
    sprintf(levelStr, "Level: %d", level);
    drawText(320, 570, levelStr);

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
                if (bricks[i][j] > 0) {
                    float bx = j * (brickWidth + 10) + 35;
                    float by = i * (brickHeight + 10) + 400;
                    if (ballX > bx && ballX < bx + brickWidth &&
                        ballY > by && ballY < by + brickHeight) {
                        bricks[i][j]--; // reduce health
                        ballDY *= -1.05f; // bounce & speed up slightly
                        if (bricks[i][j] == 0) score++; // score only when destroyed
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
                ballDX = 4 + level;
                ballDY = 4 + level;
                paddleX = 350;
            }
        }

        // Win condition for a level
        bool allBricksBroken = true;
        for (int i = 0; i < brickRows; i++)
            for (int j = 0; j < brickCols; j++)
                if (bricks[i][j] > 0) allBricksBroken = false;

        if (allBricksBroken) {
            level++;
            brickRows++; // add a row each level
            ballDX = (ballDX > 0 ? 4 + level : -(4 + level));
            ballDY = (ballDY > 0 ? 4 + level : -(4 + level));
            ballX = 400;
            ballY = 300;
            paddleX = 350;
            initBricks();
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
        level = 1;
        brickRows = 5;
        initBricks();

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
    initBricks();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Brick Breaker with Levels + Brick Durability");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
