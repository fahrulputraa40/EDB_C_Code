/*
 * CDB.c
 *
 *  Created on: Oct 9, 2020
 *      Author: fahrul
 */
#include "EDB.h"

// private functions

// low level byte write
void edbWrite(EDB_Handler *handler, unsigned long ee, const byte *p, unsigned int recsize) {
	if (handler->_write_buffer == NULL) {
		for (unsigned int i = 0; i < recsize; i++)
			handler->_write_byte(ee++, *p++);
	} else {//not used
		handler->_write_buffer(ee, p, recsize);
	}
}

void edbRead(EDB_Handler *handler, unsigned long ee, byte* p, unsigned int recsize)
{	if(handler->_read_buffer == NULL){
		for (unsigned i = 0; i < recsize; i++)
		*p++ = handler->_read_byte(ee++);
	}else{//not used
		handler->_read_buffer(ee,p,recsize);
	}
}

// writes EDB_Header
void EDB_writeHead(EDB_Handler *handler)
{
  edbWrite(handler, handler->EDB_head_ptr, EDB_REC handler->EDB_head, (unsigned long)sizeof(struct EDB_Header));
}

// reads EDB_Header
void EDB_readHead(EDB_Handler *handler)
{
  edbRead(handler, handler->EDB_head_ptr, EDB_REC handler->EDB_head, (unsigned long)sizeof(struct EDB_Header));
}

/**************************************************/
// public functions

void EDB__construct(EDB_Handler *handler, EDB_Write_Handler *Write_Handler, EDB_Read_Handler *Read_Handler){
	handler->_read_buffer = NULL;
	handler->_read_byte = Read_Handler;
	handler->_write_buffer = NULL;
	handler->_write_byte = Write_Handler;
}

enum EDB_Status EDB_create(EDB_Handler *handler, unsigned long head_ptr, unsigned long tablesize, unsigned int recsize){
	handler->EDB_head_ptr = head_ptr;
	handler->EDB_table_ptr = sizeof(struct EDB_Header) + handler->EDB_head_ptr;
	handler->EDB_head.flag = EDB_FLAG;
	handler->EDB_head.n_recs = 0;
	handler->EDB_head.rec_size = recsize;
	handler->EDB_head.table_size = tablesize;
	EDB_writeHead(handler);
	if (handler->EDB_head.flag == EDB_FLAG){
	return EDB_OK;
	} else {
	return EDB_ERROR;
	}
}

enum EDB_Status EDB_open(EDB_Handler *handler, unsigned long head_ptr){
	handler->EDB_head_ptr = head_ptr;
	handler->EDB_table_ptr = sizeof(struct EDB_Header) + handler->EDB_head_ptr;
	EDB_readHead(handler);
	return handler->EDB_head.flag == EDB_FLAG ?  EDB_OK : EDB_ERROR;
}

enum EDB_Status EDB_writeRec(EDB_Handler *handler, unsigned long recno, const EDB_Rec rec)
{
  edbWrite(handler, handler->EDB_table_ptr + ((recno - 1) * handler->EDB_head.rec_size), rec, handler->EDB_head.rec_size);
  return EDB_OK;
}

enum EDB_Status EDB_readRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec){
	if (recno < 1 || recno > handler->EDB_head.n_recs)
		return EDB_OUT_OF_RANGE;
	edbRead(handler, handler->EDB_table_ptr + ((recno - 1) * handler->EDB_head.rec_size), rec,
			handler->EDB_head.rec_size);
	return EDB_OK;
}

enum EDB_Status EDB_deleteRec(EDB_Handler *handler, unsigned long recno){
	if (recno < 0 || recno > handler->EDB_head.n_recs)
		return  EDB_OUT_OF_RANGE;
	#ifdef EDB_USE_MALLOC
	  EDB_Rec rec = (byte*)malloc(handler->EDB_head.rec_size);
	#else
	  byte recArray[handler->EDB_head.rec_size];
	  EDB_Rec rec = recArray;
	#endif
	  for (unsigned long i = recno + 1; i <= handler->EDB_head.n_recs; i++)
	  {
		  EDB_readRec(handler, i, rec);
		  EDB_writeRec(handler, i - 1, rec);
	  }

	#ifdef EDB_USE_MALLOC
	  free(rec);
	#endif

	  handler->EDB_head.n_recs--;
	  EDB_writeHead(handler);
	  return EDB_OK;
}

enum EDB_Status EDB_insertRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec){
	if (count(handler) == limit(handler)) return EDB_TABLE_FULL;
	if (count(handler) > 0 && (recno < 0 || recno > handler->EDB_head.n_recs)) return EDB_OUT_OF_RANGE;
	if (count(handler) == 0 && recno == 1) return EDB_appendRec(handler, rec);

#ifdef EDB_USE_MALLOC
	EDB_Rec buf = (byte*)malloc(handler->EDB_head.rec_size);
#else
	byte bufArray[handler->EDB_head.rec_size];
	EDB_Rec buf = bufArray;
#endif

	for (unsigned long i = handler->EDB_head.n_recs; i >= recno; i--)
	{
		EDB_readRec(handler, i, buf);
		EDB_writeRec(handler, i + 1, buf);
	}
	#ifdef EDB_USE_MALLOC
	  free(buf);
	#endif

	  EDB_writeRec(handler, recno, rec);
	handler->EDB_head.n_recs++;
	EDB_writeHead(handler);
	return EDB_OK;
}

enum EDB_Status EDB_updateRec(EDB_Handler *handler, unsigned long recno, EDB_Rec rec){
	if (recno < 0 || recno > handler->EDB_head.n_recs)
		return EDB_OUT_OF_RANGE;
	EDB_writeRec(handler, recno, rec);
	  return EDB_OK;
}

enum EDB_Status EDB_appendRec(EDB_Handler *handler, EDB_Rec rec){
	if (handler->EDB_head.n_recs + 1 > EDB_limit(handler))
		return EDB_TABLE_FULL;
	handler->EDB_head.n_recs++;
	EDB_writeRec(handler, handler->EDB_head.n_recs,rec);
	EDB_writeHead(handler);
  return EDB_OK;
}
unsigned long EDB_limit( EDB_Handler *handler){
	return (handler->EDB_head.table_size - sizeof(struct EDB_Header)) / handler->EDB_head.rec_size;
}
unsigned long EDB_count(EDB_Handler *handler){
	return handler->EDB_head.n_recs;
}

void EDB_clear(EDB_Handler *handler){
	EDB_readHead(handler);
	EDB_create(handler, handler->EDB_head_ptr, handler->EDB_head.table_size, handler->EDB_head.rec_size);
}
