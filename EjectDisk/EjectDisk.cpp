#include <iostream>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>

using namespace std;

#define EXIT_IF_FAILS(callString)\
    if(CR_SUCCESS != callString)\
    {\
        cout << "The call " << #callString << " failed." << endl;\
        return false;\
    }

typedef struct
{
    DEVINST id;
    wstring name;
}DEV_INFO;

bool getDiskDevices(PZZWSTR* pBuffer);
vector<DEV_INFO> createDeviceInfoList(PZZWSTR devices);
void eject(DEVINST dev);

int main()
{
    PZZWSTR devices = NULL;
    getDiskDevices(&devices);

    auto list = createDeviceInfoList(devices);

    cout << "==== List of disks ====" << endl;
    for (auto info : list)
    {
        wcout <<"["<<info.id<<"] " << info.name << endl;
    }

    cout << endl << "Type the number of the disk to eject, and press enter:";
    int targetID;
    cin >> targetID;

    DEV_INFO ejectTarget;
    for (auto info : list)
    {
        if (info.id == targetID)
        {
            ejectTarget = info;
            break;
        }
    }

    wcout << "Eject [" << ejectTarget.id << "] " << ejectTarget.name <<"? (Y/n):";
    bool bEject = true;
    while (true)
    {
        auto input = _getch();
        if (input == '\r' || input == 'Y' || input == 'y')
        {
            break;
        }
        else if (input == 'N' || input == 'n')
        {
            bEject = false;
            break;
        }
    }

    if(bEject)
    {
        cout << endl;
        eject(ejectTarget.id);
        cout << endl << "Press any to exit." << endl;
        (void)_getch();
    }

    return 0;
}

bool getDiskDevices(PZZWSTR* pBuffer)
{
    ULONG length;
    auto guid = const_cast<GUID*>(&GUID_DEVINTERFACE_DISK);

    EXIT_IF_FAILS(CM_Get_Device_Interface_List_Size
    (&length, guid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT));

    auto buffer = (PWSTR)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        length * sizeof(WCHAR));

    EXIT_IF_FAILS(CM_Get_Device_Interface_List(guid, NULL,
        buffer, length, CM_GET_DEVICE_INTERFACE_LIST_PRESENT));

    *pBuffer = buffer;
    return true;
}

vector<DEV_INFO> createDeviceInfoList(PZZWSTR devices)
{
    PWSTR currentDevice;
    DEVINST Devinst;
    WCHAR buffer[MAX_PATH];
    DEVPROPTYPE PropertyType;
    ULONG PropertySize;
    CONFIGRET apiResult;

    auto devInfoList = vector<DEV_INFO>();

    for (currentDevice = devices; *currentDevice; currentDevice += wcslen(currentDevice) + 1)
    {
        PropertySize = sizeof(buffer);
        apiResult = CM_Get_Device_Interface_Property(currentDevice, &DEVPKEY_Device_InstanceId,
            &PropertyType, (PBYTE)buffer,
            &PropertySize,
            0);

        apiResult = CM_Locate_DevNode(&Devinst, buffer, CM_LOCATE_DEVNODE_NORMAL);

        PropertySize = sizeof(buffer);
        apiResult = CM_Get_DevNode_Property(Devinst, &DEVPKEY_NAME,
            &PropertyType, (PBYTE)buffer,
            &PropertySize,
            0);

        if (apiResult != CR_SUCCESS)
        {
            continue;
        }

        if (PropertyType != DEVPROP_TYPE_STRING)
        {
            continue;
        }

        DEV_INFO info;
        info.id = Devinst;
        info.name = wstring(buffer);

        devInfoList.push_back(info);
    }

    return devInfoList;
}


void eject(DEVINST dev)
{
    CONFIGRET apiResult;
    DEVINST parent;
    CM_Get_Parent(&parent, dev, NULL);

    PNP_VETO_TYPE vType;
    WCHAR vName[MAX_PATH];
    apiResult = CM_Request_Device_Eject(parent, &vType, vName, MAX_PATH, NULL);

    if (apiResult == CR_SUCCESS)
    {
        cout << "Ejection success." << endl;
    }
    else
    {
        wcout << "Ejection failed." << endl;
        wcout << "Veto type:" << vType << endl;
        wcout << "Veto name:" << vName << endl;
    }
}