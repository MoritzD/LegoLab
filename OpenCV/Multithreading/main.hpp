#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <pthread.h>
#include <thread>
#include <mutex>
#include <time.h>

void calcDirectionWindowed();
float calcDirection(cv::Mat buff);
int init_uart();
int uart_write(int handle, unsigned char data);
unsigned char map_angle(float dir, float level_range, float prog);
void threadMainLoop(cv::Mat buff);

#endif // MAIN_H
