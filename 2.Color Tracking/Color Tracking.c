#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main (int argc, char** argv )
{
         //initialise key for exit and color choice
         int c,key;
         //number of iteration for dilationa and erosion
         int iterations = 5;
         //initialization of the color range ,here for blue
         CvScalar hsv_min={110,190,0};
         CvScalar hsv_max={120, 225, 255}; 
       
       //initialise the two secondary images that we are going to use
       IplImage *inRange =0;
        IplImage *thresholded =0;
        IplImage *cthresholded =0;
        IplImage *hsv =0;
        IplImage *smoothed =0;
        
        //capture struct for video stream
        CvCapture* capture=cvCaptureFromCAM(0);
        
        //initialise the two windows
        cvNamedWindow("src",1);
        cvNamedWindow("hsv",1);
        cvNamedWindow("inRange",1);
        cvNamedWindow("thresholded",1);
        
        //here is the menu that corresponds to the tick 
        //menu in my Visual C# application 
        //for the Rovios
        
        //printf("      Menu    \n");
        //choose color detection
        //printf("Choose Color    \n");
//        printf("b: BLUE        \n");
//        printf("p: PURPLE      \n");
//        printf("y: YELLOW     \n");
//        printf("n: Nothing     \n");
//        printf("Enter Character    \n");
//        scanf("%d\n",&key);
//        printf("you chose:%d\n",key);
//        if (key==1)
//                {
//                    CvScalar hsv_min={110,50,110};
//                    CvScalar hsv_max={124, 180, 200};    
//                }
//        else if (key==2)
//                {
//                     CvScalar hsv_min={125, 50, 110};
//                     CvScalar hsv_max={150, 180, 200};  
//                }
//        else if (key==3)
//                {
//                     CvScalar hsv_min={31, 50, 180};
//                    CvScalar hsv_max={40, 200, 200};  
//                } 
                

        while(1) //main loop
        {
         
                IplImage *src = 0;
                src=cvQueryFrame(capture);
                
                if( !src )    //if there is no frame exit the while(1)
                break;
                
                if( !thresholded )    //if there is no image thresholded, do the followings
                {
                    hsv=cvCreateImage(cvGetSize(src),8,3); 
                    thresholded=cvCreateImage(cvGetSize(src),8,1);
                    cthresholded=cvCreateImage(cvGetSize(src),8,1);
                    inRange=cvCreateImage(cvGetSize(src),8,1);
                    smoothed=cvCreateImage(cvGetSize(src),8,1);  
                }
                //convert to HSV
                cvCvtColor(src, hsv, CV_BGR2HSV);
                //search for the given range
                cvInRangeS(hsv, hsv_min, hsv_max, inRange);
                
                //filtering the gray image
                cvSmooth(inRange, smoothed, CV_BLUR, 9, 9, 0, 0);
                //cvSmooth(smoothed, smoothed, CV_BLUR, 9, 9, 0, 0);
                //cvSmooth(smoothed, smoothed, CV_BLUR, 9, 9, 0, 0);
                
                //detecting the appropriate range in the gray scale
                cvThreshold(smoothed, thresholded, 12, 256, CV_THRESH_BINARY);
                
                //opening and closing 
                cvErode(thresholded, thresholded, NULL, iterations);
                cvDilate(thresholded, thresholded, NULL, iterations);
                cvDilate(thresholded, thresholded, NULL, iterations);
                cvErode(thresholded, thresholded, NULL, iterations);
                
                
                //show the two images
                cvShowImage("src",src); 
                cvShowImage("hsv",hsv);
                cvShowImage("inRange",inRange);
                cvShowImage("smoothed",smoothed);
                cvShowImage("thresholded",thresholded); 
                
                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

