#include "driverdef.h"

VOID DriverUnload(PDRIVER_OBJECT pDriver);
PVOID ProcHandle = NULL;

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	OB_CALLBACK_REGISTRATION cr;  
	_OB_OPERATION_REGISTRATION or;
	
	UNREFERENCED_PARAMETER(RegistryPath);

	

	DriverObject->DriverUnload = DriverUnload;
	((PLDR_DATA)DriverObject->DriverSection)->Flags |= 0x20;    //绕过签名检测

	RtlZeroMemory(&or, sizeof(OB_OPERATION_REGISTRATION));
	RtlZeroMemory(&cr, sizeof(OB_CALLBACK_REGISTRATION));
	RtlInitUnicodeString(&cr.Altitude, L"321000");

	cr.Version = OB_FLT_REGISTRATION_VERSION;

	cr.RegistrationContext = NULL;
	cr.OperationRegistrationCount = 1;
	
	or.ObjectType = PsProcessType;
	or.Operations = OB_OPERATION_HANDLE_CREATE;
	or.PreOperation = ObjectPreCallback;

	cr.OperationRegistration = &or;
	
	return ObRegisterCallbacks(&cr, &ProcHandle);
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);

	if (ProcHandle != NULL)ObUnRegisterCallbacks(ProcHandle);
}

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(
	_In_  PVOID RegistrationContext,
	_In_  POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	
	PEPROCESS pEProcess = NULL;

	// 判断对象类型 
	if (*PsProcessType != OperationInformation->ObjectType)
	{
		return OB_PREOP_SUCCESS;
	}


	//获取该进程结构对象的名称
	pEProcess = (PEPROCESS)OperationInformation->Object;
	HANDLE pid = PsGetProcessId(pEProcess);
	char szProcName[256] = { 0 };
	strcpy(szProcName, GetProcessImageNameByProcessID((ULONG)pid));

	if (_stricmp(szProcName, "Demo.exe"))return OB_PREOP_SUCCESS;

	switch (OperationInformation->Operation)
	{
	case OB_OPERATION_HANDLE_DUPLICATE:
		break;

	case OB_OPERATION_HANDLE_CREATE:
	{
		if (OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & 1)
		{
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		}
		break;
	}
	}
	
	return OB_PREOP_SUCCESS;
}

char* GetProcessImageNameByProcessID(
	_In_ ULONG ulProcessID
)
{
	NTSTATUS  Status;
	PEPROCESS  EProcess = NULL;

	Status = PsLookupProcessByProcessId(
		(HANDLE)ulProcessID,
		&EProcess);    //EPROCESS

	//通过句柄获取EProcess
	if (!NT_SUCCESS(Status))
		return FALSE;
	ObDereferenceObject(EProcess);
	//通过EProcess获得进程名称
	return (char*)PsGetProcessImageFileName(EProcess);
}