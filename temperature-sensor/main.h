//main header file

#define MS_PER_SECOND   1000
#define MS_PER_MIN      (MS_PER_SECOND * 60)


#define READ_DELAY_MS   100     // Wait before reading new value (in milliseconds)
#define UPLOAD_INTV_MIN 2       // Time to send readings to HTTP REST API (in minutes).
#define UPLOAD_INTV_MS  (UPLOAD_INTV_MIN * MS_PER_MIN)

#define FAILBUFFERLENGTH 10     // size of buffer for error handling


#define JSON_BUFFER_SIZE 512    // JSON buffer size


struct tMessage {
    char *startT;
    char *endT;
    double minT;
    double maxT;
    double avgT;
};

struct tMessage messArr[FAILBUFFERLENGTH];

typedef struct {
    char startTm[128];
    char endTm[128];
} Measurement;

static char json_payload[JSON_BUFFER_SIZE];

static char failed_payload[JSON_BUFFER_SIZE];

const char *url = "http://localhost:5000/api/temperature";

const char *alturl = "http://localhost:5000/api/temperature/missing";

short int failed_transmission = 0;