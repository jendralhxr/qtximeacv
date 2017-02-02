// compile against Ximea M3 API and OpenCV
// g++ test.cpp -lm3api `pkg-config --libs opencv`

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <m3api/xiApi.h> 
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define EXPOSURE 4000 // us

struct timeval capture_start, capture_stop, render_start, render_stop;
float fps_capture, fps_render;
unsigned int counter_capture, counter_render;

// spawn the spiderlings
int child_capture, child_render;
	               
int main(int argc, char **argv){
	void *buffer = mmap(NULL, sizeof(char)*IMAGE_HEIGHT*IMAGE_WIDTH, \
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	// capture thread
	child_capture = fork();
	if (!child_capture){
		HANDLE handle = NULL;
		// ximea stuff
		if(xiOpenDevice(0, &handle) != XI_OK) exit(1);
		if (argc>=2) xiSetParamInt(handle, XI_PRM_EXPOSURE, atoi(argv[1])); // us
		else xiSetParamInt(handle, XI_PRM_EXPOSURE, EXPOSURE); // us
		xiSetParamFloat(handle, XI_PRM_GAIN, 7.4); // -3.5 to 7.4
		//xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24); // simply cause I can
		xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8); // faster
		xiSetParamInt(handle, XI_PRM_OFFSET_X, 0);
		xiSetParamInt(handle, XI_PRM_OFFSET_Y, 0);
		xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
		xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
		//xiSetParamInt(handle, XI_PRM_AUTO_WB, 0);
		//xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
		//xiSetParamInt(handle, XI_PRM_FRAMERATE, 90);// Requested fps
		
		XI_RETURN stat = XI_OK;
		XI_IMG image;
		image.size = sizeof(XI_IMG);
		image.bp = NULL;
		image.bp_size = 0;
		xiStartAcquisition(handle);
		
		gettimeofday(&capture_start, NULL);
		while(1){
			gettimeofday(&capture_stop, NULL);
			if ((capture_stop.tv_sec-capture_start.tv_sec) > 1){
				fps_capture= counter_capture / 
					((capture_stop.tv_sec-capture_start.tv_sec) + \
					 ((capture_stop.tv_usec-capture_start.tv_usec)/1e6));	
				printf("capture: %f\n",fps_capture);
				counter_capture= 0;
				gettimeofday(&capture_start, NULL);
			}
			// capture routine starts
			xiGetImage(handle, 1000, &image); // capture, timeout in us
			memcpy(buffer, image.bp, IMAGE_WIDTH*IMAGE_HEIGHT);
			// capture routine ends
			counter_capture++;
			}
		}
		
	// render thread
	child_render = fork();
	if (!child_render){
		// opencv stuff    
		using namespace cv;
		Mat *frame, *frame_color; // for raw (gbgr Bayer) and color
	
		frame = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
		frame_color= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);           
		gettimeofday(&render_start, NULL);
		while(1){
			gettimeofday(&render_stop, NULL);
			if ((render_stop.tv_sec-render_start.tv_sec) > 1){
				fps_render= counter_render / 
					((render_stop.tv_sec-render_start.tv_sec) + \
					 ((render_stop.tv_usec-render_start.tv_usec)/1e6));	
				printf("render: %f\n",fps_render);
				counter_render= 0;
				gettimeofday(&render_start, NULL);
				}
			//render routine starts
			memcpy(frame->data, buffer, IMAGE_WIDTH*IMAGE_HEIGHT);
			cvtColor(*frame, *frame_color, CV_BayerBG2BGR);
			imshow("display", *frame_color);
			waitKey(1);
			//render routine ends	
			
			counter_render++;
			usleep(20000); // limit render rate to 50 fps
			}

		}
	
	sleep(30);
//	wait(NULL);
	kill(child_render, SIGKILL);
	kill(child_capture, SIGKILL);
	}
