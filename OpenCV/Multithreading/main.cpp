#include "main.hpp"




#define VIDEO_DEVICE_NUM 0 //number of video device
#define ALPHA 0.4 //0.6   //influence of new direction
#define STEERING_LEVEL_SIZE 5.0 //size of a discrete steering level
#define STEERING_LEVEL_SIZE_PROGRESSION 0//.5 //dif size between progressing steering levels
#define MIN_GRADIENT_THRESHOLD 25 //points with lower gradient value are not considert to be a edge of the line
#define MAX_POINT_DISTANCE_Y 50 //maximum distance between two neighbouring points on y-axis
#define MAX_POINT_DISTANCE_X 80 //maximum distance between two neighbouring points on x-axis
#define UARTDEV "/dev/ttyAMA0"	// The device file for the UART connection to the Hano board.
#define THREAD_NUMBER 4



//#define DEBUG
#define OUTPUT
//#define UART

using namespace cv;


const Mat LINE_DETECTION_MASK = (Mat_<char>(3,3) << -1,0,1,-2,0,2,-1,0,1);
const float alpha(ALPHA);    //Influence of new direction

VideoCapture cap(VIDEO_DEVICE_NUM); //Used to cature frame of camare
std::mutex cap_mutex;	//Mutex to synchronize cap

float current_Direction(0.0);	//Direction that is used by mainthread
struct timeval tp_last_frame;	//Used to check if frame is up to date
std::mutex cur_dir_mutex;	//Mutex to synchronize current_Direction

std::condition_variable cond_var;
std::mutex cond_mutex;



int main(){
		
	gettimeofday(&tp_last_frame, NULL); //Init tp_last_frame

	//Init Uart connection
#ifdef UART
    int uart_handle(-1);
    uart_handle = init_uart();
    if(uart_handle==-1) return -1;
#endif

    if(!cap.isOpened()){
        std::cout << "Could not open video stream!\n";
        return -1;
    }
	 //Data that is send via Uart
    unsigned char data = 7; //7 is steering straight
		
	 //Create and start threads used for framecomputation
    std::thread threads[THREAD_NUMBER];
    Mat thread_buffer[THREAD_NUMBER];
    for(int i = 0; i<THREAD_NUMBER; i++){
        threads[i] = std::thread(threadMainLoop, thread_buffer[i]);
    }

	 //Buffer for direction
    float cur_dir(0.0);

	 //Loop of main thread
    while(1){
		  //Get current Direction(steering angle)
        cur_dir_mutex.lock();
        cur_dir = current_Direction;
        cur_dir_mutex.unlock();

		  //Map direction to discrete angles
        data = map_angle(cur_dir, STEERING_LEVEL_SIZE, STEERING_LEVEL_SIZE_PROGRESSION);

#ifdef OUTPUT
        std::cout << "------------------------------------------------\n";
        std::cout << "driving direction is " << cur_dir << '\n';
        std::cout << "Uart data is: " << (int) data << std::endl;
#endif

		  //Send data via Uart
#ifdef UART
        uart_write(uart_handle, data);
#endif

		  //Wait until new frame got calculated (preventing busy wait)
		  std::unique_lock<std::mutex> lock(cond_mutex);
		  cond_var.wait(lock);
		  lock.unlock();
		  
    }
    return 0;
}

//Function to compute angle given a frame
float calcDirection(Mat buff){


#ifdef DEBUG
	 std::stringstream oss;
	 oss << "Origin for " << std::this_thread::get_id();
    namedWindow(oss.str(), WINDOW_AUTOSIZE);
    imshow(oss.str(), buff);
#endif


    //cap >> buff; //get frame
//    GaussianBlur(buff, buff, Size2i(5,5),0,0, BORDER_DEFAULT); //apply gaussian filter
    filter2D(buff, buff, buff.depth(), LINE_DETECTION_MASK); //aply maks
    cvtColor(buff, buff, CV_RGB2GRAY, 1); //reduce color to one channel

    //calc max points for each row
    std::vector<Point2i> max_points(buff.rows-2);
    char max_value = 0;
    char current_value = 0;
    int x_max, point_count = 0;
    for(int j = buff.rows-2; j>1; j--){ //edges not interpolated by filter2D (--> buff.rows-2, ...)
		  //Stops if y-Distance between points get to big
        if(point_count>0 && std::abs(max_points[point_count-1].y-j)>MAX_POINT_DISTANCE_Y) break;
        max_value = 0;
        x_max = 0;
		  //Set startingpoint to search row
        int i = point_count>0 && max_points[point_count-1].x>MAX_POINT_DISTANCE_X ? max_points[point_count-1].x-MAX_POINT_DISTANCE_X : 1;
        for(; i < buff.cols-1; i++){
				//Stops if x-Distance between points get to big
            if(point_count>0 && i-max_points[point_count-1].x>MAX_POINT_DISTANCE_X) break;
            current_value = buff.ptr<uchar>(j)[i];
            if(max_value < current_value){
                max_value = current_value;
                x_max = i;
            }
        }
        if(max_value>MIN_GRADIENT_THRESHOLD) max_points[point_count++] = cvPoint(x_max, j-1); //point is not added if gradient to small
    }


#ifdef OUTPUT
    std::cout <<"Vector size: " << point_count << '\n'; //Just for debugging
#endif
	//Make sure fitLine does not throw exception
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

    //make sure max_point is the further away one (line output does some weird stuff...)
    Point2i max_pointb = cvPoint(max_line[2] + 200*max_line[0], max_line[3] + 200*max_line[1]);
    if(max_point.y>max_pointb.y) max_point = max_pointb;

    //estimated position of car
    Point2i car_position = cvPoint(buff.cols/2, buff.rows);

    //Calculate driving direction
    Vec2f car_direction(max_point.x-car_position.x, max_point.y-car_position.y);
    normalize(car_direction, car_direction, 1, NORM_L1);
    float direction = copysignf(std::acos(std::abs(car_direction[1])), car_direction[0]);
    direction = direction * 180 / M_PI;

    // Just for debugging
#ifdef DEBUG
	 Mat debug;
    cvtColor(buff, debug, CV_GRAY2RGB, 3);
    for(unsigned int i = 0; i<max_points.size(); i++){
        circle(debug, max_points[i], 1, CV_RGB(255,255,0), 1, 8, 0);
    }
	 std::stringstream ss;
	 ss << std::this_thread::get_id();
    namedWindow(ss.str(), WINDOW_AUTOSIZE);
    imshow(ss.str(), debug);
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

//Writes one character via Uart
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

//Map angle to level understood by the nanoboard
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

//Funktion run by threads
void threadMainLoop(Mat buff){
    float new_dir(0.0);
//	 int frame_time = time(NULL);

    while(1){
		  //Get frame and systemtime of getting it
        cap_mutex.lock();
        cap >> buff;
		  struct timeval frame_tp;
		  gettimeofday(&frame_tp, NULL);
        cap_mutex.unlock();
	
#ifdef OUTPUT	
		  std::clock_t c_start = std::clock(); 		  
#endif
		
		  //Calculate direction from frame
        new_dir = calcDirection(buff);

#ifdef OUTPUT
		  std::clock_t c_stop = std::clock();
		  std::cout << "Thread calculated frame for " << 1000.0*(c_stop-c_start) /CLOCKS_PER_SEC << " ms\n";
#endif

		  //Lock current_Direction
        cur_dir_mutex.lock();
		  //Check Direction was updated with newer frame
		  if(tp_last_frame.tv_sec < frame_tp.tv_sec ||
				(tp_last_frame.tv_sec==frame_tp.tv_sec && tp_last_frame.tv_usec<frame_tp.tv_usec)){
				//Uptade Direction and time
        		current_Direction = alpha * new_dir + (1-alpha) * current_Direction;
				tp_last_frame = frame_tp;
				//Notify main thread
				cond_var.notify_all();
		  }else{
#ifdef OUTPUT
				std::cout << "                                                  Frame got deleted\n";
#endif
		  }
        cur_dir_mutex.unlock();
    }
}


