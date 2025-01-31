#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
// #include "readTemperature.c"
// #include "pushppandhttppost.c"
// #include "curl/include/curl/curl.h"
#include <curl/curl.h>


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

//read a single value from the text file
double getTemperature(int linecount)
{
    FILE* file = fopen ("temperature2.txt", "r");
    double i = 0;

    fscanf (file, "%d", &i);
    for (int p=0; p<linecount; p++)
    {
        int k =0;
        
        fscanf (file, "%d", &k);
        i = k; //convert value to double from integer
    }
    fclose (file);

    return i;
}

//Convert ADC value read from file to degree centigrade
/****THEORY****
 * Given:
 * ------
 * For example the ADC can read the following values from the sensor:
 *  - 2048 (rougly 0C)
 *  - 3000 (rougly 23C)
 * 
 * =>
 * --
 * 1 deg C ~= 41 units of ADC value.
 * 
 **************/
double convertTemptodegC (double rawtemp)
{
    // double degCTemp = 0;
    // degCTemp =  ((rawtemp - 2048.00) / 41.00);

    // printf ("Raw temperature= %.02f, Converted temperature= %.02f", rawtemp, degCTemp);
    return ((rawtemp - 2048.00) / 41.00);
}

void create_json_payload(Measurement messArr, float max_val, float min_val, float avg_val, char *json_payload) {
    snprintf(json_payload, JSON_BUFFER_SIZE, 
        "{"
        "\"time\": {\"start\": \"%s\", \"end\": \"%s\"},"
        "\"min\": %.2f, "
        "\"max\": %.2f, "
        "\"average\": %.2f"
        "}",
        messArr.startTm, messArr.endTm, min_val, max_val, avg_val
    );
}

//DROPPED: Function to send HTTP POST request using libcurl
int send_http_post(const char *url, const char *json_payload) {
    CURL *curl;
    CURLcode res;
    long response_code = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);  // Ignore response body

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Get HTTP response code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            printf("\nServer Response Code: %ld\n", response_code);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return (int)response_code;

}
/******TEST CODE**********
 * 
 * return 0;
 * 
 * 
 * 
 */

//struct tMessage messArr[60];
void make_and_send_values(char* startT, char* endT, double max_val, double min_val, double avg_val)
{

    //DONE: store readings to array
    //store readings to pipeline
    messArr[0].startT = startT;
    messArr[0].endT = endT;
    messArr[0].maxT = max_val;
    messArr[0].minT = min_val;
    messArr[0].avgT = avg_val;
    
    //DONE: make a push-pop to store readings in case.
    //update readings in the pipeline
    for (int j = 0; j<FAILBUFFERLENGTH; j++)
    {
        messArr[FAILBUFFERLENGTH - j].startT = messArr[FAILBUFFERLENGTH - j - 1].startT;
        messArr[FAILBUFFERLENGTH - j].endT = messArr[FAILBUFFERLENGTH - j - 1].endT;
        messArr[FAILBUFFERLENGTH - j].maxT = messArr[FAILBUFFERLENGTH - j - 1].maxT;
        messArr[FAILBUFFERLENGTH - j].minT = messArr[FAILBUFFERLENGTH - j - 1].minT;
        messArr[FAILBUFFERLENGTH - j].avgT = messArr[FAILBUFFERLENGTH - j - 1].avgT;        
    }

    printf ("\n Start time:  %s", messArr[0].startT);
    printf("\n maxT: %.02f, minT: %.02f, avgT: %.02f.", max_val, min_val, avg_val);
    printf ("\n End time:  %s", messArr[0].endT);

    
    //DONE:
    // ## Error handling
    // 5> It must be expected that the backend will fail quite often and return the HTTP status code 500.
    // All errors that could happen on the server side should be handled by temporarily storing the last produced TemperatureMeasurement.
    // 6> If a failure has occured the previously failed TemperatureMeasurement values should be sent on the next 2 minute interval.
    // 7> Upon failure, the last maximum 10 TemperatureMeasurement values should be sent to an alternative endpoint `http://localhost:5000/api/temperature/missing` in JSON array format, e.g.:

    //DONE: make JSON format of data to transmit
    Measurement timemessArr[1] = {
        {messArr[0].startT, messArr[0].endT}  
    };
    // BUG: Debug time stamp in json message.
    // printf ("\n%s, %s", messArr[0].startT, messArr[0].endT);
    // printf ("\n%s, %s", timemessArr[0].startTm, timemessArr[0].endTm);
    // while(1);

    create_json_payload(timemessArr[0], messArr[0].maxT, messArr[0].minT, messArr[0].avgT, json_payload);

    //DROPPED: send as HTTP POST request.
    int response_code;
    
    if (failed_transmission == 0)
    {
        response_code = send_http_post(url, json_payload);
    }
    else
    {
        response_code = send_http_post(url, failed_payload);
    }

    printf("\nJSON Payload: %s\n", json_payload);

    if (response_code == 200) {
        printf("\nData successfully sent to the server.\n");
        
        //reset failed counter
        failed_transmission = 0;
    }
    else
    {
        //increment failed counter        
        failed_transmission++;

        if (response_code == 500) 
        {
            printf("\nServer returned HTTP 500. Saving payload for retransmit...\n");
        }
        else
        {
            printf("\nUnexpected response code: %d\n", response_code);
        }

        //update failure payload
        printf("\nUpdating fai payload for next transmit\n");
        create_json_payload(timemessArr[0], messArr[0].maxT, messArr[0].minT, messArr[0].avgT, json_payload);
            printf("\nJSON Payload: %s\n", failed_payload);

        //DONE: 7> Upon failure, the last maximum 10 TemperatureMeasurement values should be sent to an alternative endpoint `http://localhost:5000/api/temperature/missing` in JSON array format, e.g.:
        printf("\n Sending last 10 Temperature Measurement values to alternate endpoint...\n");
        for (int k= 0; k<10; k++)
        {
            Measurement pipelinetimemessArr[1] = {
            {messArr[k].startT, messArr[k].endT}  
                };
            
            create_json_payload(pipelinetimemessArr[0], messArr[k].maxT, messArr[k].minT, messArr[k].avgT, json_payload);
            
            printf("\nJSON Payload: %s\n", json_payload);

            response_code = send_http_post(url, json_payload);

            if (response_code != 200)
            {
                printf ("\nTransmit %d / 10: Alternate address retuned: %d\n", k, response_code);
            }
        }
        
    }

}
/**********TEST CODE********************
 * 
 *         // response_code = send_http_post(url, json_payload);

    // printf("\nhttp post response: %d", response_code );
 *   
    //DONE: make JSON format of data to transmit
    / *```
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
    ``` * /
 * 
 *     //char startTime[] = "Start time";
 *     //char startTime[] = "Start time";
    // time_t time_start;
    // time_start = time(NULL);
    // printf("Seconds since January 1, 1970 = %ld\n", time_start);
    //printf("\n%d-%d-%dT%d:%d:%dZ", tm_sec );
    time_t anytime;
    struct tm *current;
    char time_str[128];

    time(&anytime);

    current = localtime(&anytime);
    strftime(time_str, 128, "%Y-%m-%dT%H:%M:%SZ", current);
    printf("Today is %s\n", time_str);


//    char startTime[] = (char) time_start;
  //  messArr[0].startT = &startTime;
    //printf ("/n %s", messArr[0].startT);
    
    while(1);
 * 
 *     //char request[] = "POST /mes/rest/r2Label/print/Biesse/105/1B6D0X-----11225105/112251/Q0161B8 HTTP/1.1\r\nHost: meswebapptest.hermanmiller.com\r\nContent-Lenght:9\r\n\r\nname=nina";

 * 
 */

//DONE: Read time and make ISO:8601 format
void getcurrenttime (char* timestring)
{
    time_t anytime;
    struct tm *current;

    time(&anytime);
    current = localtime(&anytime);
    strftime(timestring, 128, "%Y-%m-%dT%H:%M:%SZ", current);
}
/*************TEST CODE***********
 * 
 *     //char start_time[128];
 *     //printf("Today is %s\n", timestring);

    //return &timestring;

    //while(1);

 * 
 * 
 * 
 */

volatile char start_time[128];

volatile char end_time[128];

double convertTemptodegC (double);
double getTemperature(int);

int main()
{
    static double rawTemperature = 0;

    static double gotTemperature = 0;

    static double minTemperature = 5000; //arbitary big number. DROPPED: Read first value from file

    static double maxTemperature = 0;

    static double avgTemperature = 0;

    static double temperture_table[(UPLOAD_INTV_MS/READ_DELAY_MS) +1];

    static int temperature_file_line_count = 0;

    static int time_elapsed = 0;
    
    printf("Start!");

    getcurrenttime(&start_time);

    while (1)
    {
        //****************read temperature from file (increment line number on every read)
        rawTemperature = getTemperature(temperature_file_line_count++);

       //DONE: convert read value to celcius:
        gotTemperature = convertTemptodegC(rawTemperature);

        //****************display_read_reading(gotTemperature);
        printf("\n temperature read: %.02f deg C.", gotTemperature);
        
        //****************Calculate max, min and average temperature
        if (gotTemperature < minTemperature) minTemperature = gotTemperature;

        if (gotTemperature > maxTemperature) maxTemperature = gotTemperature;
        
        avgTemperature = avgTemperature + gotTemperature;
        
        printf (". time elapsed: %d / %d.", time_elapsed, UPLOAD_INTV_MS/READ_DELAY_MS);
        //printf("\n maxT, minT, avgT: %.02f, %.02f, %.02f", maxTemperature, minTemperature, avgTemperature);

        if (time_elapsed >= (UPLOAD_INTV_MS/READ_DELAY_MS))
        {
            //read end time
            getcurrenttime(&end_time);

            time_elapsed = 0;
            printf("\n two minutes completed");

            avgTemperature = avgTemperature / ((UPLOAD_INTV_MS/READ_DELAY_MS) * 1.0);
            //BUG: average value above.

            make_and_send_values(&start_time, &end_time, maxTemperature, minTemperature, avgTemperature);

            //printf("\n maxT, minT, avgT: %.02f, %.02f, %.02f", maxTemperature, minTemperature, avgTemperature);
            
            minTemperature = gotTemperature;
            maxTemperature = gotTemperature;
            avgTemperature = gotTemperature;
            
            //read start time
            getcurrenttime(&start_time);
        }

        //**************** wait 100 ms
        delay_ms(READ_DELAY_MS);  
      
        time_elapsed++;
    }

    return 0;
}

/****************TEST CODES****************
 * 
 * 
 * 
 * 
 * // messArr[0].startT = &start_time;
            // messArr[0].endT = &end_time;

            // printf ("\n Start time:  %s", messArr[0].startT);
            // printf ("\n End time:  %s", messArr[0].endT);
 * 
 *         if (time_elapsed >= 3)
        {
 * 
 * 
 * 
 * 
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

***************DISCARDED CODE************
        //**************** store value for calculation and reporting.
//        temperture_table[time_elapsed] = gotTemperature;
 *
 *
 */