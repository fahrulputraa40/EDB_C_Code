/*
 * CDB.h
 *
 *  Created on: Oct 9, 2020
 *      Author: fahrul
 */

#ifndef EDB_EDB_H_
#define EDB_EDB_H_

#include "stdint.h"
#include "stddef.h"

//to use malloc, uncommect it
//#define EDB_USE_MALLOC

#define EDB_FLAG 0b11011011


typedef unsigned char byte;
struct EDB_Header
{
  byte flag;
  unsigned long n_recs;
  unsigned int rec_size;
  unsigned long table_size;
};
enum EDB_Status {
                          EDB_OK,
                          EDB_ERROR,
                          EDB_OUT_OF_RANGE,
                          EDB_TABLE_FULL
                        };
typedef enum EDB_Status EDB_Response;
typedef byte* EDB_Rec;
typedef void EDB_Write_Handler(unsigned long, const uint8_t);
typedef uint8_t EDB_Read_Handler(unsigned long);
typedef void EDB_Write_Buffer(unsigned long, const byte*, unsigned int);
typedef void EDB_Read_Buffer(unsigned long, byte*, unsigned int);
typedef struct EDB_Handler_Def{
	unsigned long EDB_head_ptr;
	unsigned long EDB_table_ptr;
	EDB_Write_Handler *_write_byte;
	EDB_Read_Handler *_read_byte;
	EDB_Write_Buffer *_write_buffer;
	EDB_Read_Buffer *_read_buffer;
	struct EDB_Header EDB_head;
}EDB_Handler;
#define EDB_REC (byte*)(void*)&

void EDB__construct(EDB_Handler *handler, EDB_Write_Handler *Write_Handler, EDB_Read_Handler *Read_Handler);
enum EDB_Status EDB_create(EDB_Handler *handler, unsigned long head_ptr, unsigned long tablesize, unsigned int recsize);
enum EDB_Status EDB_open(EDB_Handler *handler, unsigned long head_ptr);
enum EDB_Status EDB_readRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec);
enum EDB_Status EDB_deleteRec(EDB_Handler *handler, unsigned long recno);
enum EDB_Status EDB_insertRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec);
enum EDB_Status EDB_updateRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec);
enum EDB_Status EDB_appendRec(EDB_Handler *handler, EDB_Rec rec);
unsigned long EDB_limit( EDB_Handler *handler);
unsigned long EDB_count(EDB_Handler *handler);
void EDB_clear(EDB_Handler *handler);



#endif /* EDB_EDB_H_ */
