#pragma once

#include <ntifs.h>

#include <ntddk.h>
#include<windef.h>
#include<ntstatus.h>

#include <stdio.h>
#include <stdlib.h>

extern "C" UCHAR * PsGetProcessImageFileName
(
	_In_ PEPROCESS Process
);

typedef struct _LDR_DATA
{
	LIST_ENTRY listEntry;
	ULONG unknown1;
	ULONG unknown2;
	ULONG unknown3;
	ULONG unknown4;
	ULONG unknown5;
	ULONG unknown6;
	ULONG unknown7;
	UNICODE_STRING path;
	UNICODE_STRING name;
	ULONG   Flags;
}LDR_DATA, * PLDR_DATA;

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(
	_In_  PVOID RegistrationContext,
	_In_  POB_PRE_OPERATION_INFORMATION OperationInformation
);


char* GetProcessImageNameByProcessID(
	_In_ ULONG ulProcessID
);
