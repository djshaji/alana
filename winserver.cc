#ifndef __linux__
#include "winserver.h"

asio::thread_pool ioc(1);


void read_session(Presets * p, GtkLabel * status, tcp::socket sock) {
    IN
    LOGD ("Connection established");

    std::array<char, 1000> buffer;
    std::string str ("");

    for (boost::system::error_code ec;;) {
        size_t n = sock.read_some(asio::buffer(buffer), ec);
        LOGD( "Received %d bytes: %s (%s)", n, std::string(buffer.data(), n).c_str (), ec.message().c_str ());

        str.append (std::string(buffer.data(), n));
        if (ec.failed() || str.find("}}}") != string::npos || str.c_str () [1] == '}')
            break;
    }

    if (str.c_str () [1] != '}') {
        while (str.find ("}}}") != std::string::npos)
            str.pop_back () ;
    } else {
        while (str.find ("}}") != std::string::npos)
            str.pop_back () ;        
    }
        
    LOGD ("[server] got request: %s", str.c_str ());
    json j = json::parse (str);
    
    json our = p -> get_all_user_presets ();
    std::string toSendStr = our.dump () ;
    if (our.empty ()) {
        toSendStr = std::string ("{}");
    }
    
    std::string msg = toSendStr.append (string ("}\n")) ;
    LOGD ("[server] send message: %s", msg.c_str ());
    write(sock, asio::buffer(msg));

    int how_many = p -> import_presets_from_json (j);
    char * ss = g_strdup_printf ("<span foreground=\"green\" weight=\"bold\" size=\"x-large\">Imported %d presets successfully</span>", how_many);
    LOGD("[server] %s\n", ss);
    gtk_label_set_markup (status, ss);
    g_free (ss);
    LOGD ("Connection closed");
    OUT
}

void Client::create () {
    IN
    
    OUT
}

Client::Client (string host, int port) {
    IN
    host_ = host ;
    port_ = port ;
    OUT
}

std::string Client::send_preset (json j) {
    IN
    asio::io_service ioc(1);
    std::array<char, 1000> buffer;
    std::string str ("");

    try {
        tcp::socket m_Socket(ioc);
        ///| todo enter hostname here
        boost::asio::ip::tcp::endpoint endpoint(
            boost::asio::ip::address::from_string(host_), 6906);
        
        m_Socket.connect(endpoint);

        LOGD ("Client connected");

        ///| todo: add extra } here
        std::string msg = j.dump ().append (string ("}")) ;
        if (j.empty ())
            msg = string ("{}}");
        LOGD ("[client] send message: %s", msg.c_str ());
        write(m_Socket, asio::buffer(msg));
        std::this_thread::sleep_for(100ms);

        LOGV ("[client] reading server response ...");
        for (boost::system::error_code ec;;) {
            size_t n = m_Socket.read_some(asio::buffer(buffer), ec);
            LOGD( "Received %d bytes: %s (%s)", n, std::string(buffer.data(), n).c_str (), ec.message().c_str ());

            str.append (std::string(buffer.data(), n));
            if (ec.failed() || str.find("}}}") != string::npos || str.c_str () [1] == '}')
                break;
        }

        LOGV (str.c_str ());
        if (str.c_str () [1] != '}') {
            while (str.find ("}}}") != std::string::npos)
                str.pop_back () ;
        } else {
            while (str.find ("}}") != std::string::npos)
                str.pop_back () ;            
        }
    } catch (std::exception& e) {
        LOGD("Exception: %s",e.what());
    }
    
    return str ;
    OUT
}

void Server::close_socket () {
    g_main_loop_quit (main_loop);
    ioc.stop();
    ioc.join();
}

void Client::close_socket () {
    
}

Server::Server () {
}

void Server::run () {
    IN
    TheServer        server(ioc.get_executor(), 6906, status);    
    main_loop = g_main_loop_new (null, true);
    g_main_loop_run (main_loop);
    OUT
}

string getMachineIPAddr()
{
    /* Declare and initialize variables */
    vector<string> adapterName;
    vector<string> adapterDesc;
    vector<string> adapterIpAddr;

    int count = 0;

    // It is possible for an adapter to have multiple
    // IPv4 addresses, gateways, and secondary WINS servers
    // assigned to the adapter. 
    //
    // Note that this sample code only prints out the 
    // first entry for the IP address/mask, and gateway, and
    // the primary and secondary WINS server for each adapter. 

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

    /* variables used to print DHCP time info */
    struct tm newtime;
    char buffer[32];
    errno_t error;
    string err_str = "Err: Exiting function!";

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return err_str;
    }
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return err_str;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            printf("\tAdapter Addr: \t");

            adapterName.push_back(pAdapter->AdapterName);
            adapterDesc.push_back(pAdapter->Description);
            adapterIpAddr.push_back(pAdapter->IpAddressList.IpAddress.String);

            for (i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }
            printf("\tIndex: \t%d\n", pAdapter->Index);
            printf("\tType: \t");
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case MIB_IF_TYPE_ETHERNET:
                printf("Ethernet\n");
                break;
            case MIB_IF_TYPE_TOKENRING:
                printf("Token Ring\n");
                break;
            case MIB_IF_TYPE_FDDI:
                printf("FDDI\n");
                break;
            case MIB_IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case MIB_IF_TYPE_LOOPBACK:
                printf("Lookback\n");
                break;
            case MIB_IF_TYPE_SLIP:
                printf("Slip\n");
                break;
            default:
                printf("Unknown type %ld\n", pAdapter->Type);
                break;
            }

            printf("\tIP Address: \t%s\n",
                pAdapter->IpAddressList.IpAddress.String);
            printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

            printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            printf("\t***\n");

            if (pAdapter->DhcpEnabled) {
                printf("\tDHCP Enabled: Yes\n");
                printf("\t  DHCP Server: \t%s\n",
                    pAdapter->DhcpServer.IpAddress.String);

                printf("\t  Lease Obtained: ");
                /* Display local time */
                error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }

                printf("\t  Lease Expires:  ");
                error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }
            }
            else
                printf("\tDHCP Enabled: No\n");

            if (pAdapter->HaveWins) {
                printf("\tHave Wins: Yes\n");
                printf("\t  Primary Wins Server:    %s\n",
                    pAdapter->PrimaryWinsServer.IpAddress.String);
                printf("\t  Secondary Wins Server:  %s\n",
                    pAdapter->SecondaryWinsServer.IpAddress.String);
            }
            else
                printf("\tHave Wins: No\n");
            pAdapter = pAdapter->Next;
            printf("\n");
        }
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    
    // THIS IS CODE TO ELIMINATE ADAPTERS WITH 'MICROSOFT' OR 'VIRTUAL' 
    // IN THE ADAPTER DESCRIPTIONS
    vector<string>::iterator it = adapterDesc.begin();
    vector<string>::iterator it2 = adapterIpAddr.begin();

    string compareStr1 = "Microsoft";
    string compareStr2 = "Virtual";
    string outAdapterDesc = "";
    string outAdapterIPAddr = "";

    for (it = adapterDesc.begin(), it2 = adapterIpAddr.begin(); it != adapterDesc.end(); it++, it2++)
    {
        printf("*it: %s\n", it->c_str());
        printf("*it2: %s\n", it2->c_str());
         
        size_t found = it->find("Microsoft");
        size_t found2 = it->find("Virtual");

        if ((found == string::npos) || (found2 == string::npos))
        {
            outAdapterDesc = *it;
            outAdapterIPAddr = *it2;
        }
    }

    printf("outAdapterDesc: %s\n", outAdapterDesc.c_str());
    printf("outAdapterIPAddr: %s\n", outAdapterIPAddr.c_str());

    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return outAdapterIPAddr;
}

#endif

