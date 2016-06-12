#include "main.hpp"


using namespace cv;


const Mat LINE_DETECTION_MASK = (Mat_<char>(3,3) << -1,0,1,-2,0,2,-1,0,1);

int main(){
    //calcDirectionWindowed();


    int uart_handle(-1);
    uart_handle = init_uart();
    if(uart_handle==-1) return -1;


    VideoCapture cap(1);
    if(!cap.isOpened()){
        std::cout << "Could not open video stream!\n";
        return -1;
    }
    Mat frameBuffer;
    //influence of new direction
    float alpha = 0.2;
    float cur_dir, new_dir = 0;
    unsigned char data = 7; //7 is straight
    while(1){
        new_dir = calcDirection(frameBuffer, cap);
        cur_dir = (1-alpha) * cur_dir + alpha * new_dir;
        std::cout << "new direction is: " << new_dir << '\n';
        std::cout << "driving direction is " << cur_dir << '\n';
        data = 7 + (char)(cur_dir/4.28);
        uart_write(uart_handle, data);
    }
    return 0;
}

void calcDirectionWindowed()
{
    VideoCapture cap(1);

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
        float direction = std::copysignf(std::acos(std::abs(car_direction[1])), car_direction[0]);
        direction = direction * 180 / M_PI;
        std::cout << "The calculated direction is: " << direction << '\n';

    }
}

float calcDirection(Mat buff, VideoCapture cap){


    cap >> buff; //get frame
    GaussianBlur(buff, buff, Size2i(5,5),0,0, BORDER_DEFAULT); //apply gaussian filter
    filter2D(buff, buff, buff.depth(), LINE_DETECTION_MASK); //aply maks
    cvtColor(buff, buff, CV_RGB2GRAY, 1); //reduce color to one channel

    //calc max points for each row
    std::vector<Point2i> max_points(buff.rows-2);
    char max_value = 0;
    char current_value = 0;
    int x_max = 0;
    for(int j = 1; j<buff.rows-1; j++){ //edges not interpolated
        max_value = 0;
        x_max = 0;
        for(int i = 1; i < buff.cols-1; i++){
            current_value = buff.ptr<uchar>(j)[i];
            if(max_value < current_value){
                max_value = current_value;
                x_max = i;
            }
        }
        max_points[j-1] = cvPoint(x_max, j-1);
    }

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
    float direction = std::copysignf(std::acos(std::abs(car_direction[1])), car_direction[0]);
    direction = direction * 180 / M_PI;

    /* Just for debugging
    namedWindow("Debug", WINDOW_AUTOSIZE);
    imshow("Debug", buff);
    waitKey(10);
    */

    return direction;
}


//returns handle to filestream
int init_uart(){
    int handle = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); //Initialisierung der UART
    if (handle == -1) {
        std::cout << "[ERROR] UART open()\n";
    }else{
        struct termios options;
        tcgetattr(handle, &options);
        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
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



