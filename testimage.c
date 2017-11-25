
#include "cv.h"
#include "highgui.h"
 
int main( int argc, char** argv )
{
    IplImage* pImg; //声明IplImage指针
 
    //载入图像
    if((pImg = cvLoadImage(argv[1],1)) != 0 )
    {
        cvNamedWindow( "Image", 1 );//创建窗口
        cvShowImage( "Image", pImg );//显示图像
 
        cvWaitKey(0); //等待按键
 
        cvDestroyWindow( "Image" );//销毁窗口
        cvSaveImage(argv[2],pImg,0);
        cvReleaseImage( &pImg ); //释放图像
        return 0;
    }
  /*CvCapture* cap = cvCaptureFromFile("image/image_%02d.png");
 cvNamedWindow( "Image", 1 );//创建窗口
   if(cap){
	while(1){
    	IplImage *src = cvQueryFrame(cap);
     cvShowImage( "Image", src );
cvWaitKey(0);
	}
    }*/
    return -1;
}
