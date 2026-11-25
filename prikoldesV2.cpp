#include <iostream>
#include <cstring>
#include <cmath>
#include <thread>
#include <chrono>

using namespace std;

void calculatePoints(float x, float y, float z, float A, float B, float C, char* buffer, int width, int height) {
    float x1 = x;
    float y1 = y * cos(A) - z * sin(A);
    float z1 = z * cos(A) + y * sin(A);

    float x2 = x1 * cos(B) - z1 * sin(B);
    float y2 = y1;
    float z2 = z1 * cos(B) + x1 * sin(B);

    float x3 = x2 * cos(C) - y2 * sin(C);
    float y3 = y2 * cos(C) + x2 * sin(C);
    float z3 = z2;

    float z_cam = z3 + 5.0;
    int xp = (int)(width / 2 + 40.0 * (x3 / z_cam) * 2);
    int yp = (int)(height / 2 + 40.0 * (y3 / z_cam));
    if (xp >= 0 && xp < width && yp >= 0 && yp < height) {
        buffer[yp * width + xp] = '#';
    }
}

int main() {

    int width = 80, height = 40;
    char buffer[80 * 40];
    float A = 0, B = 0, C = 0;
    
    while(true) {
        string frame;
        std::cout << "\033[?25l";
        memset(buffer, ' ', sizeof(buffer));
        for (float i = -1.0; i < 1.0; i += 0.05) {
            calculatePoints(i, -1.0, -1.0, A, B, C, buffer, width, height);
            calculatePoints(i, 1.0, -1.0, A, B, C, buffer, width, height);
            calculatePoints(i, -1.0, 1.0, A, B, C, buffer, width, height);
            calculatePoints(i, 1.0, 1.0, A, B, C, buffer, width, height);

            calculatePoints(-1.0, i, -1.0, A, B, C, buffer, width, height);
            calculatePoints(1.0, i, -1.0, A, B, C, buffer, width, height);
            calculatePoints(-1.0, i, 1.0, A, B, C, buffer, width, height);
            calculatePoints(1.0, i, 1.0, A, B, C, buffer, width, height);

            calculatePoints(-1.0, -1.0, i, A, B, C, buffer, width, height);
            calculatePoints(1.0, -1.0, i, A, B, C, buffer, width, height);
            calculatePoints(-1.0, 1.0, i, A, B, C, buffer, width, height);
            calculatePoints(1.0, 1.0, i, A, B, C, buffer, width, height);
        }
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                frame += buffer[y * width + x];
            }
            frame += '\n';
        }
        std::cout << "\033[1;1H" << frame;
        A += 0.03;
        B += 0.02;
        C += 0.01;
        this_thread::sleep_for(chrono::milliseconds(8));
    }
}
