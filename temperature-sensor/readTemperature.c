#include <stdio.h>
#include <stdlib.h>

double getTemperatureOld(int file_line_number) //char read_one_reading()
{
    FILE *fptr;

    // Open a file in read mode
    fptr = fopen("temperature2.txt", "r");

//     // Store the content of the file
// //    char myString[100];
//     char myString;  //[100];

//     // Read the content and store it inside myString
// //    fgets(myString, 100, fptr);
//     fgets(myString, 1, fptr);

//     // Close the file
//     fclose(fptr);

    //printf ("\n getting temperaure.");
    printf("\n reading on line number %i: ", file_line_number);
    int count = 0;
    char line[8]; //[1];//[8];
    //char line[256]; /* or other suitable maximum line size */
    while (fgets(line, sizeof line, fptr) != NULL) /* read a line */
    {
        if (count == file_line_number)
        {
            //use line or in a function return it
            //in case of a return first close the file with "fclose(file);"
            fclose(fptr);

            printf("tempeature value: ");
            printf("%s, %.02f \n", line, atoi(line));//(((int)line) * 0.1f));
            // printf("%s", line);
            // printf("%f", (((int)line) * 0.1f)); 
            
            //DROPPED: Read as double float from file
            return 0;// (float)((int)line * 0.1f);//line;
        }
        else
        {
            count++;
        }
    }
    fclose(fptr);

    return -1;

    //return (double(myString));

}

