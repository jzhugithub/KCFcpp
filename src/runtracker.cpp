#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

#include <dirent.h>

using namespace std;
using namespace cv;


/*
int main() {  
    // declares all required variables  
    //! [vars]  
    Rect roi;  
    Mat frame;  
    //! [vars]  
  
    // create a tracker object  
    Ptr<Tracker> tracker = Tracker::create("KCF");  
    //! [create]  
  
    // set input video  
    //! [setvideo]  
    std::string video = "E:\\demo1.avi";  
    VideoCapture cap(video);  
    //! [setvideo]  
  
    // get bounding box  
    //! [getframe]  
    cap >> frame;  
    //! [getframe]  
    //! [selectroi]选择目标roi以GUI的形式  
    roi = selectROI("tracker", frame);  
    //! [selectroi]  
  
    //quit if ROI was not selected  
    if (roi.width == 0 || roi.height == 0)  
        return 0;  
  
    // initialize the tracker  
    //! [init]  
    tracker->init(frame, roi);  
    //! [init]  
  
    // perform the tracking process  
    printf("Start the tracking process\n");  
    for (;; ) {  
        // get frame from the video  
        cap >> frame;  
  
        // stop the program if no more images  
        if (frame.rows == 0 || frame.cols == 0)  
            break;  
  
        // update the tracking result  
        //! [update]  
        tracker->update(frame, roi);  
        //! [update]  
  
        //! [visualization]  
        // draw the tracked object  
        rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);  
  
        // show image with the tracked object  
        imshow("tracker", frame);  
        //! [visualization]  
        //quit on ESC button  
        if (waitKey(1) == 27)  
            break;  
    }  
  
    return 0;  
}  
*/
Mat img, tmp;
Point2i pre_pt, end_pt, cur_pt;
void on_mouse(int event,int x,int y,int flags,void *ustc)
{
	cur_pt = Point2i(x,y);
	img.copyTo(tmp);
	
	if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))
	{
	}
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		pre_pt = Point2i(x,y);
	}
	if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		rectangle(tmp,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		end_pt = cur_pt;
		rectangle(img,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);
	}

	imshow("img",tmp);
}
int main(int argc, char* argv[]){

	if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = false;
	bool LAB = false;

	for(int i = 0; i < argc; i++){
		if ( strcmp (argv[i], "hog") == 0 )
			HOG = true;
		if ( strcmp (argv[i], "fixed_window") == 0 )
			FIXEDWINDOW = true;
		if ( strcmp (argv[i], "singlescale") == 0 )
			MULTISCALE = false;
		if ( strcmp (argv[i], "show") == 0 )
			SILENT = false;
		if ( strcmp (argv[i], "lab") == 0 ){
			LAB = true;
			HOG = true;
		}
		if ( strcmp (argv[i], "gray") == 0 )
			HOG = false;
	}
	
	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	// Frame readed
	Mat frame;

	// Tracker results
	Rect result;

  	VideoCapture cap("/home/ubuntu/my_workspace/KCFcpp/video/a544.avi");  
	if(!cap.isOpened()){cout<<"video open error"<<endl;return -1;}
	if (!cap.read(frame)){cout<<"video end"<<endl;return -1;}
	namedWindow("img",CV_WINDOW_NORMAL);
	frame.copyTo(img);
	setMouseCallback("img",on_mouse,0);
	imshow("img",img);
	waitKey(0);
	
	
  	// Read groundtruth like a dumb
  	float x1, y1, x2, y2, x3, y3, x4, y4;
	x1 = pre_pt.x;
	y1 = pre_pt.y;
	x2 = end_pt.x;
	y2 = end_pt.y;
	x3 = pre_pt.x;
	y3 = pre_pt.y;
	x4 = end_pt.x;
	y4 = end_pt.y;
	
	// Using min and max of X and Y for groundtruth rectangle
	float xMin =  min(x1, min(x2, min(x3, x4)));
	float yMin =  min(y1, min(y2, min(y3, y4)));
	float width = max(x1, max(x2, max(x3, x4))) - xMin;
	float height = max(y1, max(y2, max(y3, y4))) - yMin;

	// Frame counter
	int nFrames = 0;


	while (1){
		if (!cap.read(frame)){cout<<"video end"<<endl;break;}

		// First frame, give the groundtruth to the tracker
		if (nFrames == 0) {
			tracker.init( Rect(xMin, yMin, width, height), frame );
			rectangle( frame, Point( xMin, yMin ), Point( xMin+width, yMin+height), Scalar( 0, 255, 255 ), 1, 8 );
		}
		// Update
		else{
			result = tracker.update(frame);
			rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
		}
		nFrames++;

		if (!SILENT){
			imshow("img", frame);
			waitKey(1);
		}
	}
	waitKey(0);
}


/*

int main(int argc, char* argv[]){

	if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = true;
	bool LAB = false;

	for(int i = 0; i < argc; i++){
		if ( strcmp (argv[i], "hog") == 0 )
			HOG = true;
		if ( strcmp (argv[i], "fixed_window") == 0 )
			FIXEDWINDOW = true;
		if ( strcmp (argv[i], "singlescale") == 0 )
			MULTISCALE = false;
		if ( strcmp (argv[i], "show") == 0 )
			SILENT = false;
		if ( strcmp (argv[i], "lab") == 0 ){
			LAB = true;
			HOG = true;
		}
		if ( strcmp (argv[i], "gray") == 0 )
			HOG = false;
	}
	
	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	// Frame readed
	Mat frame;

	// Tracker results
	Rect result;

	// Path to list.txt
	ifstream listFile;
	string fileName = "images.txt";
  	listFile.open(fileName);

  	// Read groundtruth for the 1st frame
  	ifstream groundtruthFile;
	string groundtruth = "region.txt";
  	groundtruthFile.open(groundtruth);
  	string firstLine;
  	getline(groundtruthFile, firstLine);
	groundtruthFile.close();
  	
  	istringstream ss(firstLine);

  	// Read groundtruth like a dumb
  	float x1, y1, x2, y2, x3, y3, x4, y4;
  	char ch;
	ss >> x1;
	ss >> ch;
	ss >> y1;
	ss >> ch;
	ss >> x2;
	ss >> ch;
	ss >> y2;
	ss >> ch;
	ss >> x3;
	ss >> ch;
	ss >> y3;
	ss >> ch;
	ss >> x4;
	ss >> ch;
	ss >> y4; 

	// Using min and max of X and Y for groundtruth rectangle
	float xMin =  min(x1, min(x2, min(x3, x4)));
	float yMin =  min(y1, min(y2, min(y3, y4)));
	float width = max(x1, max(x2, max(x3, x4))) - xMin;
	float height = max(y1, max(y2, max(y3, y4))) - yMin;

	
	// Read Images
	ifstream listFramesFile;
	string listFrames = "images.txt";
	listFramesFile.open(listFrames);
	string frameName;


	// Write Results
	ofstream resultsFile;
	string resultsPath = "output.txt";
	resultsFile.open(resultsPath);

	// Frame counter
	int nFrames = 0;


	while ( getline(listFramesFile, frameName) ){
		frameName = frameName;

		// Read each frame from the list
		frame = imread(frameName, CV_LOAD_IMAGE_COLOR);

		// First frame, give the groundtruth to the tracker
		if (nFrames == 0) {
			tracker.init( Rect(xMin, yMin, width, height), frame );
			rectangle( frame, Point( xMin, yMin ), Point( xMin+width, yMin+height), Scalar( 0, 255, 255 ), 1, 8 );
			resultsFile << xMin << "," << yMin << "," << width << "," << height << endl;
		}
		// Update
		else{
			result = tracker.update(frame);
			rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
			resultsFile << result.x << "," << result.y << "," << result.width << "," << result.height << endl;
		}

		nFrames++;

		if (!SILENT){
			imshow("Image", frame);
			waitKey(1);
		}
	}
	resultsFile.close();

	listFile.close();

}
*/