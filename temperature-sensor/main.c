#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <ctime>

#include "main.h"
//#include "readTemperature.c"
#include "pushppandhttppost.c"
// #include "httppost/chap06.h"
// #include "httppost/web_get.c"


void delay_ms(int number_of_milliseconds)
{
    // Converting time into milli_seconds
    //int milli_seconds = 1000 * number_of_milliseconds;
    
    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + number_of_milliseconds)
        ;
}

void display_read_reading(char read_value)
{
    // Print the file content
    

    //printf("%s", read_value);
    printf("%i", read_value);

    //printf("\n Hello!");

    // keep console open
    //getchar();

}


int getTempinInt(int linecount)
{
    FILE* file = fopen ("temperature2.txt", "r");
    int i = 0;

    fscanf (file, "%d", &i);
    for (int p=0; p<linecount; p++)
    {
        fscanf (file, "%d", &i);      
    }
    fclose (file);

    return i;
}


void make_and_send_values(double max_val, double min_val, double avg_val)
{
    printf("\n maxT: %.02f, minT: %.02f, avgT: %.02f.", max_val, min_val, avg_val);

/*```
// TemperatureMeasurement
{
	"time": {
		"start": string, // Start date and time in ISO8601 format for the measurement
		"end": string // End date and time in ISO8601 format for the measurement
	},
	"min": number, // Minimum observed temperature
	"max": number, // Maximum observed temperature
	"average": number // Average temperature
}
``` */





//TODO: make a push-pop to store readings in case.
//store reading to array
//transmit 1st reading in array: to POST function
//check if transmit successful
//reduce array pointer by 1: if transmit successful
//if array pointer > 1 : goto transmit 1st reading again
//if array pointer is <=0, set array pointer to 0.
//increase array pointer by 1: if transmit unsuccessful


//TODO: Read time and make ISO:8601 format
//TODO: make JSON format of data to transmit
//TODO: send as HTTP POST request.


char request[] = "POST /mes/rest/r2Label/print/Biesse/105/1B6D0X-----11225105/112251/Q0161B8 HTTP/1.1\r\nHost: meswebapptest.hermanmiller.com\r\nContent-Lenght:9\r\n\r\nname=nina";
}


//double getTemperature();
//double getTemperature(int);
int getTempinInt(int);
int main()
{
    static double gotTemperature = 0;

    static double minTemperature = 5000; //arbitary big number. TODO: Read first value from file

    static double maxTemperature = 0;

    static double avgTemperature = 0;

    static double temperture_table[(UPLOAD_INTV_MS/READ_DELAY_MS) +1];

    static int temperature_file_line_count = 0;

    static int time_elapsed = 0;
    
    printf("Start!");

    while (1)
    {
        //****************read temperature from file (increment line number on every read)
       gotTemperature = getTempinInt(temperature_file_line_count++);

        //****************display_read_reading(gotTemperature);
        printf("\n temperature read: %.02f", gotTemperature);

        //**************** store value for calculation and reporting.
//        temperture_table[time_elapsed] = gotTemperature;
        
        //****************Calculate max, min and average temperature
        if (gotTemperature < minTemperature) minTemperature = gotTemperature;

        if (gotTemperature > maxTemperature) maxTemperature = gotTemperature;
        
        avgTemperature = avgTemperature + gotTemperature;
        
        printf (". time elapsed: %d / %d.", time_elapsed, UPLOAD_INTV_MS/READ_DELAY_MS);
        //printf("\n maxT, minT, avgT: %.02f, %.02f, %.02f", maxTemperature, minTemperature, avgTemperature);

        if (time_elapsed >= (UPLOAD_INTV_MS/READ_DELAY_MS))
        {
            time_elapsed = 0;
            printf("\n two minutes completed");

            avgTemperature = avgTemperature / (UPLOAD_INTV_MS/READ_DELAY_MS);

            make_and_send_values(maxTemperature, minTemperature, avgTemperature);

            //printf("\n maxT, minT, avgT: %.02f, %.02f, %.02f", maxTemperature, minTemperature, avgTemperature);
            
            minTemperature = gotTemperature;
            maxTemperature = gotTemperature;
            avgTemperature = gotTemperature;
        }

        //**************** wait 100 ms
        delay_ms(READ_DELAY_MS);  
      
        time_elapsed++;
        //temperature_file_line_count++;
        //temperature_file_line_count++;
    }

return 0;

}

/****************TEST CODES****************
 * 
        //gotTemperature = getTemperature(temperature_file_line_count);   //read_one_reading();

 * 
 int getTempinInt(int linecount)
{
    FILE* file = fopen ("temperature2.txt", "r");
    int i = 0;

    fscanf (file, "%d", &i);
    // while (!feof (file))
    // {
    //     printf ("%d ", i);
    //     fscanf (file, "%d", &i);      
    // }
    for (int p=0; p<linecount; p++)
    {
        //printf ("%d ", i);
        fscanf (file, "%d", &i);      
    }
    fclose (file);
    return i;
}
 * 
        delay_ms(READ_DELAY_MS);  // wait 100 ms
        // for (int i=0; i<100; i++)
        // {
        //     delay_ms(1);
        //     //printf("\n waiting, %i", i);
        // }
        // printf ("\n reading.");    

 */