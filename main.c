/*******************************************************************************
                        EMBEDDED SYSTEMS DESIGN
                            LAB 3 PART 1
                    IDE:CODE BLOCKS,COMPILER:SDCC
                       RUCHA BORWANKAR (RUBO1268)
**********************************************************************************/

/*********HEADER FILES***********/
#include <at89c51ed2.h>       //also includes 8052.h and 8051.h
#include <mcs51reg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include<string.h>
#define DEBUG

/****** Define heap size ******/
#define HEAP_SIZE 4000
__xdata char __sdcc_heap[HEAP_SIZE];
const unsigned int __sdcc_heap_size = HEAP_SIZE;

/***Debug***/
#ifdef DEBUG
#define DEBUGPORT(x) *((__xdata uint8_t *)0xFF02) = x// generates a MOVX 0FFFFh,x where x is an 8-bit value
#else
#define DEBUGPORT(x)  // empty statement, nothing passed on from the preprocessor to the compiler
#endif

/* -------------------------------------------------- */
//          GLOBALS
/* -------------------------------------------------- */
volatile int8_t gg = 0;
/* -------------------------------------------------- */
//          FUNCTION DEFINITIONS
/* -------------------------------------------------- */

// Note: In a function file, each function should have a small explanation of its args and returns

// putchar takes a char and TX's it. Blocking. No return value.

/* -------------------------------------------------- */
//     /*Function for printing a character*/
/* -------------------------------------------------- */

int putchar (int c)
{
    // Note: Compare the asm generated for the next three lines
    //       They all accomplish the same thing, but is the asm the same?
    while (!TI);
    //while (TI == 0);
    //while ((SCON & 0x02) == 0);

    SBUF = c;           // load serial port with transmit value
    TI = 0;             // clear TI flag

    return c;
}
/* -------------------------------------------------- */
//   Function for taking a character from the user
/* -------------------------------------------------- */

int getchar (void)
{
    // Note: Compare the asm generated for the next three lines
    //       They all accomplish the same thing, but is the asm the same?
    while (!RI);
    //while ((SCON & 0x01) == 0);
    //while (RI == 0);

    RI = 0;                         // clear RI flag
    return SBUF;                    // return character from SBUF
}

/* -------------------------------------------------- */
//   Function to print string
/* -------------------------------------------------- */
int putstr (char *s)
{
    int i = 0;
    while (*s){            // output characters until NULL found
        putchar(*s++);
        i++;
    }
    return i+1;
}

/** -------------------------------------------------- *
          Function to get input from user
 -------------------------------------------------- */
int getstr(void)
{
    char a;
    char *b = NULL;
    uint16_t x;
    //while (47<a<58)
    //{
    for(int i=0;i<4;i++)
        {
            a=getchar();
            if(a==0x0d)
             {
                 b[i] = '\0';
                break;
             }
            else if((a>='0') && (a<='9'))
             {
                b[i]=a;
                //printf_tiny("%c",b[i]);
             }
             else
             {
                break;
             }
        }
        x=atoi(b);
        //x = 3;
        printf("\n \r %d \n",x);
    //}
    return x;
}

/* ------------------------------------------------------------------------- */
//                  Main function
/* ------------------------------------------------------------------------- */

int main()
{
printf("trial\n\r");
/******** INITIALIZATION ***********/
    __xdata uint8_t *buffer_0=0;
    __xdata uint8_t *buffer_1=0;
    __xdata uint8_t *base0;
    __xdata uint8_t *base1;
    uint16_t *bufferstart[140];         /***maximum buffers that can be created by + operation between 30 to 300**/
    uint16_t buffer_size[100];          /**buffer sizes are stored**/
    uint8_t *bufferend[100];            /**end addresses are stored**/
    uint8_t var;                        /**infinite loop breaking variable**/
    uint16_t b;                         /**variable that takes buffer size from user**/

/**BUFFER 0 AND BUFFER 1 MALLOC FUNCTION**/

while (1){
do{
    putstr("\n \r Enter number of bytes for buffer in range of 32 and 3200\n");
    b = getstr();
    uint16_t *endadd0,*endadd1;
    printf("\n \r The Value entered is %d",b);
    if((b>31 && b<3201) && (b%16==0))
    {
        buffer_0 = (uint8_t*) malloc(b);
        //base0=(uint8_t*)buffer_0;
        bufferstart[0]=buffer_0;
        endadd0=buffer_0+b;
        bufferend[0]=*buffer_0+endadd0;
        buffer_size[0]= b;
        //printf("%p %p",base0,buffer_0);
        if (buffer_0 == NULL)
            {
                putstr("\n \r BUFFER 0 cannot be allocated\n");
            }
        else
            {
                printf("\n\r BUFFER 0 is successfully allocated using malloc at address %p and end address as %p\n\r",buffer_0,endadd0);
            }
    }
    else
    {
            putstr("\n \r Re-enter number of bytes for buffer in range of 32 and 3200");
    }

/** -------------------------------------------------- *
          Function to check for Buffer 1
 -------------------------------------------------- */

    if(b<2000)
    {
        buffer_1 = (int*) malloc(b);
        //base1=(uint8_t*)buffer_1;
        bufferstart[1]=buffer_1;
        if (buffer_1 == NULL)
            {
                putstr("\n \r Memory not allocated in heap\n");
            }
        else
            {
                endadd1=buffer_1+b;
                bufferend[1]=*buffer_1+endadd1;
                buffer_size[1]= b;
                printf("\n\rBuffer 1 successfully allocated using malloc at address %p with end address at %p\n\r",buffer_1,endadd1);
            }
    }
    else
    {
        free(buffer_0);
        putstr("\n \r Buffer 0 cleared,please enter smaller buffer size\n \r");
    }

}while(((buffer_0 == 0) || (buffer_1 == 0)));

/**Initialization***/
uint16_t countm=0;              /***count for all the inputs***/
uint8_t countz=0;              /**count for a to z letter inputs***/
//pr`intf_tiny("countz value is %d",countz);
uint16_t countb=2;              /****count for number of buffers created*****/
uint8_t i=2,j=0;
uint16_t dec=b;
/***infinite loop for getting character**/
 while (var=1)
{
    putstr("\n \r ******************************************************************************************************* \n");
    putstr("\n \r              PLEASE ENTER SPECIFIC CHARACTER FOR FOLLOWING OPERATIONS        \n \r");
    putstr("\n \r ******************************************************************************************************\n \r");
    putstr("\n \r                1)      '+'       -   CREATE NEW BUFFERS                               \n \r");
    putstr("\n \r                2)      '-'       -   DELETE A BUFFER                                   \n \r");
    putstr("\n \r                3)     'a to z'   -   STORE THE LOWER CASE CHARACTER                    \n \r");
    putstr("\n \r                4)      '?'        -   INFORMATION ABOUT HEAP AND EMPTY BUFFER 0         \n \r");
    putstr("\n \r                5)      '='        -   PRINT VALUES IN BUFFER 0                          \n \r");
    putstr("\n \r                6)      '@'        -   FREE ALL BUFFERS AND RESTART                     \n \r");
    putstr("\n \r ********************************************************************************************************\n \r");

     char ip=getchar();             /**Get character from user*/
     putchar(ip);
     countm++;

      /***Check for a to z condition**/
     if(ip>='a' && ip<='z')
     {
         if(countz<b)
            {
                buffer_0[j]=ip;
                j++;
               printf("\n \r Buffer_0 stores the value %c ",ip);
                countz++;
                dec--;
            }
         else
            {
                printf_tiny("\n \r Buffer0 full ,please clear buffer\n \r");
            }
     }
     /*** + operation to create buffers**/
     else if(ip == '+')
    {
        DEBUGPORT(0x02);
        putstr("\n \r Enter the size for buffer between 30 and 300 bytes :\n \r");
        uint16_t size=getstr();
            if(size>29 && size<301)
            {
                __xdata uint8_t *buffer_n = (int*) malloc(size);        /*memory allocated*/
                uint8_t *basen=buffer_n;
                    if (buffer_n == NULL)
                        {
                            putstr("\n \r BUFFER cannot be allocated\n");
                        }
                    else
                        {
                            printf("\n\r BUFFER %d is successfully allocated using malloc at address %p at d value is %d\n\r",i,buffer_n,buffer_n);
                            bufferstart[i]=buffer_n;
                            bufferend[i]=buffer_n+size;
                            buffer_size[i]=size;
                            countb++;
                            printf("array i has value %p %p",bufferstart[i],bufferend[i]);
                            i++;
                        }
            }
            else
            {
                putstr("\n \r Error,please put value between 30 to 300 bytes\n \r");
            }
    }
     /***Check for - condition to free selective buffer**/
    else if(ip == '-')
        {
            putstr("\n \r Enter the buffer number to be deleted : \n \r");
            uint8_t size1=getstr();
            if(size1 <= i)
              {
                free(bufferstart[size1]);
                printf("\n \r Clearing buffer at address %p",bufferstart[size1]);
                countb--;
                for(uint8_t d=size1;d<countb;d++)
                {
                    bufferstart[d]=bufferstart[d+1];
                    bufferend[d]=bufferend[d+1];
                    buffer_size[d]=buffer_size[d+1];
                }

              }
                else
                {
                    putstr("\n \r out of bounds,please enter correct value \n \r");
                }

        }
        /**Check '=' condition to display buffer 0 elements**/
    else if(ip == '=')
    {
        printf("\n \r The buffer 0 has following data: \n \r");
        if(countz>0)
        {
            for(uint8_t c=0;c<countz;c++){
            if(c % 15==0)
                {
                    printf_tiny(" \n \r");
                    printf("%p:",buffer_0[c]);
                }
            else
                {
                    printf(" %x  ",buffer_0[c]);
                }
            }
        }
        else
        {
            printf_tiny("\n \r Cannot print small alphabets as buffer is empty \n \r");
        }
    }
/**Check ? operation to display all heap information and clear buffer 0 element**/
    else if(ip=='?')
    {
        DEBUGPORT(0x02);
            putstr("\n \r *******************************************Details of buffer are as follows:************************************************* \n \r");
            putstr("\n \r Buffer #      Buffer start address     Buffer end address        total size allocated       no. of storage characters in buffers     free space\n \r");
            printf("\n \r    0             %p                          %p                     %d                               %d                                %d           \n \r ",bufferstart[0],bufferend[0],buffer_size[0],countz,dec);


        for(uint8_t k=1;k<countb;k++)
        {
            printf("\n \r   %d              %p                          %p                     %d                                0                                %d       \n \r ",k,bufferstart[k],bufferend[k],buffer_size[k],buffer_size[k]);

        }
        for(uint8_t y=0;y<countz;y++)
        {
            if(y % 64==0)
            {
               printf_tiny("\n");
            }
            else
            {
                printf("%c  ",buffer_0[y]);
                buffer_0[y]=0;
            }
        }

    }
    /*****Free the entire heap using @ and restart the program *****/
    else if(ip == '@')
    {
        free(buffer_0);
        printf("\n\rBuffer 0 is freed from memory\n\r");
        for(uint8_t m=1;m<=countb;m++)
        {
            free(bufferstart[m]);
            printf_tiny("\n \r Buffer %d is freed from the memory \n \r",m);
        }
        printf("\n \r ********************************************Heap size is freed*********************************************** \n \r");
       //var=0;
       main();
    }
}
}
return 0;
}










