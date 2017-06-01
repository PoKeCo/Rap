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
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include "stdafx.h"
#include <string.h>



#ifdef M_PI
#define PI M_PI
#endif
#ifndef PI
#define PI 3.14159265358979323846
#endif

//#define MAX( A, B ) (((A)>(B))?(A):(B))
//#define MIN( A, B ) (((A)<(B))?(A):(B))



#if _WIN32 || _WIN64
#include "windows.h"
#endif //_WIN32 || _WIN64

#define FULL_SCREEN

/*****************************************************************************/
#define frand() ((double)rand()/RAND_MAX)
#define LIST_CNT 1000


#define Z_VAR_CNT 10

#if 1
#define IMG_HEIGHT 1080
#define IMG_WIDTH  1920
#define RAP_CNT  400
#else
#define IMG_HEIGHT 512
#define IMG_WIDTH  1024
#define RAP_CNT  200
#endif

#define FRAME_CNT (24*210)
//#define FRAME_CNT (240)
#define FRAME_RATE 24

#define CAM_EN 1
#define VW_EN 1
#define MON_EN 1

typedef struct _Rap{
	double x;
	double y;
	double z;
	double a;  
	double v;  
	int iz;
	double th;
	double dth; 
	int flag;
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
	rap->v   = 0;

	rap->dth = M_PI/60.0*0.5 ;
	int i;
	for( i = 0; i < 4 ; i ++ ){
		rap->col.val[i] = col.val[i];
	}
}
void RapRand( Rap *rap, int i, int cnt ){
	int iz;
	if ( i >= 0 ){
		iz =(int)(  ( i * Z_VAR_CNT )/(double)cnt );
	} else {
		iz = rap->iz;
	}

	RapSet( rap,
		IMG_WIDTH * (0.05+frand()*0.9),
		IMG_HEIGHT* frand()+IMG_HEIGHT,
		iz+2.0,
		iz,
		frand()*2*M_PI,
		frand()*0.5+0.5 * M_PI/60.0*0.5,
		CV_RGB( 255, 128, 0 ) );
}

void RapMove( Rap *rap ){
	double zs = 64.0;
	if( rap->flag ){
		rap->x -= 4.0/rap->z*(sin(rap->th));
		rap->y -= 16.0/rap->z * 0.25;
		rap->z -= 0;
		//rap->th+= M_PI/60.0*0.5;
		rap->th += rap->dth ;	  
	}else{  
		rap->v = rap->v + 2.0;	   
		rap->x -= rap->dth-0.5 * M_PI/60.0*0.5;
		rap->y = rap->y + rap->v ;
		if( rap->y > 1080 ){
			rap->y = rap->y - rap->v;
			rap->v = - 0.8 * rap->v ; 
		}
		rap->z -= 0;
	}
	if( rap->y+4*zs/rap->z < 0 ){
		//rap->flag = 0;
		RapRand( rap, -1, 1 );
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
		RapRand( &pRL->rap[i], i, count );
		pRL->rap[i].flag= 1;
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
	for( i=pRL->count-1 ; i>=0 ; i-- ){
		cvRectangle( pImg,
			cvPoint( (int)(pRL->rap[i].x-zs/pRL->rap[i].z), (int)(pRL->rap[i].y-zs/pRL->rap[i].z) ),
			cvPoint( (int)(pRL->rap[i].x+zs/pRL->rap[i].z), (int)(pRL->rap[i].y+zs/pRL->rap[i].z) ),
			pRL->rap[i].col,//CV_RGB( 255, 128, 0 ),
			3,//CV_FILLED,
			8,
			0 );


	}
} 

void PasteEz( IplImage *pImg, IplImage *pPat, int x, int y, double a ){
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
				pImg->imageData[ Ip+0 ] = (int)(PB*a);
				pImg->imageData[ Ip+1 ] = (int)(PG*a);
				pImg->imageData[ Ip+2 ] = (int)(PR*a);
			}
		}
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
	double X;

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
#if 0
				pImg->imageData[ Ip+0 ] = (int)MAX(PB*a,IB);
				pImg->imageData[ Ip+1 ] = (int)MAX(PG*a,IG);
				pImg->imageData[ Ip+2 ] = (int)MAX(PR*a,IR);
#else
				pImg->imageData[ Ip+0 ] = (int)((X>0.3)?PB*a:MIN(255,IB+PB) );
				pImg->imageData[ Ip+1 ] = (int)((X>0.3)?PG*a:MIN(255,IG+PG) );
				pImg->imageData[ Ip+2 ] = (int)((X>0.3)?PR*a:MIN(255,IR+PR) );
#endif

			}
		}
	}
}

void DrawRap2( IplImage *pImg, RapList *pRL, IplImage **pPats ){
	int i;
	double zs = 64.0;
	for( i = 0 ; i < pRL->count ; i++ ){
		Paste( pImg, pPats[ pRL->rap[i].iz ] , (int)pRL->rap[i].x, (int)pRL->rap[i].y, 1.0 );
	}
} 

void DrawRap2Ez( IplImage *pImg, RapList *pRL, IplImage **pPats ){
	int i;
	double zs = 64.0;
	for( i = 0 ; i < pRL->count ; i++ ){
		PasteEz( pImg, pPats[ pRL->rap[i].iz ] , (int)pRL->rap[i].x, (int)pRL->rap[i].y, 1.0 );
	}
} 

void DrawRap3( IplImage *pImg, RapList *pRL, IplImage **pPats1,IplImage **pPats2  ){
	int i;
	double zs = 64.0;
	//for( i = 0 ; i < pRL->count ; i++ ){
	for( i=pRL->count-1 ; i>=0 ; i-- ){
		if( i & 1 ){
			Paste( pImg, pPats1[ pRL->rap[i].iz ] , (int)pRL->rap[i].x, (int)pRL->rap[i].y, pRL->rap[i].a );
		}else{
			Paste( pImg, pPats2[ pRL->rap[i].iz ] , (int)pRL->rap[i].x, (int)pRL->rap[i].y, pRL->rap[i].a );
		}
	}
} 

void SetBackground( IplImage *pImg, CvScalar c1, CvScalar c2, double a ){
	CvScalar c;
	int Height = pImg->height;
	int Width  = pImg->width;
	int x, y, p, i;
	//CvScalar d = CV_RGB(128,64,64);
	CvScalar d = CV_RGB(64,32,32);
	a = MIN( 1.0, a );
	for( y = 0 ; y < Height ; y ++ ){
		for( i = 0 ; i < 4 ; i ++ ){
			c.val[i] = ( c2.val[i] * y + c1.val[i] * (Height-y) ) / (double)Height;
			c.val[i] += d.val[i]*a;
		}
		for( x = 0 ; x < Width ; x ++ ){
			p = pImg->widthStep * y + x * 3 ;
			pImg->imageData[ p+0 ] = (char)c.val[0];
			pImg->imageData[ p+1 ] = (char)c.val[1];
			pImg->imageData[ p+2 ] = (char)c.val[2];
		}
	}
}

int MouseRestart= 0;
void MouseCallback(int event, int x, int y, int flags, void* param){
   switch (event){
   case cv::EVENT_LBUTTONDOWN:
     MouseRestart = 1;
   }
}
int sub_main( int argc , char *argv[] ){

	CvSize sz = cvSize( IMG_WIDTH, IMG_HEIGHT );
	int i, j, loop_exit ;
	IplImage *pPink   = cvLoadImage("pink1.bmp",CV_LOAD_IMAGE_UNCHANGED  );
	IplImage *pBlue   = cvLoadImage("blue1.bmp",CV_LOAD_IMAGE_UNCHANGED  );
	IplImage *pDeka   = cvLoadImage("pyachideka2.jpg",CV_LOAD_IMAGE_UNCHANGED  );
  IplImage *pIshi   = cvLoadImage("ishikirin.jpg",CV_LOAD_IMAGE_UNCHANGED  );
	//IplImage *pBlue   = cvLoadImage("test.bmp",CV_LOAD_IMAGE_UNCHANGED  );

	
	IplImage **pPinks = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );
	IplImage **pBlues = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );
	IplImage **pDekas = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );
	int Pw, Ph, Bw, Bh, Dw, Dh;
	for( i = 0 ; i < Z_VAR_CNT ; i++ ){
		Pw = (int)(pPink->width *2.0/(i+3.0));
		Ph = (int)(pPink->height*2.0/(i+3.0));
		pPinks[i] = cvCreateImage( cvSize(Pw, Ph), IPL_DEPTH_8U, 3 );
		cvResize( pPink, pPinks[i], CV_INTER_LINEAR );
		Bw = (int)(pBlue->width *2.0/(i+3.0));
		Bh = (int)(pBlue->height*2.0/(i+3.0));
		pBlues[i] = cvCreateImage( cvSize(Bw, Bh), IPL_DEPTH_8U, 3 );
		cvResize( pBlue, pBlues[i], CV_INTER_LINEAR );
    if( !(i & 1) ){
		  Dw = (int)(pDeka->width *2.0/(i+3.0));
		  Dh = (int)(pDeka->height*2.0/(i+3.0));
		  pDekas[i] = cvCreateImage( cvSize(Dw, Dh), IPL_DEPTH_8U, 3 );
		  cvResize( pDeka, pDekas[i], CV_INTER_LINEAR );
    }else{
		  Dw = (int)(pIshi->width *2.0/(i+3.0));
		  Dh = (int)(pIshi->height*2.0/(i+3.0));
		  pDekas[i] = cvCreateImage( cvSize(Dw, Dh), IPL_DEPTH_8U, 3 );
		  cvResize( pIshi, pDekas[i], CV_INTER_LINEAR );
    }
	}
#if MON_EN
#if _WIN32 || _WIN64
	//cvNamedWindow("Mon",CV_WINDOW_AUTOSIZE);
  LPCSTR windowName = "Mon";
  cvNamedWindow(windowName,CV_WINDOW_FULLSCREEN);
  cvSetWindowProperty(windowName,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
#ifdef FULL_SCREEN
  // windowNameを持つウィンドウを検索
  HWND windowHandle = ::FindWindowA(NULL, windowName);

  if (NULL != windowHandle) {

	  // ウィンドウスタイル変更（メニューバーなし、最前面）
	  SetWindowLongPtr(windowHandle,  GWL_STYLE, WS_POPUP);
	  SetWindowLongPtr(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST);

	  // 最大化する
	  ShowWindow(windowHandle, SW_MAXIMIZE);
	  cvSetWindowProperty(windowName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	  // ディスプレイサイズを取得
	  int mainDisplayWidth = GetSystemMetrics(SM_CXSCREEN);
	  int mainDisplayHeight = GetSystemMetrics(SM_CYSCREEN);

	  // クライアント領域をディスプレーに合わせる
	  SetWindowPos(windowHandle, NULL,
		  0, 0, mainDisplayWidth, mainDisplayWidth,
		  SWP_FRAMECHANGED | SWP_NOZORDER);
  }
#endif // FULL_SCREEN
#else  //_WIN32 || _WIN64
  const char windowName[] = "Mon";
  cvNamedWindow(windowName,CV_WINDOW_FULLSCREEN);
  cvSetWindowProperty(windowName,CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
#endif //_WIN32 || _WIN64
#endif //MON_EN
  
    cvSetMouseCallback("Mon", MouseCallback, (void *)NULL);
#if CAM_EN
	IplImage *pCam;
	CvCapture *pCap=0;
	pCap = cvCreateCameraCapture( 0 ) ;
	if( pCap == 0 ){
		printf("Capture failed\n");
	}
	//cvSetCaptureProperty( pCap, CV_CAP_PROP_FRAME_WIDTH, 640 );
	//cvSetCaptureProperty( pCap, CV_CAP_PROP_FRAME_HEIGHT, 480 );
	cvSetCaptureProperty( pCap, CV_CAP_PROP_FRAME_HEIGHT, 360 );
	pCam = cvQueryFrame( pCap );
	IplImage **pCams  = (IplImage**)malloc(sizeof(IplImage*) * Z_VAR_CNT );
	int Cw, Ch;
	for( i = 0 ; i < Z_VAR_CNT ; i++ ){
		Cw = (int)(pCam ->width *2.0/(i+3.0));
		Ch = (int)(pCam ->height*2.0/(i+3.0));
		pCams [i] = cvCreateImage( cvSize(Cw, Ch), IPL_DEPTH_8U, 3 );
		//pCams [i] = cvCreateImage( cvSize(640, 480), IPL_DEPTH_8U, 3 );
		cvResize( pCam, pCams[i], CV_INTER_LINEAR );
	}
#endif //CAM_EN

	IplImage *pSrcImg = cvCreateImage( sz, IPL_DEPTH_8U, 3 );

	if( pSrcImg == 0 ){
		printf("Error\n");
	}
  
  int video_cnt=0;
  char VideoName[256];
  //////////////////////////////////////////////////////////////////////////////////////////////////
  loop_exit = 0;
  while( !loop_exit ){
#if VW_EN
    CvVideoWriter *pVW;
    sprintf( VideoName, "rapunzel%05d.avi",video_cnt);
    pVW = cvCreateVideoWriter (VideoName, CV_FOURCC ('D', 'I', 'V', 'X'), FRAME_RATE, sz,1);
#endif //VW_EN

    RapList RL, RLD, RLC;

    RapListCreate( &RL, RAP_CNT );
    RapListCreate( &RLD, Z_VAR_CNT );
    RapListCreate( &RLC, Z_VAR_CNT );
    RLD.rap[0].z  = 7;
    RLD.rap[0].iz = 7;
    double a;
    for( i = 0 ; i < FRAME_CNT; i ++ ){
      //cvSetZero( pSrcImg );
      a = ((double)i)/FRAME_CNT;
      //printf("a=%lf\n",a);
      SetBackground( pSrcImg, CV_RGB( 23, 55, 94 ), CV_RGB( 0, 0, 0 ), a );


#if CAM_EN
      cvQueryFrame( pCap );
      pCam = cvQueryFrame( pCap );
      for( j = 0 ; j < Z_VAR_CNT ; j++ ){
        cvResize( pCam, pCams[j], CV_INTER_LINEAR );
      }
      //Paste( pSrcImg, pCam, 0 , 0, 1.0 );
      DrawRap2Ez( pSrcImg, &RLC, pCams );
#endif //CAM_EN
      DrawRap2Ez( pSrcImg, &RLD, pDekas );

      //DrawRap( pSrcImg, &RL );
      //DrawRap2( pSrcImg, &RL, pPinks );
      DrawRap3( pSrcImg, &RL, pPinks, pBlues );
      //DrawRap2( pSrcImg, &RLD, pDekas );

      // Update State 
      RapListMove( &RL );
      RapListMove( &RLD );
      RapListMove( &RLC );
#if VW_EN
      cvWriteFrame( pVW, pSrcImg );
#endif //VW_EN
#if MON_EN
      cvShowImage( "Mon", pSrcImg );
#endif //MON_EN

      //if( cvWaitKey(1000/24) >= 0 ) break;
      if( cvWaitKey(1) >= 0 ){
        loop_exit=1;
        break;
      }
      if( MouseRestart ){
        MouseRestart=0;
        break;
      }


    }
	  RapListRelease( &RL  );
	  RapListRelease( &RLD );
    RapListRelease( &RLC );
#if VW_EN
    video_cnt ++ ;
	  cvReleaseVideoWriter( &pVW );
#endif //VW_EN

  }//loop_exit
  //////////////////////////////////////////////////////////////////////////////////////////////////

#if MON_EN
	cvDestroyWindow("Mon");
#endif

	cvReleaseImage( &pSrcImg );

	cvReleaseImage( &pPink );
	cvReleaseImage( &pBlue );
	cvReleaseImage( &pDeka );
  cvReleaseImage( &pIshi );
	//cvReleaseImage( &pCam  );
	for( i = 0 ; i < Z_VAR_CNT ; i++ ){
		cvReleaseImage( &pPinks[i] );
		cvReleaseImage( &pBlues[i] );
		cvReleaseImage( &pDekas[i] );
	}
	free( pPinks );
	free( pBlues );
	free( pDekas );


#if CAM_EN
	cvReleaseCapture( &pCap );
	for( i = 0 ; i < Z_VAR_CNT ; i++ ){
		cvReleaseImage( &pCams [i] );	}
	free( pCams  );  
#endif//CAM_EN

	return 0;
}
