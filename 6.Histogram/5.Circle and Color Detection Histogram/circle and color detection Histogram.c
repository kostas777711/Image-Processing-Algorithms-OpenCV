#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


void show_histogram(char* window_title, IplImage* src, char* channel_name);


int main (int argc, char** argv )
{
    
         int thresh = 50;
         CvScalar centreColor={0,0,0};   //black centre
         CvScalar circumColor={255,0,255};   //purple centre
         
         //averaging the values of the colors
         int row_x=0,row_x1=0,row_x2=0;
         int total=0;
         
         int c,x,y,key;
         int px[0], py[0];
         int edge_thresh = 1;
         IplImage *csrc=0;
         IplImage *gray =0;  
         IplImage *edge =0;  
         //IplImage *hsv =0; 
         CvMemStorage* cstorage = cvCreateMemStorage(0);
         
        
         //get the video from webcam
         CvCapture* capture=cvCaptureFromCAM(0);
        
        //initialese windows
        cvNamedWindow("src",1);
        cvNamedWindow("gray",1);
        //cvNamedWindow("hsv",1);

        while(1)
        {
                IplImage *src = 0;
                src=cvQueryFrame(capture);
                if( !src )    //if there is no frame exit the while(1)
                break;
                if( !csrc )    //if there is no image csrc, do the followings
                {
                    csrc=cvCreateImage(cvGetSize(src),8,3);
                    //hsv=cvCreateImage(cvGetSize(src),8,3);
                    gray=cvCreateImage(cvGetSize(src),8,1);
                    edge=cvCreateImage(cvGetSize(src),8,1);
                }
                csrc=cvCloneImage(src);
                //cvCvtColor(src,hsv,CV_BGR2HSV);
                cvCvtColor(src,gray,CV_BGR2GRAY);
                gray->origin=1;
                cvThreshold(gray,gray,100,255,CV_THRESH_BINARY);
                cvSmooth( gray, gray, CV_GAUSSIAN, 11, 11 ,0,0);
                cvCanny(gray, edge, (float)edge_thresh, (float)edge_thresh*3, 5);
                CvSeq* circles =  cvHoughCircles( gray, cstorage, CV_HOUGH_GRADIENT, 2, gray->height/50, 5, 35,0,0 );

                int i;
                for( i = 0; circles->total>=1?i<1:i < circles->total; i++ ) //just make a filter to limit only <=2 circles to draw
                {
                     float* p = (float*)cvGetSeqElem( circles, i );
                     cvCircle( src, cvPoint(cvRound(p[0]),cvRound(p[1])), 1, centreColor, 1, 8, 0 );   //dot circle
                     cvCircle( src, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), circumColor, 3, 8, 0 );  //circle circuference
                     px[i]=cvRound(p[0]); 
                     py[i]=cvRound(p[1]);
                }

                  key=cvWaitKey(10);
                  if (key=='a')  //sampling
                  {
                   for (y=0;y<src->height;y++)
                   {
                      unsigned char* row=&CV_IMAGE_ELEM(src,unsigned char,y,0);
                      for(x=0;x<src->width*src->nChannels;x+=src->nChannels)
                      {
                                //search for the black centre still in RGB? Why?
                                if( row[x]==0 && row[x+1]==0 && row[x+2]==0)
                                {
                                    
                                    total++;
                                    row_x+=row[x+12];
                                    row_x1+=row[x+13];
                                    row_x2+=row[x+14];
                                    
                                    
                                    
                                    printf("Centre : R[%d] G[%d] B[%d] \n",row[x+14],row[x+13],row[x+12]);
                                }                                                                
                      }
                      
                    }
                    printf("\n Average : R[%d] G[%d] B[%d] \n\n",cvRound(row_x2/total),cvRound(row_x1/total),cvRound(row_x/total));
                  }
                   
                  
                  //bgr histograms
                show_histogram("scrHistBlue", src, "blue");
                show_histogram("scrHistGreen", src, "green");
                show_histogram("srcHistRed", src, "red");
                
                
                  
                cvShowImage("src",src); 
                cvShowImage("gray",gray); 
                //cvShowImage("hsv",hsv); 
                //release memory
                cvReleaseImage(&csrc);
                
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

