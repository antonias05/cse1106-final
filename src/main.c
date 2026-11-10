#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

#define LED_PIN 17
#define FILE_NAME "temperature_log.csv"

/*
int main(void){
    int temp[3];
    char warn;

    for (int i = 0; i < 3; i++){
        printf("Enter classroom temp %d: ", i + 1);
        scanf("%d", &temp[i]);
    }

    for (int i = 0; i < 3; i++){
        if (temp[i] < 68 || temp[i] > 76){
            warn = 'W';
        } else {
            warn = 'N';
        }

        if (warn == 'W'){
            printf("%d F    WARNING     ON\n", temp[i]);
        } else {
            printf("%d F    NORMAL     OFF\n", temp[i]);
        }
    }

    

    return 0;
}*/

/* Function to control LED on GPIO 17 */
void setLED(int on) {
    if (on == 1) {
        gpioSetMode(LED_PIN, PI_OUTPUT);
        gpioWrite(LED_PIN, 1);
    } else {
        gpioWrite(LED_PIN, 0);
    }
}

/* Check if temperature is acceptable */
int checkTemperature(float temp, float min, float max) {
    if (temp >= min && temp <= max) {
        return 1;
    } else {
        return 0;
    }
}

/* Save temperature and status to file */
void saveTemperature(float temp, char status[]) {
    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%.1f,%s\n", temp, status);
    fclose(file);
    printf("Temperature saved successfully!\n");
}

/* Calculate average temperature */
void calculateAverage() {
    FILE *file = fopen(FILE_NAME, "r");
    float temp;
    float sum = 0;
    int count = 0;
    char status[20];
    
    if (file == NULL) {
        printf("No temperature records found.\n");
        return;
    }
    
    while (fscanf(file, "%f,%19s", &temp, status) == 2) {
        sum += temp;
        count++;
    }
    fclose(file);
    
    if (count > 0) {
        printf("\nAverage Temperature: %.2f F\n", sum / count);
    } else {
        printf("No temperatures recorded yet.\n");
    }
}

/* Display saved temperatures */
void displayRecords() {
    FILE *file = fopen(FILE_NAME, "r");
    float temp;
    char status[20];
    
    if (file == NULL) {
        printf("No saved records found.\n");
        return;
    }
    
    printf("\n--- Saved Temperature Records ---\n");
    while (fscanf(file, "%f,%19s", &temp, status) == 2) {
        printf("Temperature: %.1f F | Status: %s\n", temp, status);
    }
    fclose(file);
}

/* Main function */
int main() {
    float temperature, minTemp = 68.0, maxTemp = 76.0, newMin, newMax;
    int choice, result;
    char status[20];

    /* Start with LED off */
    setLED(0);

    do {
        printf("\n=================================\n");
        printf(" SMART CLASSROOM COMFORT MONITOR\n");
        printf("=================================\n");
        printf("Acceptable Range: %.1f F - %.1f F\n\n", minTemp, maxTemp);
        printf("1. Enter Temperature\n");
        printf("2. Change Temperature Range\n");
        printf("3. Calculate Average Temperature\n");
        printf("4. Display Saved Records\n");
        printf("5. Exit\n\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            setLED(0);
            return 1;
        }

        switch (choice) {
            case 1:
                printf("\nEnter classroom temperature (F): ");
                if (scanf("%f", &temperature) != 1) {
                    printf("Invalid temperature!\n");
                    setLED(0);
                    return 1;
                }
                
                result = checkTemperature(temperature, minTemp, maxTemp);
                if (result == 1) {
                    strcpy(status, "NORMAL");
                    setLED(0);
                    printf("\nTemperature: %.1f F\n", temperature);
                    printf("System Status: NORMAL\n");
                    printf("Warning LED: OFF\n");
                } else {
                    strcpy(status, "WARNING");
                    setLED(1);
                    printf("\nTemperature: %.1f F\n", temperature);
                    printf("System Status: WARNING\n");
                    printf("Warning LED: ON\n");
                }
                saveTemperature(temperature, status);
                break;

            case 2:
                printf("\nEnter new minimum temperature: ");
                if (scanf("%f", &newMin) != 1) {
                    printf("Invalid input!\n");
                    setLED(0);
                    return 1;
                }
                printf("Enter new maximum temperature: ");
                if (scanf("%f", &newMax) != 1) {
                    printf("Invalid input!\n");
                    setLED(0);
                    return 1;
                }

                if (newMin >= newMax) {
                    printf("Invalid temperature range!\n");
                } else {
                    minTemp = newMin;
                    maxTemp = newMax;
                    printf("Temperature range updated!\n");
                }
                break;

            case 3:
                calculateAverage();
                break;

            case 4:
                displayRecords();
                break;

            case 5:
                gpioTerminate();
                break;

            default:
                printf("\nInvalid choice. Try again.\n");
                break;
        }
    } while (choice != 5);

    return 0;
}