/****************************************************************************/
/*           rapunzel Ver1.00 (2016/10/27)                 
/*                                                                             
/*                         Yoshimitsu MURAHASHI                                
/****************************************************************************/
/*  説明                                                                     
/*
/****************************************************************************/
/*  更新履歴                                                                 
/*  Ver 1.00 初期Version                                                     
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#ifdef M_PI
#define PI M_PI
#endif
#ifndef PI
#define PI 3.14159265358979323846
#endif



/*****************************************************************************/

#define XD_SIZE 10
#define  U_SIZE 10
#define  Y_SIZE 10

#define LIST_CNT 1000
#define RAP_CNT  100

typedef struct _Rap{
  double x;
  double y;
  double z;
  double th;
  CvScalar col;
} Rap;
typedef struct _RapList{
  int count;
  Rap *rap;
} RapList;

void RapSet( Rap *rap, double x, double y, double z, double th, CvScalar col ){
  rap->x   = x;
  rap->y   = y;
  rap->z   = z;
  rap->th  = th;
  int i;
  for( i = 0; i < 4 ; i ++ ){
    rap->col.val[i] = col.val[i];
  }
}
void RapRand( Rap *rap ){
    RapSet( rap,
	    rand()&1023,
	    511+(rand()&511),
	    (rand()&7)+2.0,
	    (rand()&7)/8.0*2*M_PI,
	    CV_RGB( 255, 128, 0 ) );
}

void RapMove( Rap *rap ){
  double zs = 64.0;
  rap->x -= 4.0/rap->z*(sin(rap->th));
  rap->y -= 16.0/rap->z;
  rap->z -= 0;
  rap->th+= M_PI/60.0*0.5;
  if( rap->y+zs/rap->z < 0 ){
    RapRand( rap );
  }

  //rap->col.val[2] = 32.0*sin( rap->th*10.0 ) + (255.0-32.0);
  //rap->col.val[1] = 16.0*sin( rap->th*10.0 ) + (128.0-16.0);
  //rap->col.val[0] = 0;
  rap->col.val[2] += 32/4*sin( rap->th*10.0 );
  rap->col.val[1] += 16/4*sin( rap->th*10.0 );
  rap->col.val[0] += 0;
}

void RapListMove( RapList *pRL ){
  int i;
  for( i = 0 ; i < pRL->count ; i ++ ){
    RapMove( &pRL->rap[i] );
  }

}

void RapListCreate( RapList *pRL, int count ){
  int i;
  pRL->count = count ;
  pRL->rap = (Rap*)malloc( sizeof(Rap) * count );
  for( i = 0 ; i < count ; i ++ ){
    RapRand( &pRL->rap[i] );
  }
} 

void RapListRelease( RapList *pRL ){
  pRL->count = 0;
  free( pRL->rap );
  pRL->rap = NULL;
} 

void DrawRap( IplImage *pImg, RapList *pRL ){
  int i;
  double zs = 64.0;
  for( i = 0 ; i < pRL->count ; i++ ){
    cvRectangle( pImg,
		 cvPoint( pRL->rap[i].x-zs/pRL->rap[i].z, pRL->rap[i].y-zs/pRL->rap[i].z ),
		 cvPoint( pRL->rap[i].x+zs/pRL->rap[i].z, pRL->rap[i].y+zs/pRL->rap[i].z ),
		 pRL->rap[i].col,//CV_RGB( 255, 128, 0 ),
		 3,//CV_FILLED,
		 8,
		 0 );
    
    
  }
} 

void SetBackground( IplImage *pImg, CvScalar c1, CvScalar c2 ){
  CvScalar c;
  int Height = pImg->height;
  int Width  = pImg->width;
  int x, y, p, i;
  for( y = 0 ; y < Height ; y ++ ){
    for( i = 0 ; i < 4 ; i ++ ){
      c.val[i] = ( c2.val[i] * y + c1.val[i] * (Height-y) ) / (double)Height;
    }
    for( x = 0 ; x < Width ; x ++ ){
      p = pImg->widthStep * y + x * 3 ;
      pImg->imageData[ p+0 ] = c.val[0];
      pImg->imageData[ p+1 ] = c.val[1];
      pImg->imageData[ p+2 ] = c.val[2];
    }
  }
}
int main( int argc , char *argv[] ){

  double xD[XD_SIZE];
  double u [XD_SIZE];
  double y [XD_SIZE];
  
  CvSize sz ;
  int i ;
  sz.width = 1024;
  sz.height = 512;
  //IplImage *pSrcImg = cvLoadImage("test.bmp",CV_LOAD_IMAGE_UNCHANGED  );
  IplImage *pSrcImg = cvCreateImage( sz, IPL_DEPTH_8U, 3 );
  IplImage *frm;

  if( pSrcImg == 0 ){
    printf("Error\n");
  }
  CvPoint ps, pe;
  CvCapture *cap=0;
  //cap = cvCreateCameraCapture( 0 ) ;
  //if( cap == 0 ){
  //  printf("Capture failed\n");
  // }
  //cvSetCaptureProperty( cap, CV_CAP_PROP_FRAME_WIDTH, 720 );
  //cvSetCaptureProperty( cap, CV_CAP_PROP_FRAME_HEIGHT, 480 );
  //cvNamedWindow("Cap",CV_WINDOW_AUTOSIZE);

  //CvVideoWriter *vw;
  //vw = cvCreateVideoWriter ("cap.avi", CV_FOURCC ('X', 'V', 'I', 'D'), 15, cvSize ((int) sz.width, (int) sz.height ),1);
  //vw = cvCreateVideoWriter ("cap.avi", CV_FOURCC ('M', 'J', 'P', 'G'), 15, cvSize ((int) sz.width, (int) sz.height ),1);
  //vw = cvCreateVideoWriter ("cap.avi", CV_FOURCC ('P', 'I', 'M', '1'), 15, cvSize ((int) sz.width, (int) sz.height ),1);
  //vw = cvCreateVideoWriter ("cap.avi", CV_FOURCC ('F', 'L', 'V', '1'), 15, cvSize ((int) sz.width, (int) sz.height ),1);
  //vw = cvCreateVideoWriter ("cap.avi", -1, 15, cvSize ((int) sz.width, (int) sz.height ),1);
  RapList RL;

  RapListCreate( &RL, RAP_CNT );
  for( i = 0 ; i < LIST_CNT; i ++ ){
    //cvSetZero( pSrcImg );
    SetBackground( pSrcImg, CV_RGB( 23, 55, 94 ), CV_RGB( 0, 0, 0 ) );
    DrawRap( pSrcImg, &RL );
    RapListMove( &RL );
    cvShowImage( "Show", pSrcImg );
    if( cvWaitKey(1000/24) >= 0 ) break;
    
  }
  cvDestroyWindow("Show");

  RapListRelease( &RL );


  return 0;
}
