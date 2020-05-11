#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"

int main(){

    Bitmap *test1 = bm_load("flower.bmp"); //carico file input
    ip_mat *test1_ipmat = bitmap_to_ip_mat(test1); //converto in ip_mat
    ip_mat *result = ip_mat_to_gray_scale(test1_ipmat); //CONVERTO IN SCALA DI GRIGI
    Bitmap *out = ip_mat_to_bitmap(result);//converto in bitmap
    bm_save(out,"TEST1.bmp");//salvo l'immagine su filter

    ip_mat_free(test1_ipmat);
    ip_mat_free(result);
    bm_free(test1);
    bm_free(out); //libero tutta la memoria dinamica
    return 0;
}
