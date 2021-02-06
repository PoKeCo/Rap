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
#include "opencv2/opencv.hpp"

#define _USE_MATH_DEFINES

#define CAM_EN 1

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
#endif


  // Draw Loop
  while (1){

#if CAM_EN
    cvQueryFrame( pCap );
    pCam = cvQueryFrame( pCap );
    cvShowImage("Mon", pCam );
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
#endif
  
  // Destroy Window
  cvDestroyWindow("Mon");

  // Release Image Memory
  cvReleaseImage( &pPink );
  
  return 0;
}
