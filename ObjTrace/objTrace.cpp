/****************************************************************************/
/*           objTrace Ver1.00 (2021/2/6)                 
/*                                                                             
/*                         Yoshimitsu MURAHASHI                                
/****************************************************************************/
/* About
/*
/****************************************************************************/
/* History
/*  Ver 1.00 First Version                                                     
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>


//#include "opencv/cv.h"
using namespace cv;

#define _USE_MATH_DEFINES

#define CAM_EN 0
#define VW_EN  0
#define VR_EN  1

#define FRAME_RATE 24

//#define MAX( A, B ) (((A)>(B))?(A):(B))
//#define MIN( A, B ) (((A)<(B))?(A):(B))

int main( int argc , char *argv[] ){

  // Read Image 
  IplImage *pPink   = cvLoadImage("pyachideka2.jpg",CV_LOAD_IMAGE_UNCHANGED  );

  // Create Window
  const char windowName[] = "Mon";
  cvNamedWindow(windowName,CV_WINDOW_FULLSCREEN);
  cvSetWindowProperty(windowName,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);

#if CAM_EN
  IplImage *pCam;
  CvCapture *pCap=0;
  pCap = cvCreateCameraCapture( 0 ) ;
  if( pCap == 0 ){
    printf("Capture failed\n");
  }
  cvSetCaptureProperty( pCap, CV_CAP_PROP_FRAME_HEIGHT, 360 );
  pCam = cvQueryFrame( pCap );
#if VW_EN
    CvSize sz = cvSize( pCam->width, pCam->height );
    CvVideoWriter *pVW;
    pVW = cvCreateVideoWriter ("video.avi",CV_FOURCC ('D', 'I', 'V', 'X'), FRAME_RATE, sz,1);
#endif //VW_EN
#endif


  // Draw Loop
  while (0){

#if CAM_EN
    cvQueryFrame( pCap );
    pCam = cvQueryFrame( pCap );
    cvShowImage("Mon", pCam );
#if VW_EN
    cvWriteFrame( pVW, pCam );
#endif //VW_EN

    
#else
    // Draw Main
    cvShowImage("Mon", pPink );
#endif
    
    // CTRL+C detection 
    if( cvWaitKey(1) >= 0 ){
      break;
    }
  }

#if CAM_EN
  cvReleaseCapture( &pCap );
  cvReleaseImage( &pCam );
#if VW_EN
  cvReleaseVideoWriter( &pVW );
#endif //VW_EN
#endif

  
  // Destroy Window
  cvDestroyWindow("Mon");

  // Release Image Memory
  cvReleaseImage( &pPink );
  
#if VR_EN
  VideoCapture video("video.avi");
  if( !video.isOpened() ){
    fprintf(stderr,"ERROR\n");
    return (-1);
  }else{
    fprintf(stderr,"OK\n");
  }
  Mat edges;
  namedWindow("edges",1);
  Mat frame;
  int frame_cnt = video.get( CV_CAP_PROP_FPS );
  //cv::Ptr<cv::BackgroundSubtractor> bgfs = cv::bgsegm::createBackgroundSubtractorGSOC();
 
  printf("frame_cnt = %d\n",frame_cnt);
  //for( int i=0; i< frame_cnt ; i++ ){

  while(1){
    Mat frame, foreGroundMask, segm;
    video >> frame ;
    if( frame.empty() ){
      break;
    }

    //bgfs->apply(frame, foreGroundMask);
    //frame.convertTo(segm, 0, 0.5);
    //cv::add(frame, cv::Scalar(100, 100, 0), segm, foreGroundMask);
    
    imshow("edges",frame);
    //imshow("edges",fgmask);
    //imshow("edges",segm);
    if(waitKey(1000/24) >= 0)break;
  }
#endif

  return 0;
}
