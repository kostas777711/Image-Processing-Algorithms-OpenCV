#include <stdio.h>
#include <stdlib.h>

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


int main(int argc, char *argv[])
{
    argv[1]="112";
    argv[2]="172";
    argv[3]="182";
    struct rgb_color rgb;
    struct hsv_color hsv;
    rgb.r = (unsigned char)atoi(argv[1]);
    rgb.g = (unsigned char)atoi(argv[2]);
    rgb.b = (unsigned char)atoi(argv[3]);
    hsv = rgb_to_hsv(rgb);
    printf("Hue: %d\nSaturation: %d\nValue: %d\n\n", hsv.hue, hsv.sat, hsv.val);
    system("PAUSE");	
    return 0;
}
