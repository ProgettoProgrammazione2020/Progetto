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
        printf("[get_val]Errore: coordinate inserite non corrette");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[i][j][k]=v;
    }else{
        printf("[set_val]Errore: coordinate inserite non corrette");
        exit(1);
    }
}

float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));

}

/*
* PARTE 1
*/

void compute_stats(ip_mat * t){
    float max,min,somma;
    int x,y,z;
    for(x=0;x<(t->k);x++)
    {
        max=get_val(t,0,0,x);
        min=get_val(t,0,0,x);
        for(y=0;y<(t->h);y++)
        {
            for(z=0;z<(t->w);z++)
            {
                if(max<(get_val(t,y,z,x))){
                    max=get_val(t,y,z,x);
                }
                if(min>(get_val(t,y,z,x))){
                    min=get_val(t,y,z,x);
                }
                somma+=get_val(t,y,z,x);

            }
        }
        t->stat[x].min=min;
        t->stat[x].max=max;
        t->stat[x].mean=(somma/(float)((t->h)*(t->w)));
        somma=0.0;

    }
}

ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){
    int x,y,z;
    if((row_end-1)>(t->h) || (col_end-1)>(t->w)){
        printf("[ip_mat_subset] Errore: dimensioni errate");
        exit(1);
    }else{
        ip_mat *r=ip_mat_create(row_end,col_end,(t->k),0.0);
        for(x=0;x<(t->k);x++)
        {
            for(y=0;y<row_end;y++)
            {
                for(z=0;z<col_end;z++)
                {
                    set_val(r,y,z,x,get_val(t,y,z,x));
                }
            }
        }
        return r;
    }
}


ip_mat * ip_mat_copy(ip_mat * in){
    int channel, row, col;
    ip_mat *r;
    r=ip_mat_create(in->h,in->w,in->k,0.0);
    for (channel=0; channel<(r->k); channel++)
    {
        for (row=0; row<(r->h); row++)
        {
            for (col=0; col<(r->w); col++)
                set_val(r,row,col,channel,get_val(in,row,col,channel));
        }
        /*r->stat[z].min=in->stat[z].min;
        r->stat[z].max=in->stat[z].max;
        r->stat[z].mean=in->stat[z].mean;*/
    }
    return r;
}

void ip_mat_init_random(ip_mat * t, float mean, float var){
    int channel, row, col;
    for (channel=0; channel<(t->k); channel++)
    {
        for (row=0; row<(t->h); row++)
        {
            for (col=0; col<(t->w); col++)
                set_val(t,row,col,channel,get_normal_random()*var+mean);
        }
    }
}

ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){
    int channel, row, col;
    ip_mat *r;
    if((a->h)!=(b->h) || (a->w)!=(b->w) || (a->k)!=(b->k)){
        printf("[ip_mat_mean] Errore: dimensioni incongruenti!!!");
        exit(1);
    }else{
        r=ip_mat_create(a->h,a->w,a->k,0.0);
        for (channel=0; channel<(r->k); channel++)
        {
            for (row=0; row<(r->h); row++)
            {
                for (col=0; col<(r->w); col++)
                {
                    set_val(r,row,col,channel,(get_val(a,row,col,channel) + get_val(b,row,col,channel))/2.0);
                }
            }
        }
        return r;
    }
}
void ip_mat_free(ip_mat *a){
    int x, y;
    for (x=0; x<(a->h); x++)
    {

        for (y=0; y<(a->w); y++)
        {
            free(a->data[x][y]);
        }
        free(a->data[x]);
    }
    free(a->data);
    free(a->stat);
    free(a);
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
  int channel, row, col;
  ip_mat *result;

  if((a->w == b->w) && (a->h == b->h) && (a->k == b->k))
  {
      result = ip_mat_create(a->h,a->w,a->k,0.0);
      for(channel = 0; channel < a->k; channel++)
      {
          for(row = 0; row < a->h; row++)
          {
              for(col = 0; col < a->w; col++)
              {
                  set_val(result,row,col,channel,get_val(a,row,col,channel) + get_val(b,row,col,channel));
              }
          }
      }
  }
  else
  {
      printf("[ip_mat_sum]Le immagini sono di dimensioni diverse");
      exit(1);
  }

  return result;

}

ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b)
{

    int channel, row, col;
    ip_mat *result;

    if((a->w == b->w) && (a->h == b->h) && (a->k == b->k))
    {
        result = ip_mat_create(a->h,a->w,a->k,0.0);
        for(channel = 0; channel < a->k; channel++)
        {
            for(row = 0; row < a->h; row++)
            {
                for(col = 0; col < a->w; col++)
                {
                    set_val(result,row,col,channel,get_val(a,row,col,channel) - get_val(b,row,col,channel));
                }
            }
        }
    }
    else
    {
        printf("[ip_mat_sub]Le immagini sono di dimensioni diverse");
        exit(1);
    }

    return result;

}


ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione)
{
    int channel, row, col;
    ip_mat *result;
    if((dimensione == 0) && (a->w == b->w) && (a->k == b->k))
    {
      result = ip_mat_create(a->h + b->h, a->w, a->k, 0.0);
      for(channel = 0; channel < result->k;channel++) /*adding a*/
      {
        for(row = 0; row < a->h;row++)
        {
          for(col = 0; col < result->w; col++)
          {
            set_val(result,row,col,channel,get_val(a,row,col,channel));
          }
        }
      }
      for(channel = 0; channel < result->k; channel++) /*adding b*/
      {
        for(row = 0; row < b->h;row++)
        {
          for(col = 0; col < result->w; col++)
          {
            set_val(result,row + a->h,col,channel,get_val(b,row,col,channel));
          }
        }
      }
    }
    else if((dimensione == 1) && (a->h == b->h) && (a->k == b->k))
    {
      result = ip_mat_create(a->h, a->w + b->w, a->k, 0.0);
      for(channel = 0; channel < result->k;channel++) /*adding a*/
      {
        for(row = 0; row < result->h;row++)
        {
          for(col = 0; col < a->w; col++)
          {
            set_val(result,row,col,channel,get_val(a,row,col,channel));
          }
        }
      }
      for(channel = 0; channel < result->k;channel++) /*adding b*/
      {
        for(row = 0; row < result->h;row++)
        {
          for(col = 0; col < b->w; col++)
          {
            set_val(result,row,col + a->w,channel,get_val(b,row,col,channel));
          }
        }
      }
    }
    else if((dimensione == 2) && (a->h == b->h) && (a->w == b->w))
    {
      result = ip_mat_create(a->h, a->w , a->k + b->k, 0.0);
      for(channel = 0; channel < a->k;channel++) /*adding a*/
      {
        for(row = 0; row < result->h;row++)
        {
          for(col = 0; col < result->w; col++)
          {
            set_val(result,row,col,channel,get_val(a,row,col,channel));
          }
        }
      }
      for(channel = 0; channel < b->k;channel++) /*adding b*/
      {
        for(row = 0; row < result->h;row++)
        {
          for(col = 0; col < result->w; col++)
          {
            set_val(result,row,col,channel + a->k,get_val(b,row,col,channel));
          }
        }
      }
    }
    else
    {
      exit(1);
      printf("[ip_mat_concat]Input error: dimensioni incongruenti");
    }

    return result;
}


ip_mat * ip_mat_mul_scalar(ip_mat *a, float c)
{
    int row, col, channel;
    ip_mat *new_mat;
    new_mat = ip_mat_create (a->h, a->w, a->k, c);
    for (channel=0; channel<a->k; channel++)
    {
        for (row=0; row<a->h; row++)
        {
            for (col=0; col<a->w; col++)
            {
                set_val(new_mat,row,col,channel,a->data[row][col][channel] * c);
                /*new_mat->data[x][y][z] = a->data[x][y][z] * c;*/
            }
        }
    }
    return new_mat;
}

ip_mat *  ip_mat_add_scalar(ip_mat *a, float c)
{
    int row, col, channel;
    ip_mat *new_mat;
    new_mat = ip_mat_create (a->h, a->w, a->k, c);
    for (channel=0; channel<a->k; channel++)
    {
        for (row=0; row<a->h; row++)
        {
            for (col=0; col<a->w; col++)
            {
                set_val(new_mat,row,col,channel,a->data[row][col][channel] + c);
                /*new_mat->data[x][y][z] = a->data[x][y][z] + c;*/
            }
            
        }
    }
    compute_stats(new_mat);
    return new_mat;
}
/*
* PARTE 2
*/

ip_mat * ip_mat_to_gray_scale(ip_mat * in)
{
  ip_mat *result = ip_mat_create(in->h, in->w, 3, 0.0);
  int row,col;
  float mean;

  for(row = 0; row < result->h; row++)
  {
    for(col = 0; col < result->w; col++)
    {
      mean = (get_val(in, row, col, 0) + get_val(in, row, col, 1) + get_val(in, row, col, 2)) / 3.0;
      set_val(result, row, col, 0, mean);
      set_val(result, row, col, 1, mean);
      set_val(result, row, col, 2, mean);
    }
  }
  return result;
}


ip_mat * ip_mat_corrupt(ip_mat * a, float amount){
    int channel, row, col;
    ip_mat *result = ip_mat_create(a->h, a->w, a->k, 0.0);
    for (channel=0; channel<(a->k); channel++)
    {
        for (row=0; row<(a->h); row++)
        {
            for (col=0; col<(a->w); col++)
            {
                set_val(result, row, col, channel, (get_normal_random()*(amount/3)+(get_val(a,row,col,channel))));
            }
        }
    }
    return result;
}

ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){
    int x, y, z;
    ip_mat *blend  = ip_mat_create(a->h, a->w, a->k, 0.0);
    for (z=0; z<a->k; z++)
    {
    if((a->h)!=(b->h) || (a->w)!=(b->w) || (a->k)!=(b->k)){
        printf("Errore ip_mat_mean!!!");
        exit(1);
    }else{
        int x, y, z;
        ip_mat *blend  = ip_mat_create(a->h, a->w, a->k, 0.0);
        for (x=0; x<a->h; x++)
        {
            for (y=0; y<a->w; y++)
            {
                for (z=0; z<a->k; z++)
                {
                    set_val(blend , x, y, z, (alpha * (get_val(a,x,y,z))) + (1-alpha)* (get_val(b,x,y,z)));
                }
            }
        }
        return blend;
    }
}

ip_mat * ip_mat_brighten(ip_mat * a, float bright)
{
    ip_mat *result;
    result = ip_mat_add_scalar (a, bright);
    normalize_rgb(result);
    return result;
}

void normalize_rgb(ip_mat *a) /*controllo se i valori sono nel range 0-255; se sono fuori range vengono portati a 0 (se sono negativi) e a 255 (se sono >255) */
{
    int x, y, z;
    for (x=0; x<a->h; x++)
    {
        for (y=0; y<a->w; y++)
        {
            for (z=0; z<a->k; z++)
            {
                if(a->data[x][y][z]  >255)
                {
                    set_val(a,x,y,z,255);
                }
                if (a->data[x][y][z] <0)
                {
                    set_val(a,x,y,z,0);
                }
}
