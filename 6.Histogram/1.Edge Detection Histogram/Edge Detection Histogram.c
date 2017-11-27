#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void show_histogram(char* window_title, IplImage* src, char* channel_name);

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

                //show_histogram("histg", src, "gray");
                show_histogram("scrHistBlue", src, "blue");
                show_histogram("scrHistGreen", src, "green");
                show_histogram("srcHistRed", src, "red");

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
