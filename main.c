#include <ntifs.h>
#include <ntddk.h>
#define PID 12212

OB_OPERATION_REGISTRATION op;
OB_CALLBACK_REGISTRATION reg;
PVOID regHandle = NULL;
void DriverUnload(PDRIVER_OBJECT DriverObject)
{
    if (regHandle)
    {
        DbgPrint("DriverUnload -> Callback unloaded");
        ObUnRegisterCallbacks(regHandle);
    }


    UNREFERENCED_PARAMETER(DriverObject);

}

OB_PREOP_CALLBACK_STATUS perCallBack(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
    UNREFERENCED_PARAMETER(RegistrationContext);
    DbgPrint("perCallBack\n");
    ACCESS_MASK access_mask = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
    PEPROCESS targetProcess = (PEPROCESS)OperationInformation->Object;
    if (PsGetProcessId(targetProcess) == (HANDLE)(ULONG_PTR)PID /*|| 1*/)
    {
        DbgPrint("Process Detected\n");
        access_mask = PROCESS_ALL_ACCESS;
        OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = access_mask;
        return OB_PREOP_SUCCESS;
    }
    return OB_PREOP_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    op.ObjectType = PsProcessType;
    op.Operations = OB_OPERATION_HANDLE_CREATE;
    op.PreOperation = &perCallBack;
    op.PostOperation = NULL;

    reg.Version = OB_FLT_REGISTRATION_VERSION; //버전 이거 적어주면됨
    reg.OperationRegistrationCount = 1; //몇개의 OB_OPERATION_REGISTRATION을 등록할건지 
    RtlInitUnicodeString(&reg.Altitude, L"321234.7700a1d");

    reg.RegistrationContext = NULL; //현재 드라이버의 상태를 알려주고 싶다면 
    reg.OperationRegistration = &op;
    DbgPrint("Kernel start\n");
    if (ObRegisterCallbacks(&reg, &regHandle) == OB_PREOP_SUCCESS)
    {
        DbgPrint("Callback Success\n");

    }
    DriverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;

}
