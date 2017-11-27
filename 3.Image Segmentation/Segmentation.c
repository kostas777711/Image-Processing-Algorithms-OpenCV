#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main (int argc, char** argv )
{
         //key for exit
        int c;
        //for the number of segments on the image (nxn= the total segments)
        //expirement with this variable for different results
        int n=8;
        //counters for loop which creates the points
        int i,j;
        //color of the rectangles
        CvScalar colorRect={0,255,255};
        //text color
        CvScalar colorText={0,0,255};
        //initialise the font that we re goin to use
        CvFont font;
        //initialise the strings for the display of the two points of each rectangle
        char string_p1[40];
        char string_p2[40];
        //initialisation of segmented image
        IplImage *segment =0;
        //struct capture
        CvCapture* capture=cvCaptureFromCAM(0);
        //initialise the two windows
        cvNamedWindow("src",1);
        cvNamedWindow("segment",1);
        
        while(1)  //infinite loop
        {
                IplImage *src = 0;
                src=cvQueryFrame(capture);
                
                if( !src )    //if there is no frame exit the while(1)
                break;
                
                if( !segment )    //if there is no image segment, set its format
                {
                    segment=cvCreateImage(cvGetSize(src),8,3);
                }
                //copy the src to image segment
                cvCopy(src,segment,0);
                
                //two for loops
                //for creating the various points 
                //and display them on the image segment
                
                //n X n : the total number of segments
                for (i = 0; i <= n; i++)  
                {
                      for (j = 0; j <= n; j++)
                      {
                          //create the points
                          CvPoint p1={i * src->width / n, j*src->height / n};
                          CvPoint p2={(i + 1) * src->width / n, (j+1)*src->height / n};
                          
                          //initialise the font that we'll use
                          cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,0.5,0.5, 0,1, 8);
                          
                          //create the string 
                          //which will display point 1 
                          //of each rectangle
                          //begin the string with the "("
                          strcpy( string_p1, "(");
                          char p1_x[5];
                          //convert int to string
                          itoa(p1.x, p1_x, 10);
                          //copy to the end of the previous string
                          strcat( string_p1, p1_x);
                          //add comma
                          strcat( string_p1,",");
                          char p1_y[5];
                          //convert int to string
                          itoa(p1.y, p1_y, 10);
                          //add this to the current end
                          strcat( string_p1, p1_y);
                          //complete the string for display
                          strcat( string_p1,")");
                          //display the final string 
                          cvPutText(segment, string_p1, p1, &font, colorText);
                          //display the rectangles based on the two points
                          cvRectangle(segment, p1, p2, colorRect, 1, 8, 0);
                      }
                }
                //show the two images
                cvShowImage("src",src); 
                cvShowImage("segment",segment); 

                //ready to exit loop
                c=cvWaitKey(10);
                if(c==27)break;
        }
        cvReleaseCapture( &capture);
        cvDestroyAllWindows();
}

