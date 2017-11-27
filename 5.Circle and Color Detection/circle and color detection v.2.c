#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


#define MIN3(x,y,z)  ((y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)))
#define MAX3(x,y,z)  ((y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)))
                        
struct rgb_color 
{
    unsigned char r, g, b;    /* Channel intensities between 0 and 255 */
};

struct hsv_color 
{
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
};

struct hsv_color rgb_to_hsv(struct rgb_color rgb) 
{
    struct hsv_color hsv;
    unsigned char rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);
    hsv.val = rgb_max;
    if (hsv.val == 0) 
    {
    hsv.hue = hsv.sat = 0;
    return hsv;
    }
    hsv.sat = 255*(rgb_max - rgb_min)/hsv.val;
    if (hsv.sat == 0) 
    {
    hsv.hue = 0;
    return hsv;
    }
    /* Compute hue */
    if (rgb_max == rgb.r) 
    {
    hsv.hue = 0 + 43*(rgb.g - rgb.b)/(rgb_max - rgb_min);
    } 
    else if (rgb_max == rgb.g) 
    {
    hsv.hue = 85 + 43*(rgb.b - rgb.r)/(rgb_max - rgb_min);
    } 
    else /* rgb_max == rgb.b */ 
    {
    hsv.hue = 171 + 43*(rgb.r - rgb.g)/(rgb_max - rgb_min);
    }
    return hsv;
}



int main (int argc, char** argv )
{
         //initialise structures
        struct rgb_color rgb;
        struct hsv_color hsv;
        
         int thresh = 50;
         CvMemStorage* fstorage = 0;
         CvScalar centreColor={0,0,0};   //black centre
         CvScalar circumColorI={255,255,255};   //purple centre Internal
         CvScalar circumColorE={255,0,0};   //purple centre External 
         
         //averaging the values of the colors
         int row_x=0,row_x1=0,row_x2=0;
         int total=0;
         
         int c,x,y,key;
         int px[0], py[0];
         int edge_thresh = 1;
         IplImage *csrc=0;
         IplImage *gray =0;  
         IplImage *edge =0; 
         IplImage *thresholded =0;   
         IplImage *hsv2 =0; 
         
         CvMemStorage* cstorage = cvCreateMemStorage(0);
         fstorage= cvCreateMemStorage(0);
        
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
                    gray=cvCreateImage(cvGetSize(src),8,1);
                    edge=cvCreateImage(cvGetSize(src),8,1);
                }
                csrc=cvCloneImage(src);
                //cvCvtColor(src,hsv,CV_BGR2HSV);
                cvCvtColor(src,gray,CV_BGR2GRAY);
                gray->origin=1;
                cvThreshold(gray,gray,100,255,CV_THRESH_BINARY);
                cvSmooth( gray, gray, CV_GAUSSIAN, 11, 11 ,0,0);
                cvCanny(gray, edge, (float)edge_thresh*3, (float)edge_thresh*7, 5); //cvCanny(gray, edge, (float)edge_thresh, (float)edge_thresh*3, 5);
                CvSeq* circles =  cvHoughCircles( gray, cstorage, CV_HOUGH_GRADIENT, 2, gray->height/10, 5,50, 0,100 );   //5,35,0,0

                int i;
                for( i = 0; circles->total>=1?i<1:i < circles->total; i++ ) //just make a filter to limit only <=2 circles to draw
                {
                     float* p = (float*)cvGetSeqElem( circles, i );
                     cvCircle( src, cvPoint(cvRound(p[0]),cvRound(p[1])), 1, centreColor, 1, 8, 0 );   //black circle centre (dot)
                     cvCircle( src, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2])-3, circumColorI, 3, 8, 0 );  //circle circuference
                     cvCircle( src, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), circumColorE, 3, 8, 0 );  //circle circuference
                     px[i]=cvRound(p[0]); 
                     py[i]=cvRound(p[1]);
                     printf("Centre (%d,%d), Radius: %d\n",px[0],py[0],cvRound(p[2]));
                }


                  key=cvWaitKey(10);
                  
                  if (key=='a')  //sampling
                  {
                    printf("\n\n"); //two new lines
                                
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
                    printf("\n Average : R[%d] G[%d] B[%d] \n",cvRound(row_x2/total),cvRound(row_x1/total),cvRound(row_x/total));
                    rgb.r=cvRound(row_x2/total);
                    rgb.g=cvRound(row_x1/total);
                    rgb.b=cvRound(row_x/total);
                    hsv = rgb_to_hsv(rgb);
                    printf("\n >>Average : H[%d] S[%d] V[%d] <<\n\n",hsv.hue, hsv.sat, hsv.val);
                    printf("\tMenu\n");
                    printf(" a - Sample Color Again\n");
                    printf(" c - Find color \n\n");
                  }
                   
                  
                  key=cvWaitKey(10);
                  
                  if(key=='c')
                  { 
                              //set half ranges of the HSV format
                              int hue_half_range=90;
                              int sat_half_range=15;
                              int val_half_range=15;
                              
                              //set hsv range
                              int hsv_min_hue=hsv.hue-hue_half_range;
                              if(hsv_min_hue<0)
                                      hsv_min_hue=0; 
                                              
                              int hsv_min_sat=hsv.sat-sat_half_range;
                              if(hsv_min_sat<0)
                                      hsv_min_sat=0;
                                      
                              int hsv_min_val=hsv.val-val_half_range;
                              if(hsv_min_val<0)
                                      hsv_min_val=0;
                                      
                              int hsv_max_hue=hsv.hue+hue_half_range;
                              if(hsv_max_hue>255)
                                      hsv_max_hue=255;         
                              
                              int hsv_max_sat=hsv.sat+sat_half_range;
                              if(hsv_max_sat>255)
                                      hsv_max_sat=255; 
                              
                              int hsv_max_val=hsv.val+val_half_range;
                              if(hsv_max_val>255)
                                      hsv_max_val=255; 
                              
                              CvScalar hsv_min={hsv_min_hue, hsv_min_sat, hsv_min_val};  //hsv_min_val
                              CvScalar hsv_max={hsv_max_hue, hsv_max_sat, hsv_max_val};  //hsv_max_val
                              
                              //print hsv range
                              printf("\n HSV_MIN : H[%d] S[%d] V[%d] \n",hsv_min_hue, hsv_min_sat, hsv_min_val);
                              printf("\n HSV_MAX : H[%d] S[%d] V[%d] \n",hsv_max_hue, hsv_max_sat, hsv_max_val);
                              //iterations
                              int iterations=5;
                              CvMoments moments;
                              CvPoint centre;
                              //initialise the color for the centre and the font in RGB
                              CvScalar colorCenter={255, 255, 0};
                              CvScalar colorFont={255, 0, 0};
                              //initialise the font 
                              CvFont font;
                               //initialise the string to display the centre of the detected area
                               char textCenter[40];
                              
                            while(1) //infinite loop
                            {
                                   IplImage *src = 0;
                                   src=cvQueryFrame(capture);
                
                                   if( !src )    //if there is no frame exit the while(1)
                                   break;
                
                                   if( !thresholded )    //if there is no image thresholded, do the followings
                                   {

                                       thresholded=cvCreateImage(cvGetSize(src),8,1);
                                       hsv2=cvCreateImage(cvGetSize(src),8,3);
                    
                                    }
                                    //convert image to HSV format
                                    cvCvtColor(src, hsv2, CV_BGR2HSV);
                                    //search in the first range of pink in HSV
                                    cvInRangeS(hsv2, hsv_min, hsv_max, thresholded);
                                    //filter - smoothing - BLUR
                                    cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                                    //cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                                    //cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9, 0, 0);
                                    //threshold to hold the disired area
                                    cvThreshold(thresholded, thresholded, 12, 255, CV_THRESH_BINARY);
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
                  }
                  
                  
                cvShowImage("src",src); 
                cvShowImage("gray",gray); 
                //cvShowImage("hsv",hsv); 
                //release memory
                cvReleaseImage(&csrc);
                cvClearMemStorage( fstorage );
                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

