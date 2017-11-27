#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main (int argc, char** argv )
{
         //initialise the string to display the centre of the detected area
        char textCenter[40];
        //exit key
        int c;
        //iterations for erosion and dilation
        int iterations = 5;
        //initialise secondary images
        IplImage *thresholded =0;
        IplImage *thresholded2 =0;
        IplImage *hsv =0;
        //moments to calculate the centre
        CvMoments moments;
        //struct of the centre
        CvPoint centre;
        //initialise the color for the centre and the font in RGB
        CvScalar colorCenter={0, 255, 255};
        CvScalar colorFont={255, 0, 0};
        //initialise the font 
        CvFont font;
        //the twopink color ranges for the detection of the pink ball
        CvScalar hsv_min={0, 50, 170};
        CvScalar hsv_max={10, 180, 256};
        CvScalar hsv_min2={170, 50, 170};
        CvScalar hsv_max2={256, 180, 26};
        //structure for capturing the images
        CvCapture* capture=cvCaptureFromCAM(0);
        //create two windows to diplay the output
        cvNamedWindow("src",1);
        cvNamedWindow("thresholded",1);
        
        while(1)   //infinite main loop
        {
                IplImage *src = 0;
                src=cvQueryFrame(capture);
                
                if( !src )    //if there is no frame exit the while(1)
                break;
                
                if( !thresholded )    //if there is no image thresholded, do the followings
                {

                    thresholded=cvCreateImage(cvGetSize(src),8,1);
                    thresholded2=cvCreateImage(cvGetSize(src),8,1);
                    hsv=cvCreateImage(cvGetSize(src),8,3);
                    
                }
                //convert image to HSV format
                cvCvtColor(src, hsv, CV_BGR2HSV);
                //search in the first range of pink in HSV
                cvInRangeS(hsv, hsv_min, hsv_max, thresholded);
                //search in the second range of pink color in HSV
                cvInRangeS(hsv, hsv_min2, hsv_max2, thresholded2);
                //logical or between the two gray thresholded images 
                cvOr(thresholded, thresholded2, thresholded, 0);
                //filter - smoothing - BLUR
                cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                //threshold to hold the disired area
                cvThreshold(thresholded, thresholded, 12, 256, CV_THRESH_BINARY);
                //opening and closing - filter the small white areas
                cvErode(thresholded, thresholded, 0, iterations);
                cvDilate(thresholded, thresholded, 0, iterations);
                cvDilate(thresholded, thresholded, 0, iterations);
                cvErode(thresholded, thresholded, 0, iterations);
                //hold moments from the final gray image
                cvMoments(thresholded, &moments, 1);
                //calculate the spatial and central moments
                double moment10 = cvGetSpatialMoment(&moments, 1, 0);
                double moment01 = cvGetSpatialMoment(&moments, 0, 1);
                double areaPink = cvGetCentralMoment(&moments, 0, 0);
                //initialise the position x & y
                int posX = 0;
                int posY = 0;
                //calculate position x & y
                posX = cvRound(moment10 / areaPink);
                posY = cvRound(moment01 / areaPink);
                //give to the struct centre
                centre.x=posX;
                centre.y=posY;
                //if centre position positive (acceptable values)
                if (posX > 0 && posY > 0)
                {
               // draw circle and display the position of the gravity center of the pink blob detected
               cvCircle(src, centre, 10, colorCenter, -1, 8, 0);
               cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1, 1, 0, 1, 8);
               strcpy( textCenter, "(");
               char p1_x[5];
               itoa(posX, p1_x, 10);
               strcat( textCenter, p1_x);
               strcat( textCenter,",");
               char p1_y[5];
               itoa(posY, p1_y, 10);
               strcat( textCenter, p1_y);
               strcat( textCenter,")");
               cvPutText(src, textCenter, centre, &font, colorFont);
               }
                //show the two windows as an output
                cvShowImage("src",src); 
                cvShowImage("thresholded",thresholded); 
                

                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

