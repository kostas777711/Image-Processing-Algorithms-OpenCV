#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main (int argc, char** argv )
{
         //exit key
        int c;  
        //Initialise images that we will need   
        IplImage *gray =0;
        IplImage *rgb =0;
        IplImage *hsv =0;
        IplImage *grayr =0;
        IplImage *grayg =0;
        IplImage *grayb =0;
        IplImage *grayh =0;
        IplImage *grays =0;
        IplImage *grayv =0;
        //initialise capture structure for incoming video stream
        CvCapture* capture=cvCaptureFromCAM(0);
        //initialise the two windows that we will use for output
        cvNamedWindow("rgb",1);
        cvNamedWindow("gray",1);
        cvNamedWindow("hsv",1);
        cvNamedWindow("grayr",1);
        cvNamedWindow("grayg",1);
        cvNamedWindow("grayb",1);
        cvNamedWindow("grayh",1);
        cvNamedWindow("grays",1);
        cvNamedWindow("grayv",1);
       
        
        while(1)   //main infinite loop
        {
                //initialise the image which stores the incoming stream
                IplImage *src = 0;
                src=cvQueryFrame(capture);
                
                if( !src )    //if there is no frame exit the while(1)
                break;
                
                if( !gray )    //if there is no image gray, set the format of the secondary images
                {
                    gray=cvCreateImage(cvGetSize(src),8,1);
                    rgb=cvCreateImage(cvGetSize(src),8,3);
                    hsv=cvCreateImage(cvGetSize(src),8,3);
                    
                    grayr=cvCreateImage(cvGetSize(src),8,1);
                    grayg=cvCreateImage(cvGetSize(src),8,1);
                    grayb=cvCreateImage(cvGetSize(src),8,1);
                    grayh=cvCreateImage(cvGetSize(src),8,1);
                    grays=cvCreateImage(cvGetSize(src),8,1);
                    grayv=cvCreateImage(cvGetSize(src),8,1);
                    
                    
                }
                
                cvCopy(src, rgb,0);
                cvCvtColor(rgb,gray,CV_BGR2GRAY );
                cvCvtColor(rgb,hsv,CV_BGR2HSV );
                
                

                //shoe the two windows
                cvShowImage("rgb",rgb); 
                cvShowImage("gray",gray);
                cvShowImage("hsv",hsv);
                cvShowImage("grayr",grayr);
                cvShowImage("grayg",grayg);
                cvShowImage("grayb",grayb);
                cvShowImage("grayh",grayh);
                cvShowImage("grays",grays);
                cvShowImage("grayv",grayv);
               
                
                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

