#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <pthread.h>
#include <wiringPi.h>

#define CONNECTION_PORT 1989
#define MAX_COMMAND_SIZE 20

/*
* When the proper pin is set to "LOW", the relay switch will be configured to have the light on, 
* when the pin is set to "HIGH", the relay will switch the lights off
*/
#define LED_ON 0
#define LED_OFF 1
#define AIR_ON 0
#define AIR_OFF 1

void* instruction_manager_handler();
void* light_control_handler();
int set_pin_to_output(int pin_number);
void log_writer(char* log_level, char* log_message);

const char* LED_activate_command = "LED_ACTIVATE";
const char* LED_deactivate_command = "LED_DEACTIVATE";
const char* AIR_activate_command = "AIR_ACTIVATE";
const char* AIR_deactivate_command = "AIR_DEACTIVATE";

char* log_file = "./log_file.txt";
char message[256]; /* Message container for the logging that will be put in place. */

/* 
* If our socket status is anything other than 0, then our two threads will 
* do any cleanup, stop the vehicle force the program to exit
*/
int socket_status = 0;

pthread_cond_t cond_instruction_received;

typedef struct instruction {
    int instruction_code;
	int pin_number;
    time_t last_modified;
} instruction;

instruction light_manager;
pthread_mutex_t mutex;

int main() {
    int thread_creation_status;
    pthread_t instruction_manager_thread,
        light_controller_thread;

    /* Setup our GPIO pin configuration */
    char* configuration_file_location;

    configuration_file_location = "*"; /* This will eventually be a real file */

    /* Setup our mutex lock for instruction handling */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond_instruction_received, NULL);

    /* Initialize our motor instructions structure */
    light_manager.instruction_code = LED_OFF;
    light_manager.pin_number = 7; /* Set the default pin number for the light controller */
    light_manager.last_modified = time(NULL);

	if(set_pin_to_output(light_manager.pin_number) != 0 || set_pin_to_output(6) != 0) {
		printf("An error occurred while trying to set an output pin\n.");
		return -1;
	}

    /* Fire up our thread so they can get to work */
    thread_creation_status = pthread_create(&instruction_manager_thread,
        NULL, &instruction_manager_handler, NULL);

    if(thread_creation_status != 0) {
        /*printf("An error occurred while creating the Instruction Manager thread.\n");*/
        return -1;
    }

    thread_creation_status = pthread_create(&light_controller_thread,
        NULL, &light_control_handler, NULL);

    if(thread_creation_status != 0) {
        /*printf("An error occurred while creating the Motor Controller thread.\n");*/

        /* 
        * Since the Instruction Manager thread was created successfully
        * at this point, we need to clean it up before ending this execution.
        */
        pthread_exit(&instruction_manager_thread);
        return -1;
    }

    pthread_join(instruction_manager_thread, NULL);
    pthread_join(light_controller_thread, NULL);

    pthread_mutex_destroy(&mutex);

    /* This return should never be reached */
    /*printf("Execution Complete.\n");*/
    return 0;
}

void* instruction_manager_handler( void* arg ) {
    char buffer[MAX_COMMAND_SIZE];
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_size;
    int socketfd, incomingfd;
        
    /* Setup the socket descriptor */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    client_addr_size = sizeof(client_addr);

    /* Initialize our server and client address structures */
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    memset((char*)&client_addr, 0, client_addr_size);
    memset(buffer, 0, MAX_COMMAND_SIZE);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(CONNECTION_PORT);
   
    /* Attempt to bind the socket to the socket descriptor */
    if ( bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    	sprintf(message, "Failed to bind to socket: %i\n", CONNECTION_PORT);
    	log_writer("ERROR", message);
    }
    else {
        sprintf(message, "The socket has been bound on port: %i\n", CONNECTION_PORT);
        log_writer("DEBUG", message);
    }

    ListenForConnection:
        socket_status = listen(socketfd, 1); 

        if(socket_status != 0) {
            /*printf("Failed to listen on port: %i\n", CONNECTION_PORT);*/
            return arg; /* For lack of anything better to return right now */
        }

        while( 1 ) {
            incomingfd = accept(socketfd, (struct sockaddr*)&client_addr, 
                    &client_addr_size);

            /* 
            * If something is wrong with our message, then we will go back 
            * and check our socket connection status.
            */
            if( incomingfd < 0 ) {
               log_writer("ERROR", "Failed to accept incoming socket communication. . . . retrying.\n");
               goto ListenForConnection;
            }

            read(incomingfd, buffer, MAX_COMMAND_SIZE);

            pthread_mutex_lock(&mutex);
            
            /* log the command that was received */
            sprintf(message, "Command received: %s.\n\tPassing control to Controller\n", 
                buffer);
            log_writer("DEBUG", message);

			/*
            * Set the global light_manager structure so light_controller
            * thread can handle it accordingly.
            */
            if(strcmp(buffer, LED_activate_command) == 0) {
                light_manager.instruction_code = LED_ON;
                light_manager.pin_number = 7;
                light_manager.last_modified = time(NULL);
            } else if(strcmp(buffer, LED_deactivate_command) == 0) {
                light_manager.instruction_code = LED_OFF;
                light_manager.pin_number = 7;
                light_manager.last_modified = time(NULL);
            } else if(strcmp(buffer, AIR_activate_command) == 0) {
                light_manager.instruction_code = AIR_ON;
                light_manager.pin_number = 6;
                light_manager.last_modified = time(NULL);
            } else if(strcmp(buffer, AIR_deactivate_command) == 0) {
                light_manager.instruction_code = AIR_OFF;
                light_manager.pin_number = 6;
                light_manager.last_modified = time(NULL);
            } else {
                continue;
            }

            /* TODO: Another else if that support a call to change the active pin would be nice */

            /* give control to the motor_controller thread */
            pthread_cond_signal(&cond_instruction_received);
            pthread_mutex_unlock(&mutex);

            /* clean out the buffer */
            memset(buffer, 0, MAX_COMMAND_SIZE);
            
            log_writer("DEBUG", "Instruction Manager Handler - Waiting for mutex lock.\n");
        }
}

void* light_control_handler( void* arg ) {
    log_writer("DEBUG", "Thread Motor Controller has started successfully.\n");

    pthread_mutex_lock(&mutex);
    while( 1 ) {

		log_writer("DEBUG", "Motor Control Handler - Waiting . . .\n");
		pthread_cond_wait(&cond_instruction_received, &mutex);

		printf("Instruction processing");
		
		log_writer("DEBUG", "Instruction processing");
		digitalWrite(light_manager.pin_number, light_manager.instruction_code);
		log_writer("DEBUG","New instruction has been received and handled. Returning control to listener . . .");

	   /* reset the instruction code */
	   light_manager.instruction_code = -1;
	   pthread_mutex_unlock(&mutex);
    }
}

int set_pin_to_output(int pin_number) {
	/* TODO: if config file exists, then use the specified . . . */

    if(wiringPiSetup() == -1) {
    	/*TODO: Removing. Log this data to a log file and not to stdout*/
        /*printf("An error occured during the wiringPi setup.\n");
        printf("Please be sure to have the WiringPi library installed.\n");*/
        return -1;
    }

    /* setup all pins defined above to be in output mode */
	pinMode(pin_number, OUTPUT);
	
    return 0;
}

/* write out to a log message to the specified log file location (config file?) */
void log_writer(char* log_level, char* log_message) { 
/*inside of some loop, this was called a ton of time and failed each time.*/
    time_t rawtime;
    FILE* file;

    rawtime = time(NULL);
    file = fopen(log_file, "a"); /* open the file so we can append to it */
    
    if (file == NULL) {
        printf("Error opening file");
        return;
    }

    fprintf(file, "(%s) %s: %s\n", asctime(localtime(&rawtime)), log_level, log_message);

    fclose(file);
}
