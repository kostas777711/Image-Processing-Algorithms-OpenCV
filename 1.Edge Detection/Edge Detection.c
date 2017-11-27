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
        IplImage *grayDown =0;
        IplImage *grayUp =0;
        //initialise capture structure for incoming video stream
        CvCapture* capture=cvCaptureFromCAM(0);
        //initialise the two windows that we will use for output
        cvNamedWindow("src",1);
        cvNamedWindow("grayDown",1);
        cvNamedWindow("grayUp",1);
        cvNamedWindow("gray",1);
        
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
                    grayDown=cvCreateImage(cvSize( gray->width/2, gray->height/2 ),8,1);
                    grayUp=cvCreateImage(cvGetSize(src),8,1);
                }
                
                //main image processing
                //convert to gray scale
                cvCvtColor(src,gray,CV_BGR2GRAY );
                //downscale the gray scale image
                cvPyrDown(gray,grayDown,CV_GAUSSIAN_5x5 );
                //upscale the downscaled image
                cvPyrUp(grayDown,grayUp,CV_GAUSSIAN_5x5 );
                //perform the canny edge detection algorithm
                //we can expirament with the up and down limit for
                //different results
                cvCanny(grayUp,gray,3,100,3 );

                //shoe the two windows
                cvShowImage("src",src); 
                cvShowImage("grayDown",grayDown);
                cvShowImage("grayUp",grayUp);
                cvShowImage("gray",gray); 
                
                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

