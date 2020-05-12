#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"

int main(){

    char *source = "flower.bmp";
    //char *source = "flower2.bmp";
    //char *source1 = "mongolfiere.bmp";
    char *destination = "bright_flower.bmp";
    //char *destination = "blending.bmp";
    
    int success;

    Bitmap *test1 = bm_load(source); //carico file input
    //Bitmap *test2 = bm_load(source1); //carico file input
    
    ip_mat *test1_ipmat = bitmap_to_ip_mat(test1); //converto in ip_mat
    //ip_mat *test2_ipmat = bitmap_to_ip_mat(test2); //converto in ip_mat
    
    //ip_mat *result = ip_mat_blend(test1_ipmat, test2_ipmat, 1.0);//BLENDING DI DUE IMMAGINI
    /*ip_mat *result = ip_mat_to_gray_scale(test1_ipmat); //CONVERTO IN SCALA DI GRIGI*/
    ip_mat *result = ip_mat_brighten(test1_ipmat, 100.0);
    
    Bitmap *out = ip_mat_to_bitmap(result);//converto in bitmap

    
    success = bm_save(out,destination);//salvo l'immagine su filter

    ip_mat_free(test1_ipmat);
    //ip_mat_free(test2_ipmat);
    ip_mat_free(result);
    bm_free(test1);
    //bm_free(test2);
    bm_free(out); //libero tutta la memoria dinamica
    return success;
}
