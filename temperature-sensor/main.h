//main header file

#define MS_PER_SECOND   1000
#define MS_PER_MIN      (MS_PER_SECOND * 60)


#define READ_DELAY_MS   100     // Wait before reading new value (in milliseconds)
#define UPLOAD_INTV_MIN 2       // Time to send readings to HTTP REST API (in minutes).
#define UPLOAD_INTV_MS  (UPLOAD_INTV_MIN * MS_PER_MIN)