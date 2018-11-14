#include <stdio.h>
#include <stdlib.h>
#include "seq.h"
#include "segments.h"

#define Seq_T Seg_T

int main() 
{
        int arr[10] = {0, 1, 2, 3, 4, 5 ,6 ,7 ,8 ,9};
        int arr2[5] = {10, 11 ,12 ,13 ,14};
        /* Test creatomg segments structure */
        Seg_T instructions = Seg_new(2);
        
        for(int i = 0; i < 10; i++) {
                printf("%d ", arr[i]);
        }
        printf("\n");

        /* Test inserting value into segments structure */
        Seg_addlo(instructions, &arr);
        Seg_addhi(instructions, &arr2);
        for (int i =0 ; i < 10; i++) {
                /* Testing extraction of value in segments structure */
                int (*temp)[10] = (int (*)[10])Seg_get(instructions, 0);
                printf("%d ", (*temp)[i]);
        }
        printf("\n");

        /* Test retrieval of segments structure length */
        printf("%d \n", Seg_length(instructions));
        
        /* Test put values in to segments structure */
        void * temp1 = Seg_put(instructions, 1, &arr2);
        (void) temp1;
        for (int i =0 ; i < 5; i++) {
                int (*temp)[5] = (int (*)[5])Seg_get(instructions, 1);
                printf("%d ", (*temp)[i]);
        }
        printf("\n");
        printf("%d \n", Seg_length(instructions));
        

        Seg_T stack = Seg_new(2);

        /* Unmap a Segment */
        Seg_unmapp(instructions, stack, 0);
        int arr3[2] = {1, 2};
        
        /* Map new array to segment that was unmapped */
        Seg_map(instructions, stack, &arr3);

        /* Test freeing segments structure */
        Seg_free(instructions);
        Seg_free(stack);
        
        return 0;
}
