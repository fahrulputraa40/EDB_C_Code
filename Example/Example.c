/*
 * Example.c
 *
 *  Created on: Oct 9, 2020
 *      Author: fahrul
 */
#if 0

#define TABLE_SIZE 8192

// The number of demo records that should be created.  This should be less
// than (TABLE_SIZE - sizeof(EDB_Header)) / sizeof(LogEvent).  If it is higher,
// operations will return EDB_OUT_OF_RANGE for all records outside the usable range.
#define RECORDS_TO_CREATE 10

char kk[60];
FRESULT result;
FATFS fs;
FIL file;

const char string[50] = "Hallo world\0";
uint16_t written = 0, readlen;
FILINFO fno;
EDB_Handler edb;

void writer (unsigned long address, byte data) {

	f_lseek(&file, address);
	FRESULT f = f_write(&file, &data, 1, &written);
	f_sync(&file);
	 HAL_Delay(1);
//	sprintf(kk, "Write Address: %ld: %d, res: %d\n", address, data, f);
//	 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
//	 HAL_Delay(100);
}

unsigned char reader (unsigned long address) {
	byte b = '\0';
	f_lseek(&file, address);
	FRESULT f = f_read(&file, &b, 1, &readlen);
	 HAL_Delay(1);
//	sprintf(kk, "Read Address: %ld: %d, res: %d\n", address, b, f);
//					 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
//					 HAL_Delay(100);
    return b;
}

void printError(enum EDB_Status err)
{
	sprintf(kk, "Error: ");
		HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
    switch (err)
    {
        case EDB_OUT_OF_RANGE:
        	sprintf(kk, "Out Range.\n");
        		HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
            break;
        case EDB_TABLE_FULL:
        	sprintf(kk, "Table Full.\n");
        		HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
            break;
        case EDB_OK:
        default:
        	sprintf(kk, "OK\n");
        		HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
            break;
    }
}

struct LogEvent {
    int id;
    int temperature;
}
logEvent;

void createRecords(int num_recs)
{
	sprintf(kk, "Create Record.");
	HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
    for (int recno = 1; recno <= num_recs; recno++)
    {
        logEvent.id = recno;
        logEvent.temperature = 100;
        enum EDB_Status result = EDB_appendRec(&edb, EDB_REC logEvent);
        if (result != EDB_OK) printError(result);
    }
    sprintf(kk, " Done\n");
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
}

void selectAll()
{
    for (int recno = 1; recno <= EDB_count(&edb); recno++)
    {
        enum EDB_Status result = EDB_readRec(&edb, recno, EDB_REC logEvent);
        if (result == EDB_OK)
        {
        	sprintf(kk, "Data no: %d, ID: %d: %d\n", recno, logEvent.id, logEvent.temperature);
        	HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
        	HAL_Delay(100);
        }
        else
        	printError(result);
    }
}

int test(){
	EDB__construct(&edb, &writer, &reader);

	result = f_open(&file, "data.db", FA_READ | FA_WRITE);
	if(result == FR_OK){
	 if(EDB_open(&edb, 0) == EDB_OK){
			 sprintf(kk, "Open Done\n");
			HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
	 }
	 else{
		 sprintf(kk, "Open Failed. Create...\n");
		 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);

		 if(EDB_create(&edb, 0, TABLE_SIZE, RECORDS_TO_CREATE) == EDB_OK){
			 sprintf(kk, "Create Done\n");
			  HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
		 }
		 else{
			 sprintf(kk, "Create Failed\n");
			 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
		 }
	 }
	 sprintf(kk, "1. Limit: %ld, Count: %ld\n",EDB_limit(&edb), EDB_count(&edb));
	 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
	 createRecords(5);
	 HAL_Delay(100);
	 sprintf(kk, "2. Limit: %ld, Count: %ld\n",EDB_limit(&edb), EDB_count(&edb));
	 HAL_UART_Transmit(&hlpuart1, (uint8_t *)kk, strlen(kk), 100);
	 selectAll();
 }
	while(1){

	}
}

#endif

