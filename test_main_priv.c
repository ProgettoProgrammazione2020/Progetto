#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"
#include <string.h>

#define COUNT_FILTER 5
#define COUNT_SOURCE 5
#define DIRECTORY "img/"


void get_name(char const *source, char const *prefix, char *name)
{
  int i;
  for(i = 0; i < strlen(prefix); i++)
  {
    name[i] = prefix[i];
  }
  for(i = strlen(prefix); i < strlen(prefix) + strlen(source); i++)
  {
    name[i] = source[i - strlen(prefix)];
  }
  name[i] = 0;
}

void test(char const *source, char const *file_name, int select)
{
  char src[40] = "";
  ip_mat *test_ipmat = NULL, *result = NULL, *filter = NULL;
  Bitmap *test_Bitmap = NULL, *result_ = NULL;

  get_name(source, DIRECTORY, src);

  printf("src: %s\n",src);

  test_Bitmap = bm_load(src); /*carico il file*/

  test_ipmat = bitmap_to_ip_mat(test_Bitmap);/*converto in ip_mat*/

  switch(select)/*FACCIO ROBA*/
  {
    case 0: filter = create_sharpen_filter();
            break;
    case 1: filter = create_edge_filter();
            break;
    case 2: filter = create_emboss_filter();
            break;
    case 3: filter = create_average_filter(3,3,3);
            break;
    case 4: filter = create_gaussian_filter(9,9,3,5.0);
            break;
  }

  result = ip_mat_convolve(test_ipmat,filter);

  clamp(result,0.0,255.0);

  result_ = ip_mat_to_bitmap(result); /*converto in bitmap*/

  ip_mat_free(test_ipmat);/*LIBERAZIONE MEMORIA*/
  ip_mat_free(result);
  ip_mat_free(filter);
  bm_free(test_Bitmap);

  bm_save(result_,file_name);/*salvo l'immagine su filter*/
  bm_free(result_);

}

int main(){
    int i, j;
    char name[50];
    char source[][30] = {"flower.bmp", "fullmoon.bmp", "flower2.bmp", "caf.bmp", "mongolfiere.bmp"};
    char filter[][30] = {"sharpen_", "edge_", "emboss_", "average_", "gaussian_"};
    for(j = 0; j < COUNT_SOURCE; j++)
    {
      for(i = 0; i < COUNT_FILTER; i++)
      {
        get_name(source[j], filter[i], name);
        printf("%s --> %s\n", source[j], name);
        test(source[j], name, i);
      }
    }
    return 0;

}
