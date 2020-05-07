/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

void ip_mat_show(ip_mat * t){
    unsigned int i,l,j;
    printf("Matrix of size %d x %d x %d (hxwxk)\n",t->h,t->w,t->k);
    for (l = 0; l < t->k; l++) {
        printf("Slice %d\n", l);
        for(i=0;i<t->h;i++) {
            for (j = 0; j < t->w; j++) {
                printf("%f ", get_val(t,i,j,l));
            }
            printf("\n");
        }
        printf("\n");
    }
}

void ip_mat_show_stats(ip_mat * t){
    unsigned int k;

    compute_stats(t);

    for(k=0;k<t->k;k++){
        printf("Channel %d:\n", k);
        printf("\t Min: %f\n", t->stat[k].min);
        printf("\t Max: %f\n", t->stat[k].max);
        printf("\t Mean: %f\n", t->stat[k].mean);
    }
}

ip_mat * bitmap_to_ip_mat(Bitmap * img){
    unsigned int i=0,j=0;

    unsigned char R,G,B;

    unsigned int h = img->h;
    unsigned int w = img->w;

    ip_mat * out = ip_mat_create(h, w,3,0);

    for (i = 0; i < h; i++)              /* rows */
    {
        for (j = 0; j < w; j++)          /* columns */
        {
            bm_get_pixel(img, j,i,&R, &G, &B);
            set_val(out,i,j,0,(float) R);
            set_val(out,i,j,1,(float) G);
            set_val(out,i,j,2,(float) B);
        }
    }

    return out;
}

Bitmap * ip_mat_to_bitmap(ip_mat * t){

    Bitmap *b = bm_create(t->w,t->h);

    unsigned int i, j;
    for (i = 0; i < t->h; i++)              /* rows */
    {
        for (j = 0; j < t->w; j++)          /* columns */
        {
            bm_set_pixel(b, j,i, (unsigned char) get_val(t,i,j,0),
                    (unsigned char) get_val(t,i,j,1),
                    (unsigned char) get_val(t,i,j,2));
        }
    }
    return b;
}

float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    if(i<a->h && j<a->w &&k<a->k){  /* j>=0 and k>=0 and i>=0 is non sense*/
        return a->data[i][j][k];
    }else{
        printf("Errore get_val!!!");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[i][j][k]=v;
    }else{
        printf("Errore set_val!!!");
        exit(1);
    }
}

float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));

}

ip_mat * ip_mat_create(unsigned int h, unsigned int w,unsigned  int k, float v)
{
    int x, y, z;
    ip_mat *result;
    result = (ip_mat*)malloc(sizeof(ip_mat));
    result->w = w;
    result->h = h;
    result->k = k;
    result->data = (float***)malloc(h * sizeof (float**));
    for (x=0; x<h; x++)
    {
        result->data[x] = (float**)malloc(w * sizeof(float*));
        for (y=0; y<w; y++)
        {
            result->data[x][y] = (float*)malloc(k*sizeof(float));
            for (z=0; z<k; z++)
                result->data[x][y][z] = v;
        }
    }
    result->stat = (stats*)malloc(k*sizeof(stats));
    return result;
}

ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b)
{
    int i,j,c;
    ip_mat *result;

    if((a->w == b->w) && (a->h == b->h) && (a->k == b->k))
    {
        result = ip_mat_create(a->h,a->w,a->k,0.0);
        for(i = 0; i < a->w; i++)
        {
            for(j = 0; j < a->h; j++)
            {
                for(c = 0; c < a->k; c++)
                {
                    set_val(result,i,j,c,get_val(a,i,j,c) + get_val(b,i,j,c));
                }
            }
        }
    }
    else
    {
        printf("Le immagini sono di dimensioni diverse");
        result = NULL;
    }


    return result;

}

ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b)
{
    int i,j,c;
    ip_mat *result;

    if((a->w == b->w) && (a->h == b->h) && (a->k == b->k))
    {
        result = ip_mat_create(a->h,a->w,a->k,0.0);
        for(i = 0; i < a->w; i++)
        {
            for(j = 0; j < a->h; j++)
            {
                for(c = 0; c < a->k; c++)
                {
                    set_val(result,i,j,c,get_val(a,i,j,c) - get_val(b,i,j,c));
                }
            }
        }
    }
    else
    {
        printf("Le immagini sono di dimensioni diverse");
        result = NULL;
    }

    return result;

}

/* Concatena due ip_mat su una certa dimensione.
 * Ad esempio:
 * ip_mat_concat(ip_mat * a, ip_mat * b, 0);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h + b.h
 *      out.w = a.w = b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 1);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w + b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 2);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w = b.w
 *      out.k = a.k + b.k
 * */
ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione)
{
    int i, j, c;
    ip_mat *result;
    if((dimensione == 0) && (a->w == b->w) && (a->k == b->k))
    {
      result = ip_mat_create(a->h + b->h, a->w, a->k, 0.0);
      for(i = 0; i < a->h;i++) /*adding a*/
      {
        for(j = 0; j < result->w;j++)
        {
          for(c = 0; c < result->k; c++)
          {
            set_val(result,i,j,c,get_val(a,i,j,c));
          }
        }
      }
      for(i = 0; i < b->h;i++) /*adding b*/
      {
        for(j = 0; j < result->w;j++)
        {
          for(c = 0; c < result->k; c++)
          {
            set_val(result,i + a->h,j,c,get_val(b,i,j,c));
          }
        }
      }
    }
    else if((dimensione == 1) && (a->h == b->h) && (a->k == b->k))
    {
      result = ip_mat_create(a->h, a->w + b->w, a->k, 0.0);
      for(i = 0; i < result->h;i++) /*adding a*/
      {
        for(j = 0; j < a->w;j++)
        {
          for(c = 0; c < result->k; c++)
          {
            set_val(result,i,j,c,get_val(a,i,j,c));
          }
        }
      }
      for(i = 0; i < result->h;i++) /*adding b*/
      {
        for(j = 0; j < b->w;j++)
        {
          for(c = 0; c < result->k; c++)
          {
            set_val(result,i,j+ a->w,c,get_val(b,i,j,c));
          }
        }
      }
    }
    else if((dimensione == 2) && (a->h == b->h) && (a->w == b->w))
    {
      result = ip_mat_create(a->h, a->w , a->k + b->k, 0.0);
      for(i = 0; i < result->h;i++) /*adding a*/
      {
        for(j = 0; j < result->w;j++)
        {
          for(c = 0; c < a->k; c++)
          {
            set_val(result,i,j,c,get_val(a,i,j,c));
          }
        }
      }
      for(i = 0; i < result->h;i++) /*adding b*/
      {
        for(j = 0; j < result->w;j++)
        {
          for(c = 0; c < b->k; c++)
          {
            set_val(result,i,j,c+ a->k,get_val(b,i,j,c));
          }
        }
      }
    }
    else
    {
      result = NULL;
      printf("Input error: incongruent dimensions");
    }

    return result;
}
