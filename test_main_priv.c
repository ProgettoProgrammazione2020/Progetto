#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"

int main(){

    char *source = "flower.bmp";
    char *destination = "B&W_flower.bmp";
    int success;

    Bitmap *test1 = bm_load(source); //carico file input
    ip_mat *test1_ipmat = bitmap_to_ip_mat(test1); //converto in ip_mat
    ip_mat *result = ip_mat_to_gray_scale(test1_ipmat); //CONVERTO IN SCALA DI GRIGI
    Bitmap *out = ip_mat_to_bitmap(result);//converto in bitmap
    success = bm_save(out,destination);//salvo l'immagine su filter

    ip_mat_free(test1_ipmat);
    ip_mat_free(result);
    bm_free(test1);
    bm_free(out); //libero tutta la memoria dinamica
    return success;
}
