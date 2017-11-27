#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void show_histogram(char* window_title, IplImage* src, char* channel_name);

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
        CvScalar colorCenter={0, 0, 255};
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
               
               
               //bgr histograms
                show_histogram("scrHistBlue", src, "blue");
                show_histogram("scrHistGreen", src, "green");
                show_histogram("srcHistRed", src, "red");
                
                //segment histograms
                show_histogram("hsvHistBlue", hsv, "blue");
                show_histogram("hsvHistGreen", hsv, "green");
                show_histogram("hsvHistRed", hsv, "red");
               
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

