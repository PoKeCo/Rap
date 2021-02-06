/****************************************************************************/
/*           rapunzel Ver1.00 (2016/10/27)                 
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
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/videoio/videoio_c.h>

#ifdef M_PI
#define PI M_PI
#endif
#ifndef PI
#define PI 3.14159265358979323846
#endif

//#define MAX( A, B ) (((A)>(B))?(A):(B))
//#define MIN( A, B ) (((A)<(B))?(A):(B))

/*****************************************************************************/
#define frand() ((double)rand()/RAND_MAX)
#define LIST_CNT 1000
#define RAP_CNT  200


#define Z_VAR_CNT 16

typedef struct _Rap{
  double x;
  double y;
  double z;
  double a;  
  int iz;
  double th;
  double dth;  
  CvScalar col;
} Rap;
typedef struct _RapList{
  int count;
  Rap *rap;
} RapList;

void RapSet( Rap *rap, double x, double y, double z, int iz, double th, double dth, CvScalar col ){
  rap->x   = x;
  rap->y   = y;
  rap->z   = z;
  rap->iz  = iz;
  rap->th  = th;
  rap->dth = M_PI/60.0*0.5 ;
  int i;
  for( i = 0; i < 4 ; i ++ ){
    rap->col.val[i] = col.val[i];
  }
}
void RapRand( Rap *rap, int i ){
  int iz;
  if ( i >= 0 ){
    iz = ( i * 7.0 )/(double)RAP_CNT ;
  } else {
    iz = rap->iz;
  }
  //  int iz = (rand()&7);
  RapSet( rap,
	  rand()&1023,
	  511+(rand()&511),
	  iz+2.0,
	  iz,
	  (rand()&7)/8.0*2*M_PI,
	  frand()*0.5+0.5 * M_PI/60.0*0.5,
	  CV_RGB( 255, 128, 0 ) );
}

void RapMove( Rap *rap ){
  double zs = 64.0;
  rap->x -= 4.0/rap->z*(sin(rap->th));
  rap->y -= 16.0/rap->z * 0.25;
  rap->z -= 0;
  //rap->th+= M_PI/60.0*0.5;
  rap->th += rap->dth ;
  if( rap->y+4*zs/rap->z < 0 ){
    RapRand( rap, -1 );
  }

  //rap->col.val[2] = 32.0*sin( rap->th*10.0 ) + (255.0-32.0);
  //rap->col.val[1] = 16.0*sin( rap->th*10.0 ) + (128.0-16.0);
  //rap->col.val[0] = 0;
  rap->col.val[2] += 32/4*sin( rap->th*10.0 );
  rap->col.val[1] += 16/4*sin( rap->th*10.0 );
  rap->col.val[0] += 0;
  rap->a = 0.9 + 0.05 * sin( rap->th * 15 );
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
    RapRand( &pRL->rap[i], i );
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
  //for( i = 0 ; i < pRL->count ; i++ ){
  for( pRL->count-1 ; i>=0 ; i-- ){
    cvRectangle( pImg,
		 cvPoint( pRL->rap[i].x-zs/pRL->rap[i].z, pRL->rap[i].y-zs/pRL->rap[i].z ),
		 cvPoint( pRL->rap[i].x+zs/pRL->rap[i].z, pRL->rap[i].y+zs/pRL->rap[i].z ),
		 pRL->rap[i].col,//CV_RGB( 255, 128, 0 ),
		 3,//CV_FILLED,
		 8,
		 0 );
    
    
  }
} 

void Paste( IplImage *pImg, IplImage *pPat, int x, int y, double a ){
  int Ph = pPat->height;
  int Pw = pPat->width;
  int Ih = pImg->height;
  int Iw = pImg->width;
  int Pp, Px, Py;
  int Ip, Ix, Iy;

  int Pxs = ( -x > 0 )?-x:0;
  int Pys = ( -y > 0 )?-y:0;
  
  int Pxe = ( x+Pw > Iw )?(Iw-x):Pw;
  int Pye = ( y+Ph > Ih )?(Ih-y):Ph;
  int IR, IG, IB ;
  int PR, PG, PB ;
  double X,Y,Z;

  
  for( Py = Pys ; Py < Pye ; Py ++ ){
    for( Px = Pxs ; Px < Pxe ; Px ++ ){
      Ix = Px + x;
      Iy = Py + y;
      Pp = pPat->widthStep * Py + Px * 3 ;
      Ip = pImg->widthStep * Iy + Ix * 3 ;
      PR = (unsigned char)pPat->imageData[ Pp+2 ];
      PG = (unsigned char)pPat->imageData[ Pp+1 ];
      PB = (unsigned char)pPat->imageData[ Pp+0 ];
      if( !( PR==0 && PG==0 && PB==0 ) ){
	IR = (unsigned char)pImg->imageData[ Ip+2 ];
	IG = (unsigned char)pImg->imageData[ Ip+1 ];
	IB = (unsigned char)pImg->imageData[ Ip+0 ];
	X = (( 3.0*PR + 6.0*PG + 1.0*PB )/10.0 ) /255.0;
	//Y =  X*2.0;
	//Z =  MAX(0, MIN( 1.0, 1.0-X));
	//Y =  MAX(0, MIN( 1.0, 1.0-Z));
	/*
	pImg->imageData[ Ip+0 ] = (int)MIN(255, ( PB*a )*Y + IB*Z );
	pImg->imageData[ Ip+1 ] = (int)MIN(255, ( PG*a )*Y + IG*Z );
	pImg->imageData[ Ip+2 ] = (int)MIN(255, ( PR*a )*Y + IR*Z );*/
	pImg->imageData[ Ip+0 ] = (int)MAX(PB*a,IB);
	pImg->imageData[ Ip+1 ] = (int)MAX(PG*a,IG);
	pImg->imageData[ Ip+2 ] = (int)MAX(PR*a,IR);
	
	/*
	pImg->imageData[ Ip+0 ] = (int)MIN(255, ( PB*a )*Y + IB*(1.0));
	pImg->imageData[ Ip+1 ] = (int)MIN(255, ( PG*a )*Y + IG*(1.0));
	pImg->imageData[ Ip+2 ] = (int)MIN(255, ( PR*a )*Y + IR*(1.0));
	*/
      }
    }
  }
}

void DrawRap2( IplImage *pImg, RapList *pRL, IplImage **pPats ){
  int i;
  double zs = 64.0;
  for( i = 0 ; i < pRL->count ; i++ ){
    Paste( pImg, pPats[ pRL->rap[i].iz ] , pRL->rap[i].x, pRL->rap[i].y, 1.0 );
  }
} 

void DrawRap3( IplImage *pImg, RapList *pRL, IplImage **pPats1,IplImage **pPats2  ){
  int i;
  double zs = 64.0;
  //for( i = 0 ; i < pRL->count ; i++ ){
  for( i=pRL->count-1 ; i>=0 ; i-- ){
    if( i & 1 ){
      Paste( pImg, pPats1[ pRL->rap[i].iz ] , pRL->rap[i].x, pRL->rap[i].y, pRL->rap[i].a );
    }else{
      Paste( pImg, pPats2[ pRL->rap[i].iz ] , pRL->rap[i].x, pRL->rap[i].y, pRL->rap[i].a );
    }
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

  CvSize sz ;
  int i, j ;
  sz.width = 1024;
  sz.height = 512;
  IplImage *pPink   = cvLoadImage("pink2.bmp",CV_LOAD_IMAGE_UNCHANGED  );
  IplImage *pBlue   = cvLoadImage("blue2.bmp",CV_LOAD_IMAGE_UNCHANGED  );
  //IplImage *pBlue   = cvLoadImage("test.bmp",CV_LOAD_IMAGE_UNCHANGED  );

  IplImage **pPinks = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );
  IplImage **pBlues = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );

  for( i = 0 ; i < Z_VAR_CNT ; i++ ){
    pPinks[i] = cvCreateImage( cvSize(pPink->width*2.0/(i+3.0), pPink->height*2.0/(i+3.0)), IPL_DEPTH_8U, 3 );
    cvResize( pPink, pPinks[i], CV_INTER_LINEAR );
    pBlues[i] = cvCreateImage( cvSize(pPink->width*2.0/(i+3.0), pPink->height*2.0/(i+3.0)), IPL_DEPTH_8U, 3 );
    cvResize( pBlue, pBlues[i], CV_INTER_LINEAR );
  }
  
  IplImage *pSrcImg = cvCreateImage( sz, IPL_DEPTH_8U, 3 );
  IplImage *frm;

  if( pSrcImg == 0 ){
    printf("Error\n");
  }
  CvPoint ps, pe;
  
  //CvCapture *cap=0;
  //cap = cvCreateCameraCapture( 0 ) ;
  //if( cap == 0 ){
  //  printf("Capture failed\n");
  //}
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

    //DrawRap( pSrcImg, &RL );
    //DrawRap2( pSrcImg, &RL, pPinks );
    DrawRap3( pSrcImg, &RL, pPinks, pBlues );
    RapListMove( &RL );
    cvShowImage( "Show", pSrcImg );
    if( cvWaitKey(1000/24) >= 0 ) break;
    
  }
  cvDestroyWindow("Show");

  RapListRelease( &RL );

  cvReleaseImage( &pSrcImg );
  cvReleaseImage( &pPink );
  cvReleaseImage( &pBlue );
  for( i = 0 ; i < Z_VAR_CNT ; i++ ){
    cvReleaseImage( &pPinks[i] );
    cvReleaseImage( &pBlues[i] );
  }
  free( pPinks );
  free( pBlues );

  return 0;
}
