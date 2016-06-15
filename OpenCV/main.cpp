#include "main.hpp"
#include <pthread.h>


#define VIDEO_DEVICE_NUM 0 //number of video device
#define ALPHA 0.9 //0.6   //influence of new direction
#define STEERING_LEVEL_SIZE 5.0 //size of a discrete steering level
#define STEERING_LEVEL_SIZE_PROGRESSION 0.5 //dif size between progressing steering levels
#define MIN_GRADIENT_THRESHOLD 25 //points with lower gradient value are not considert to be a edge of the line
#define MAX_POINT_DISTANCE_Y 50 //maximum distance between two neighbouring points on y-axis
#define MAX_POINT_DISTANCE_X 80 //maximum distance between two neighbouring points on x-axis
#define UARTDEV "/dev/ttyAMA0"	// The device file for the UART connection to the Hano board.

//#define debug
//#define output
using namespace cv;


const Mat LINE_DETECTION_MASK = (Mat_<char>(3,3) << -1,0,1,-2,0,2,-1,0,1);
const float alpha(ALPHA);    //influence of new direction


int main(){
    //calcDirectionWindowed();

/*    pthread_t threads[4];
    struct args
    {   
      // Argument for threads  
        
    };
*/
    int uart_handle(-1);
    uart_handle = init_uart();
    if(uart_handle==-1) return -1;



    VideoCapture cap(VIDEO_DEVICE_NUM);
    if(!cap.isOpened()){
        std::cout << "Could not open video stream!\n";
        return -1;
    }
    Mat frameBuffer;
    float cur_dir, new_dir = 0;
    unsigned char data = 7; //7 is straight
/*    for(int i = 0; i < 4; i++){
        pthread_create(threads + i, NULL ,&calcDirectionThread,args);   
    }
*/    


    while(1){
        new_dir = calcDirection(frameBuffer, cap);
        cur_dir = (1-alpha) * cur_dir + alpha * new_dir;
#ifdef output
	std::cout << "------------------------------------------------\n";
        std::cout << "new direction is: " << new_dir << '\n';
        std::cout << "driving direction is " << cur_dir << '\n';
#endif
        data = map_angle(cur_dir, STEERING_LEVEL_SIZE, STEERING_LEVEL_SIZE_PROGRESSION);
#ifdef output
        std::cout << "Uart data is: " << (int) data << std::endl;
#endif
        uart_write(uart_handle, data);
    }
    return 0;
}

void calcDirectionWindowed()
{
    VideoCapture cap(VIDEO_DEVICE_NUM);

    if(!cap.isOpened()){
        std::cout << "Could not open video stream!\n";
        return;
    }
    Mat frame, image;
    //Mat mask = (Mat_<char>(3,3) <<  0, -1, 0, -1, 5, -1, 0, -1, 0);
    Mat mask = (Mat_<char>(3,3) << -1,0,1,-2,0,2,-1,0,1);

    std::cout << "Mask = "<< std::endl << mask << std::endl;

    namedWindow("Original", CV_WINDOW_AUTOSIZE);
    namedWindow("Window", CV_WINDOW_AUTOSIZE);

    while(1){
        cap >> frame; //get frame
        GaussianBlur(frame, image, Size2i(5,5),0,0, BORDER_DEFAULT); //apply gaussian filter
        filter2D(image, image, frame.depth(), mask); //aply maks
        cvtColor(image, image, CV_RGB2GRAY, 1); //reduce color to one channel

        //calc max and min points for each row
        std::vector<Point2i> max_points(image.rows-2);
        std::vector<Point2i> min_points(image.rows-2);
        char max_value = 0;
        char min_value = CHAR_MAX;
        char current_value = 0;
        int x_min = 0;
        int x_max = 0;
        for(int j = 1; j<image.rows-1; j++){ //edges not interpolated
            max_value = 0;
            min_value = CHAR_MAX;
            x_min = 0;
            x_max = 0;
            for(int i = 1; i < image.cols-1; i++){
                current_value = image.ptr<uchar>(j)[i];
                if(max_value < current_value){
                    max_value = current_value;
                    x_max = i;
                }
                if(min_value > current_value){
                    min_value = current_value;
                    x_min = i;
                }
            }
            max_points[j-1] = cvPoint(x_max, j-1);
            min_points[j-1] = cvPoint(x_min, j-1);
        }

        //calculate lines
        Vec4f max_line;
        Vec4f min_line;
        fitLine(max_points, max_line, CV_DIST_L2, 0, 0.01, 0.01);
        fitLine(min_points, min_line, CV_DIST_L2, 0, 0.01, 0.01);

        cvtColor(image, image, CV_GRAY2RGB, 3); //three channels are needed to draw colored lines

        //draw min- and max-lines on the image
        Point2i max_point1 = cvPoint(max_line[2]- 200*max_line[0], max_line[3] - 200*max_line[1]);
        Point2i max_point2 = cvPoint(max_line[2]+ 200*max_line[0], max_line[3] + 200*max_line[1]);
        if(max_point1.y>max_point2.y){
            Point2i buff = max_point2;
            max_point2 = max_point1;
            max_point1 = buff;
        }

        Point2i min_point1 = cvPoint(min_line[2]+ 200*min_line[0], min_line[3] + 200*min_line[1]);
        Point2i min_point2 = cvPoint(min_line[2]- 200*min_line[0], min_line[3] - 200*min_line[1]);
        Point2i car_position = cvPoint(image.cols/2, image.rows);

        line(image, max_point1, max_point2, CV_RGB(255,0,0), 1,8,0); //max line (red)
        line(image, min_point1, min_point2, CV_RGB(0,255,0), 1,8,0); //min line (yellow)
        line(image, car_position, max_point1, CV_RGB(0,0,255), 2, 8, 0); //direction of car (blue)

        imshow("Original", frame);
        imshow("Window", image);


        waitKey(10); //delay; necessary for winows to be updated

        Vec2f car_direction(max_point1.x-car_position.x, max_point1.y-car_position.y);
        normalize(car_direction, car_direction, 1, NORM_L1);
        float direction = copysignf(std::acos(std::abs(car_direction[1])), car_direction[0]);
        direction = direction * 180 / M_PI;
        std::cout << "The calculated direction is: " << direction << '\n';

    }
}

float calcDirection(Mat buff, VideoCapture cap){


    cap >> buff; //get frame
//    GaussianBlur(buff, buff, Size2i(5,5),0,0, BORDER_DEFAULT); //apply gaussian filter
    filter2D(buff, buff, buff.depth(), LINE_DETECTION_MASK); //aply maks
    cvtColor(buff, buff, CV_RGB2GRAY, 1); //reduce color to one channel

    //calc max points for each row
    std::vector<Point2i> max_points(buff.rows-2);
    char max_value = 0;
    char current_value = 0;
    int x_max, point_count = 0;
    for(int j = buff.rows-2; j>1; j--){ //edges not interpolated
        if(point_count>0 && std::abs(max_points[point_count-1].y-j)>MAX_POINT_DISTANCE_Y) break;
        max_value = 0;
        x_max = 0;
        int i = point_count>0 && max_points[point_count-1].x>MAX_POINT_DISTANCE_X ? max_points[point_count-1].x-MAX_POINT_DISTANCE_X : 1;
        for(; i < buff.cols-1; i++){
            if(point_count>0 && i-max_points[point_count-1].x>MAX_POINT_DISTANCE_X) break;
            current_value = buff.ptr<uchar>(j)[i];
            if(max_value < current_value){
                max_value = current_value;
                x_max = i;
            }
        }
        if(max_value>MIN_GRADIENT_THRESHOLD) max_points[point_count++] = cvPoint(x_max, j-1); //point is not added if gradient to small
    }
#ifdef output
    std::cout <<"Vector size: " << point_count << '\n'; //TODO just for debugging
#endif
	if( point_count < 2){
		point_count = 2;
		max_points[0] = cvPoint(buff.cols/2, 0);
		max_points[1] = cvPoint(buff.cols/2, buff.rows);
	}
    max_points.resize(point_count);

    //Fit line on max_points
    Vec4f max_line;
    fitLine(max_points, max_line, CV_DIST_L2, 0, 0.01, 0.01);

    //Point the car should drive to
    Point2i max_point = cvPoint(max_line[2] - 200*max_line[0], max_line[3] - 200*max_line[1]);

    //make sure max_point is the further away one (line output does some weird shit)
    Point2i max_pointb = cvPoint(max_line[2] + 200*max_line[0], max_line[3] + 200*max_line[1]);
    if(max_point.y>max_pointb.y) max_point = max_pointb;

    //estimated position of car (TODO)
    Point2i car_position = cvPoint(buff.cols/2, buff.rows);

    //Calculate driving direction
    Vec2f car_direction(max_point.x-car_position.x, max_point.y-car_position.y);
    normalize(car_direction, car_direction, 1, NORM_L1);
    float direction = copysignf(std::acos(std::abs(car_direction[1])), car_direction[0]);
    direction = direction * 180 / M_PI;

    // Just for debugging
#ifdef debug
    Mat debug;
    cvtColor(buff, debug, CV_GRAY2RGB, 3);
    for(unsigned int i = 0; i<max_points.size(); i++){
        circle(debug, max_points[i], 1, CV_RGB(255,255,0), 1, 8, 0);
    }
    namedWindow("Debug", WINDOW_AUTOSIZE);
    imshow("Debug", debug);
    waitKey(10);
#endif

    return direction;
}


//returns handle to filestream
int init_uart(){
    int handle = open(UARTDEV, O_RDWR | O_NOCTTY | O_NDELAY); //Initialisierung der UART
    if (handle == -1) {
        std::cout << "[ERROR] UART open()\n";
    }else{
        struct termios options;
        tcgetattr(handle, &options);
        options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        tcflush(handle, TCIFLUSH);
        tcsetattr(handle, TCSANOW, &options);
    }
    return handle;
}

int uart_write(int handle, unsigned char data){
    if(handle==-1){
        std::cout << "[ERROR] invalid handle\n";
        return -1;
    }
    int out = write(handle, &data, 1);
    if(out!=1){
        std::cout << "[ERROR] write data uart\n";
        return -1;
    }
    return 1;
}

//map angle to level understood by the nanoboard
unsigned char map_angle(float dir, float level_range, float prog){
    unsigned char level(7); //straight
    float step(level_range);    //range of level (in degree)
    if(dir>0.0){
        while(dir>step && level<14){
            level++;
            dir-=step;
	    if(step>prog) step-=prog;
        }
    }else{
        step *= -1;
        while(dir<step && level>0){
            level--;
            dir-=step;
	    if(step<prog) step+=prog;
        }
    }
    return level;
}



