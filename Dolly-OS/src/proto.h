/*
 * proto.h
 *
 *  Created on: Sep 28, 2016
 *      Author: koneca
 */

#ifndef PROTO_H_
#define PROTO_H_

typedef enum _DOLLY_TYPE{
	Dolly_Type_Fwd		= 1,
	Dolly_Type_Rwd		= 2,
	DollyType_Stop		= 3,
	Dolly_Type_Run		= 4,
	Dolly_Type_Shoot	= 5,
	Dolly_Type_Answer	= 6,
}DOLLY_TYPE;

typedef struct _Dolly_Message{
	DOLLY_TYPE			type;
	uint8_t		 		length;
	char 				* value;
}Dolly_Message;

#endif /* PROTO_H_ */

