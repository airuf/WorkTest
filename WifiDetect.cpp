#ifndef UNICODE
#define UNICODE
#endif

#include "WifiDetect.h"

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")


int WifiTest()
{
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	int iRet = 0;

	WCHAR GuidString[39] = { 0 };

	unsigned int i, j, k;

	//variable used for WlanEnumInterfaces 

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
	PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

	int iRSSI = 0;

	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);

	if (dwResult != ERROR_SUCCESS)
	{
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		return 1;
		// can use formatMessage here to find out why the function failed.
	}

	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS)
	{
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		return 1;
		//can use formatMessage here to find out why the function failed
	}
	else
	{
		wprintf(L"Num Entries:%lu\n", pIfList->dwNumberOfItems);
		wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
		for (i = 0; i < (int)pIfList->dwNumberOfItems; i++)
		{
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			wprintf(L" Interface Index [%u]:\t %lu\n", i, i);
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
				sizeof(GuidString) / sizeof(*GuidString));
			if (iRet == 0)
			{
				wprintf(L" StringFromGUID2 failed\n");
			}
			else
			{
				wprintf(L" InterfaceGUID[%d]: %ws\n", i, GuidString);
			}
			wprintf(L" Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription);
			wprintf(L" \n");
			wprintf(L" Interface  State[%d]:\t ", i);
			switch (pIfInfo->isState)
			{
			case wlan_interface_state_not_ready:
				wprintf(L"Not ready \n");
				break;
			case wlan_interface_state_connected:
				wprintf(L"Connected\n");
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				wprintf(L"First node in a ad hoc network");
				break;
			case wlan_interface_state_disconnecting:
				wprintf(L"Disconnecting\n");
				break;
			case wlan_interface_state_disconnected:
				wprintf(L"Not connected\n");
				break;
			case wlan_interface_state_associating:
				wprintf(L"Attempting to associate with a network\n");
				break;
			case wlan_interface_state_discovering:
				wprintf(L"Auto configuration is discovering settings for the network\n");
				break;
			case wlan_interface_state_authenticating:
				wprintf(L"In process of authenticating\n");
				break;

			default:
				wprintf(L"Unknow state %ld\n", pIfInfo->isState);
				break;
			}
			wprintf(L"\n");

			dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, 0, NULL, &pBssList);

			if (dwResult != ERROR_SUCCESS)
			{
				wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
				dwRetVal = 1;
			}
			else
			{
				wprintf(L" WLAN_AVAILABEL_NETWORK_LIST for this interface\n");
				wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);
				for (j = 0; j < pBssList->dwNumberOfItems; j++)
				{
					pBssEntry = (WLAN_AVAILABLE_NETWORK *)&pBssList->Network[j];
					wprintf(L"  Profile Name[%u]: %ws\n", j, pBssEntry->strProfileName);
					wprintf(L"  SSID[%u]:\t\t", j);
					if (pBssEntry->dot11Ssid.uSSIDLength == 0)
					{
						wprintf(L"\n");
					}
					else
					{
						for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++)
						{
							wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
						}
						wprintf(L"\n");
					}
					wprintf(L"  BSS Network type[%u]:\t", j);
					switch (pBssEntry->dot11BssType)
					{
					case dot11_BSS_type_infrastructure:
						wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
						break;
					case dot11_BSS_type_independent:
						wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
						break;
					default:
						wprintf(L"Other (%lu)\n", pBssEntry->dot11BssType);
						break;
					}

					wprintf(L"  Number of BSSIDs[%u]: \t %u \n", j, pBssEntry->uNumberOfBssids);

					wprintf(L"  Connectable[%u]:\t ", j);
					if (pBssEntry->bNetworkConnectable)
					{
						wprintf(L"YES\n");
					}
					else
					{
						wprintf(L"NO\n");
						wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j, pBssEntry->wlanNotConnectableReason);
					}

					wprintf(L"  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes);
					if (pBssEntry->wlanSignalQuality == 0)
					{
						iRSSI = -100;
					}
					else if (pBssEntry->wlanSignalQuality == 100)
					{
						iRSSI = -50;
					}
					else
					{
						iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);
					}

					wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j, pBssEntry->wlanSignalQuality, iRSSI);
					wprintf(L"  Security Enabled[%u]:\t", j);
					if (pBssEntry->bSecurityEnabled)
					{
						wprintf(L"YES\n");
					}
					else
					{
						wprintf(L"NO\n");
					}

					wprintf(L"  Default AuthAlgorithm[%u]: ", j);
					switch (pBssEntry->dot11DefaultAuthAlgorithm)
					{
					case DOT11_AUTH_ALGO_80211_OPEN:
						wprintf(L"802.11 Open (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_80211_SHARED_KEY:
						wprintf(L"802.11 Shared (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_WPA:
						wprintf(L"WPA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_WPA_PSK:
						wprintf(L"WPA-PSK (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_WPA_NONE:
						wprintf(L"WPA-None (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_RSNA:
						wprintf(L"RSNA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					case DOT11_AUTH_ALGO_RSNA_PSK:
						wprintf(L"RSBA with PSK(%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					default:
						wprintf(L"Ither (%lu)\n", pBssEntry->dot11DefaultAuthAlgorithm);
						break;
					}

					wprintf(L"  Default CipherAlgorithm[%u]: ", j);
					switch (pBssEntry->dot11DefaultCipherAlgorithm)
					{
					case DOT11_CIPHER_ALGO_NONE:
						wprintf(L"None (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					case DOT11_CIPHER_ALGO_WEP40:
						wprintf(L"WEP-40 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					case DOT11_CIPHER_ALGO_TKIP:
						wprintf(L"TKIP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					case DOT11_CIPHER_ALGO_CCMP:
						wprintf(L"CCMP(0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					case DOT11_CIPHER_ALGO_WEP104:
						wprintf(L"WEP-104 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					case DOT11_CIPHER_ALGO_WEP:
						wprintf(L"WEP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					default:
						wprintf(L"Other (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
						break;
					}

					wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
					if (pBssEntry->dwFlags)
					{
						if (pBssEntry->dwFlags &WLAN_AVAILABLE_NETWORK_CONNECTED)
							wprintf(L" - Currently connected");
						if (pBssEntry->dwFlags &WLAN_AVAILABLE_NETWORK_CONNECTED)
							wprintf(L" - Has profile");
					}
					wprintf(L"\n");
					wprintf(L"\n");
				}
			}
		}
	}
	if (pBssEntry != NULL)
	{
		WlanFreeMemory(pBssList);
		pBssList = NULL;
	}
		
	if (pBssEntry != NULL)
	{
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}

	return dwRetVal;

}
