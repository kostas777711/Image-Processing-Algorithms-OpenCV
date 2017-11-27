#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void show_histogram(char* window_title, IplImage* src, char* channel_name);

int main (int argc, char** argv )
{
         //initialise key for exit and color choice
         int c,key;
         //number of iteration for dilationa and erosion
         int iterations = 5;
         //initialization of the color range ,here for blue
         CvScalar hsv_min={25, 180, 180};
         CvScalar hsv_max={60, 255, 255}; 
       
       //initialise the two secondary images that we are going to use
       IplImage *inRange =0;
        IplImage *thresholded =0;
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
                
                //bgr histograms
                show_histogram("scrHistBlue", src, "blue");
                show_histogram("scrHistGreen", src, "green");
                show_histogram("srcHistRed", src, "red");
                
                //hsv histograms
                show_histogram("thresholdedGray", thresholded, "gray");
                
                
                
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

void show_histogram(char* window_title, IplImage* src, char* channel_name)
{
  IplImage* img, * canvas;
  int       bins = 256;
  int       hist[bins];
  double    scale;
  int       i, j, channel, max = 0;

  CvScalar   colors[] = { CV_RGB(0,0,255), CV_RGB(0,255,0),
                          CV_RGB(255,0,0), CV_RGB(0,0,0) };

  channel = strcmp(channel_name, "blue")  == 0 ? 0
          : strcmp(channel_name, "green") == 0 ? 1
          : strcmp(channel_name, "red")   == 0 ? 2
          : strcmp(channel_name, "gray")  == 0 ? 3 : 0;

  if (src->nChannels == 3 && channel == 3)
  {
    img = cvCreateImage(cvGetSize(src), 8, 1);
    cvCvtColor(src, img, CV_BGR2GRAY);
  }
  else if (channel > src->nChannels)
    return;
  else
    img = cvCloneImage(src);

  canvas = cvCreateImage(cvSize(256, 125), IPL_DEPTH_8U, 3);
  cvSet(canvas, CV_RGB(255,255,255), NULL);

  /* Reset histogram */
  for (j = 0; j < bins-1; hist[j]=0, j++);

  /* Calc histogram of the image */
  for (i = 0; i < img->height; i++)
  {
    uchar* ptr = (uchar*)(img->imageData + i * img->widthStep);
    for (j = 0; j < img->width; j+=img->nChannels)
      hist[ptr[j+(channel == 3 ? 0 : channel)]]++;
  }

  /* Get histogram peak */
  for (i = 0; i < bins-1; i++)
    max = hist[i] > max ? hist[i] : max;

  /* Get scale so the histogram fit the canvas height */
  scale = max > canvas->height ? (double)canvas->height/max : 1.;

  /* Draw histogram */
  for (i = 0; i < bins-1; i++)
  {
    CvPoint pt1 = cvPoint(i, canvas->height - (hist[i] * scale));
    CvPoint pt2 = cvPoint(i, canvas->height);
    cvLine(canvas, pt1, pt2, colors[channel], 1, 8, 0);
  }

  cvShowImage(window_title, canvas);
  cvReleaseImage(&img);
}
