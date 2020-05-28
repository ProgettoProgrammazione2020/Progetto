/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

/*
*FUNZIONI PRINCIPALI
*/
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k)
{
    if((i < a->h) && (j < a->w) && (k < a->k)){
        return a->data[i][j][k];
    }else{
        printf("[get_val]Errore: coordinate inserite non corrette\n");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v)
{
    if(i < a->h && j < a->w && k < a->k){
        a->data[i][j][k]=v;
    }else{
        printf("[set_val]Errore: coordinate inserite non corrette\n");
        exit(1);
    }
}

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

float get_normal_random(float media, float std){

    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float num = cos(2*PI*y2)*sqrt(-2.*log(y1));

    return media + num*std;
}

/*
*FUNZIONI AUSILIARIE
*/

float calculate_convolve(ip_mat * a, ip_mat * f, unsigned int row_start, unsigned int col_start, unsigned int selected_channel)
{
  unsigned int row, col;
  float sum = 0.0;

  for(row = 0; row < f->h; row++)
  {
    for(col = 0; col < f->w; col++)
    {
      sum += get_val(a, row + row_start, col + col_start, selected_channel) * get_val(f, row, col, selected_channel);
    }
  }

  return sum;
}

void control_malloc (int a){
    if(a){
        printf("Errore: malloc non riuscita!\n");
        exit(1);
    }
}

/*
* PARTE 1
*/

void compute_stats(ip_mat * t){
    float max,min,somma = 0.0;
    unsigned int row,col,channel;
    if(t!=NULL){
        for(channel=0;channel<(t->k);channel++)
        {
            max=get_val(t,0,0,channel);
            min=get_val(t,0,0,channel);
            for(row=0;row<(t->h);row++)
            {
                for(col=0;col<(t->w);col++)
                {
                    if(max<(get_val(t,row,col,channel))){
                        max=get_val(t,row,col,channel);
                    }
                    if(min>(get_val(t,row,col,channel))){
                        min=get_val(t,row,col,channel);
                    }
                    somma+=get_val(t,row,col,channel);

                }
            }
            t->stat[channel].min=min;
            t->stat[channel].max=max;
            t->stat[channel].mean=(somma/(float)((t->h)*(t->w)));
            somma=0.0;

        }
    }else{
        printf("[compute_stats] Errore: puntatore a NULL!\n");
        exit(1);
    }
}
/*Accetta valori compresi tra 0 e h-1 e tra 0 e w-1*/
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){

    if(t==NULL || (row_end)>(t->h-1) || (col_end)>(t->w-1) || (row_start>row_end) || (col_start>col_end)){
        printf("[ip_mat_subset] Errore: dimensioni errate (inserire valori compresi tra 0 e h-1 e tra 0 e w-1) o puntatore a NULL!\n");
        exit(1);
    }else{
        unsigned int row,col,channel;
        ip_mat *r=ip_mat_create((row_end-row_start)+1,(col_end-col_start)+1,(t->k),0.0);

        for(channel=0;channel<(t->k);channel++)
        {
            for(row=0;row<=row_end-row_start;row++)
            {
                for(col=0;col<=col_end-col_start;col++)
                {
                    set_val(r,row,col,channel,get_val(t,(row_start+row),(col_start+col),channel));
                }
            }
        }
        return r;
    }
}



ip_mat * ip_mat_copy(ip_mat * in){
    unsigned int channel, row, col;
    ip_mat *r;
    if(in!=NULL){
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
    }else{
        printf("[ip_mat_copy] Errore: puntatore a NULL!\n");
        exit(1);
    }
}

void ip_mat_init_random(ip_mat * t, float mean, float var){
    unsigned int channel, row, col;
    if(t!=NULL){
        for (channel=0; channel<(t->k); channel++)
        {
            for (row=0; row<(t->h); row++)
            {
                for (col=0; col<(t->w); col++)
                    set_val(t,row,col,channel,get_normal_random(mean,var));
            }
        }
    }else{
        printf("[ip_mat_copy] Errore: puntatore a NULL!\n");
        exit(1);
    }
}

ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){
    unsigned int channel, row, col;
    ip_mat *r;
    if(a==NULL || b==NULL || (a->h)!=(b->h) || (a->w)!=(b->w) || (a->k)!=(b->k)){
        printf("[ip_mat_mean] Errore: dimensioni incongruenti o puntatore a NULL!\n");
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
    unsigned int row, col;
    if(a!=NULL){ /*non si interrompe*/
        for (row=0; row<(a->h); row++)
        {

            for (col=0; col<(a->w); col++)
            {
                free(a->data[row][col]);
            }
            free(a->data[row]);
        }
        free(a->data);
        free(a->stat);
        free(a);
    }
}

ip_mat * ip_mat_create(unsigned int h, unsigned int w,unsigned  int k, float v)
{
    unsigned int x, y, z;
    ip_mat *result;
    if(h!=0 && w!=0 && k!=0){
        result = (ip_mat*)malloc(sizeof(ip_mat));
        control_malloc(result==NULL);
        result->w = w;
        result->h = h;
        result->k = k;
        result->data = (float***)malloc(h * sizeof (float**));
        control_malloc(result->data==NULL);
        for (x=0; x<h; x++)
        {
            result->data[x] = (float**)malloc(w * sizeof(float*));
            control_malloc(result->data[x]==NULL);
            for (y=0; y<w; y++)
            {
                result->data[x][y] = (float*)malloc(k*sizeof(float));
                control_malloc(result->data[x][y]==NULL);
                for (z=0; z<k; z++)
                    result->data[x][y][z] = v;
            }
        }
        result->stat = (stats*)malloc(k*sizeof(stats));
        control_malloc(result->stat==NULL);
        return result;
    }else{
        printf("[ip_mat_create] Errore: dimensioni uguali a 0!\n");
        exit(1);
    }
}

ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b)
{
  unsigned int channel, row, col;
  ip_mat *result;

  if(a!=NULL && b!=NULL && ((a->w == b->w) && (a->h == b->h) && (a->k == b->k)))
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
      printf("[ip_mat_sum]Le immagini sono di dimensioni diverse o puntatore a NULL!");
      exit(1);
  }

  return result;

}

ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b)
{
    unsigned int channel, row, col;
    ip_mat *result;

    if(a!=NULL && b!=NULL && ((a->w == b->w) && (a->h == b->h) && (a->k == b->k)))
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
        return result;
    }
    else
    {
        printf("[ip_mat_sub]Le immagini sono di dimensioni diverse o puntatore a NULL!\n");
        exit(1);
    }
}


ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione)
{
    unsigned int channel, row, col;
    ip_mat *result;
    if(a!=NULL && b!=NULL){
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
        }else{
            printf("[ip_mat_concat]Input error: dimensioni incongruenti!\n");
            exit(1);
        }
    }else{
        printf("[ip_mat_concat]Input error: puntatori a NULL!\n");
        exit(1);
    }

    return result;
}


ip_mat * ip_mat_mul_scalar(ip_mat *a, float c)
{
    unsigned int row, col, channel;
    ip_mat *new_mat;
    if(a!=NULL){
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
    }else{
        printf("[ip_mat_mul_scalar]Input error: puntatori a NULL!\n");
        exit(1);
    }
}

ip_mat *  ip_mat_add_scalar(ip_mat *a, float c)
{
    unsigned int row, col, channel;
    ip_mat *new_mat;
    if(a!=NULL){
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
    }else{
        printf("[ip_mat_add_scalar]Input error: puntatori a NULL!\n");
        exit(1);
    }
}
/*
* PARTE 2
*/

ip_mat * ip_mat_to_gray_scale(ip_mat * in)
{
    ip_mat *result;
    unsigned int row,col;
    float mean;
    if(in!=NULL){
        result = ip_mat_create(in->h, in->w, 3, 0.0);
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
    }else{
        printf("[ip_mat_to_gray_scale]Input error: puntatori a NULL!\n");
        exit(1);
    }
}


ip_mat * ip_mat_corrupt(ip_mat * a, float amount){
    unsigned int channel, row, col;
    ip_mat *result;
    if(a!=NULL){
        result = ip_mat_create(a->h, a->w, a->k, 0.0);
        for (channel=0; channel<(a->k); channel++)
        {
            for (row=0; row<(a->h); row++)
            {
                for (col=0; col<(a->w); col++)
                {
                    set_val(result, row, col, channel, get_val(a,row,col,channel)+(get_normal_random(0,amount/2)));/*o get_normal_random(get_val(a,row,col,channel),amount/2)*/
                }
            }
        }
        return result;
    }else{
        printf("[ip_mat_corrupt]Input error: puntatori a NULL!\n");
        exit(1);
    }
}

ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){
    unsigned int row,col,channel;
    ip_mat *blend;
    if((a==NULL || b==NULL) || ((a->h)!=(b->h) || (a->w)!=(b->w) || (a->k)!=(b->k))){
        printf("[ip_mat_blend]Errore: dimensioni incongruenti o puntatori a NULL!\n");
        exit(1);
    }else{
        blend = ip_mat_create(a->h, a->w, a->k, 0.0);
        for (row=0; row<a->h; row++)
        {
            for (col=0; col<a->w; col++)
            {
                for (channel=0; channel<a->k; channel++)
                {
                    set_val(blend , row, col, channel, (alpha * (get_val(a,row,col,channel))) + (1-alpha)* (get_val(b,row,col,channel)));
                }
            }
        }
        return blend;
    }
}

ip_mat * ip_mat_padding(ip_mat * a, unsigned int pad_h, unsigned int pad_w)
{
    ip_mat *pad_top, *pad_right_left, *pad_bottom, *result, *appoggio;
    if(a!=NULL){
        pad_top = ip_mat_create(pad_h, a->w, a->k, 0.0);
        pad_right_left = ip_mat_create (a->h+pad_h, pad_w, a->k, 0.0);
        pad_bottom = ip_mat_create (pad_h, a->w + 2*(pad_w), a->k, 0.0);

        appoggio = ip_mat_concat (pad_top, a, 0);
        result = ip_mat_concat (pad_right_left, appoggio, 1);

        ip_mat_free (appoggio);

        appoggio = result;
        result = ip_mat_concat (appoggio, pad_right_left, 1);

        ip_mat_free (appoggio);

        appoggio = result;
        result = ip_mat_concat (appoggio, pad_bottom, 0);

        ip_mat_free (appoggio);
        ip_mat_free (pad_top);
        ip_mat_free (pad_right_left);
        ip_mat_free (pad_bottom);
        return result;
    }else{
        printf("[ip_mat_padding]Input error: puntatori a NULL!\n");
        exit(1);
    }

}

ip_mat * ip_mat_brighten(ip_mat * a, float bright)
{
    ip_mat *result;
    if(a!=NULL){
        result = ip_mat_add_scalar (a, bright);
        return result;
    }else{
        printf("[ip_mat_brighten]Input error: puntatori a NULL!\n");
        exit(1);
    }
}

ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f)
{
    unsigned int channel, row, col;

    ip_mat *aux; 
    ip_mat *result;
    if(a!=NULL && f!=NULL){
        aux = ip_mat_padding(a, (f->h - 1) / 2, (f->w - 1) / 2);
        result = ip_mat_create(a->h, a->w, a->k, 0.0);

        for(channel = 0; channel < aux->k; channel++)
        {
            for(row = 0; row + f->h < aux->h; row++)
            {
            for(col = 0; col + f->h < aux->w; col++)
            {
                set_val(result, row, col, channel, calculate_convolve(aux, f, row, col, channel));
            }
            }
        }

        ip_mat_free(aux);
        return result;
    }else{
        printf("[ip_mat_convolve]Input error: puntatori a NULL!\n");
        exit(1);
    }
}

ip_mat * create_sharpen_filter()
{
  unsigned int channel;
  ip_mat *result = ip_mat_create(3,3,3,0.0);

  for(channel = 0; channel < 3; channel++)
  {
    set_val(result, 0, 0, channel, 0.0); set_val(result, 0, 1, channel, -1.0); set_val(result, 0, 2, channel, 0.0);
    set_val(result, 1, 0, channel, -1.0); set_val(result, 1, 1, channel, 5.0); set_val(result, 1, 2, channel, -1.0);
    set_val(result, 2, 0, channel, 0.0); set_val(result, 2, 1, channel, -1.0); set_val(result, 2, 2, channel, 0.0);
  }

  return result;
}

ip_mat * create_edge_filter()
{
  unsigned int channel;
  ip_mat *result = ip_mat_create(3,3,3,0.0);

  for(channel = 0; channel < 3; channel++)
  {
    set_val(result, 0, 0, channel, -1.0); set_val(result, 0, 1, channel, -1.0); set_val(result, 0, 2, channel, -1.0);
    set_val(result, 1, 0, channel, -1.0); set_val(result, 1, 1, channel, 8.0); set_val(result, 1, 2, channel, -1.0);
    set_val(result, 2, 0, channel,-1.0); set_val(result, 2, 1, channel, -1.0); set_val(result, 2, 2, channel, -1.0);
  }

  return result;
}

ip_mat * create_emboss_filter()
{
  unsigned int channel;
  ip_mat *result = ip_mat_create(3,3,3,0.0);

  for(channel = 0; channel < 3; channel++)
  {
    set_val(result, 0, 0, channel, -2.0); set_val(result, 0, 1, channel, -1.0); set_val(result, 0, 2, channel, 0.0);
    set_val(result, 1, 0, channel, -1.0); set_val(result, 1, 1, channel, 1.0); set_val(result, 1, 2, channel, 1.0);
    set_val(result, 2, 0, channel, 0.0); set_val(result, 2, 1, channel, 1.0); set_val(result, 2, 2, channel, 2.0);
  }

  return result;
}

ip_mat * create_average_filter(unsigned int w,unsigned int h,unsigned int k)
{
    float c;
    ip_mat *result;
    if(w!=0 && h!=0 && k!=0){
        c=1.0/(w*h);
        result = ip_mat_create(h,w,k,c);

        return result;
    }else{
        printf("[create_average_filter]Input error: dimensioni uguali a 0!\n");
        exit(1);
    }
}

void rescale(ip_mat * t, float new_max)
{
    unsigned int channel, row, col;
    float appoggio;
    if(t!=NULL){
        appoggio = 0;
        compute_stats(t);
        for(channel = 0; channel < t->k; channel++)
        {
            for(row = 0; row < t->h; row++)
            {
                for(col = 0; col < t->w; col++)
                {
                    appoggio = (get_val(t,row, col, channel) - t->stat[channel].min) / ((t->stat[channel].max) - (t->stat[channel].min));
                    set_val(t,row,col,channel, appoggio*new_max);
                }

            }
        }
    }else{
        printf("[rescale]Input error: puntatore a NULL!\n");
        exit(1);
    }
}

void clamp(ip_mat * t, float low, float high)
{
    unsigned int channel, row, col;
    float appoggio;
    if(t!=NULL){
        if(low>high){ /*scambiamo le due variabili se inserite al contraio*/
            appoggio=low;
            low=high;
            high=appoggio;
        }
        appoggio = 0;
        for(channel = 0; channel < t->k; channel++)
        {
            for(row = 0; row < t->h; row++)
            {
                for(col = 0; col < t->w; col++)
                {
                    appoggio = get_val(t,row, col, channel);
                    if(appoggio > high)
                    {
                        set_val(t,row,col,channel,high);
                    }else if (appoggio < low)
                    {
                        set_val(t,row,col,channel,low);
                    }
                }

            }
        }
    }else{
        printf("[clamp]Input error: puntatore a NULL!\n");
        exit(1);
    }
}

ip_mat * create_gaussian_filter(unsigned int w, unsigned int h, unsigned int k, float sigma){
    unsigned int channel, row, col;
    unsigned int i,j;
    float sum=0.0;
    float op,x,y;
    ip_mat *result;
    if(sigma==0 || w==0 || h==0 || k==0){
        printf("[create_gaussian_filter] Errore: sigma non può essere 0 o dimensioni uguali a 0!\n");
        exit(1);
    }else{
        result = ip_mat_create(h,w,k,0.0);
        i=h/2;
        j=w/2;
        for(channel = 0; channel < k; channel++)
        {
            sum=0.0;
            for(row = 0; row < h; row++)
            {
                for(col = 0; col < w; col++)
                {
                    x=row-i;
                    y=col-j;
                    op=(1/(2*PI*sigma))*exp(-((pow(x,2)+pow(y,2))/(2*pow(sigma,2))));
                    set_val(result,row,col,channel,op);
                    sum=sum+op;
                }
            }
        }
        for(channel = 0; channel < k; channel++)
        {
            for(row = 0; row < h; row++)
            {
                for(col = 0; col < w; col++)
                {
                    set_val(result,row,col,channel,(1/sum)*(get_val(result,row,col,channel)));
                }
            }
        }
        return result;
    }
}
