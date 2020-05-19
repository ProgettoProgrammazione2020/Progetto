#include "bmp.h"
#include <math.h>
#include <stdio.h>
#include "ip_lib.h"

#define COUNT_TEST 3

ip_mat * allocate_ipmat_array(int length);
Bitmap * allocate_Bitmap_array(int length);

void test(char const *source, char const *destination, int select)
{
  ip_mat *filter = NULL;

  ip_mat *test_ipmat = NULL, *result = NULL;
  Bitmap *test_Bitmap = NULL, *result_ = NULL;

  test_Bitmap = bm_load(source); /*carico il file*/

  test_ipmat = bitmap_to_ip_mat(test_Bitmap);/*converto in ip_mat*/

  switch(select)/*FACCIO ROBA*/
  {
    case 0: filter = create_sharpen_filter();
            break;
    case 1: filter = create_edge_filter();
            break;
    case 2: filter = create_emboss_filter();
            break;
  }

  result = ip_mat_convolve(test_ipmat,filter);

  result_ = ip_mat_to_bitmap(result); /*converto in bitmap*/

  ip_mat_free(test_ipmat);/*LIBERAZIONE MEMORIA*/
  ip_mat_free(result);
  ip_mat_free(filter);
  bm_free(test_Bitmap);

  bm_save(result_,destination);/*salvo l'immagine su filter*/
  bm_free(result_);

}

int main(){
    int i;
    char source[] = "flower.bmp";
    char destination[][30] = {"sharpen_flower.bmp", "edge_flower.bmp", "emboss_flower.bmp"};
    for(i = 0; i < COUNT_TEST; i++)
    {
      printf("%s --> %s\n", source, destination[i]);
      test(source, destination[i],i);
    }
    return 0;

}
