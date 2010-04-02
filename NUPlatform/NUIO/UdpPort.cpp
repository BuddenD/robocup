/*! @file UdpPort.cpp
    @brief Implementation of UdpPort class.

    @author Aaron Wong, Jason Kulk
 
 Copyright (c) 2009 Aaron Wong, Jason Kulk
 
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "UdpPort.h"
#include "NUPlatform/NUSystem.h"
#include "debug.h"
#include "debugverbositynetwork.h"

#include <netdb.h>
#include <errno.h>
using namespace std;

/*! @brief Constructs a udp port on the specified port
 
    The port is setup to always broadcast on the local subnet.

    @param name the name of the network used for debug purposes eg. TeamPort, GameController, Jobs, etc
    @param portnumber the port number the data will be sent and received on
 */
UdpPort::UdpPort(string name, int portnumber): Thread(name, 0)
{
    #if DEBUG_NETWORK_VERBOSITY > 0
        debug << "UdpPort::UdpPort(" << name << ", " << portnumber << ")" << endl;
    #endif
    #ifdef WIN32
        WSADATA wsa_Data;
        int wsa_ReturnCode = WSAStartup(0x101,&wsa_Data);
        if (wsa_ReturnCode != 0)
        {
            debug <<  "WSA ERROR CODE: "<< wsa_ReturnCode << endl;
        }
    #endif
    
    m_port_name = name;
    m_port_number = portnumber;
    
    // We need to get the machine's local ip address.
    // We use gethostname followed by gethostbyname, and save the in_addr in m_local_address
    char hostname[255];
    gethostname(hostname, 255);
    m_host_name = string(hostname);
    struct hostent* host_entry;
    host_entry = gethostbyname(hostname);
    m_local_address = *((struct in_addr*) host_entry->h_addr);
    
    // On the socket as UDP
    if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
        errorlog << "UdpPort::UdpPort(" << m_port_name << "). Failed to create socket file descriptor, errno: " << errno << endl;
    
    // Set the broadcast flag (a) so we receive broadcasted packets, and (b) so we can broadcast if we want to
    #ifdef WIN32
        char broadcastflag = 1;
    #else
        int broadcastflag = 1;
    #endif
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastflag, sizeof(broadcastflag)) == -1)
        errorlog << "UdpPort::UdpPort(" << m_port_name << "). Failed to set socket options, errno: " << errno << endl;
    
    // Construct this address
    m_address.sin_family = AF_INET;                                     // host byte order
    m_address.sin_port = htons(m_port_number);                          // short, network byte order
    m_address.sin_addr.s_addr = INADDR_ANY;                             // automatically fill with my IP
    memset(m_address.sin_zero, '\0', sizeof m_address.sin_zero);
    
    // Construct the target address (we set the address to broadcast on the local subnet by default)
    m_target_address.sin_family = AF_INET;                              // host byte order
    m_target_address.sin_port = htons(m_port_number);                   // short, network byte order
    m_target_address.sin_addr.s_addr = m_local_address.s_addr | 0xFF000000;      // being careful here to broadcast only to the local subnet
    memset(m_target_address.sin_zero, '\0', sizeof m_target_address.sin_zero);
    
    // Bind the socket to this address
    if (bind(m_sockfd, (struct sockaddr *)&m_address, sizeof m_address) == -1)
        errorlog << "UdpPort::UdpPort(" << m_port_name << "). Failed to bind socket, errno: " << errno << endl;

    m_time_last_receive = 0;
    
    pthread_mutex_init(&m_socket_mutex, NULL);
    
    start();
}

/*! @brief Closes the udp port
 */
UdpPort::~UdpPort()
{
    #ifdef WIN32
        closesocket(m_sockfd);
        WSACleanup();
    #else
        close(m_sockfd);
    #endif
    pthread_mutex_destroy(&m_socket_mutex);
}

/*! @brief Run the UDP port's main loop

 */
void UdpPort::run()
{
#if DEBUG_NETWORK_VERBOSITY > 4
    debug << "UdpPort::run(). Starting udpport: " << m_port_name << "'s mainloop" << endl;
#endif
    struct sockaddr_in local_their_addr; // connector's address information
    socklen_t local_addr_len = sizeof(local_their_addr);
    char localdata[10*1024];
    int localnumBytes;
    while(1)
    {
        localnumBytes = recvfrom(m_sockfd, localdata, 10*1024 , 0, (struct sockaddr *)&local_their_addr, &local_addr_len);
        if (localnumBytes != -1)// && local_their_addr.sin_addr.s_addr != m_local_address.s_addr)
        {
            #if DEBUG_NETWORK_VERBOSITY > 0
                debug << "UdpPort::run()." << m_port_number << " Received " << localnumBytes << " bytes from " << inet_ntoa(local_their_addr.sin_addr) << endl;
            #endif
            m_time_last_receive = nusystem->getTime();
            stringstream buffer;
            buffer.write(reinterpret_cast<char*>(localdata), localnumBytes);
            handleNewData(buffer);
            #if DEBUG_NETWORK_VERBOSITY > 4
            debug << "UdpPort::run(). Received " << buffer << endl;
            #endif
        }
    }
    return;
}

/*! @brief Sends a string stream over the network
    @param stream the stream containing the information to be sent over the network
 */
void UdpPort::sendData(const stringstream& stream)
{
    char* data;
    int numbytes;
    numbytes = stream.str().size();
    data = (char*) stream.str().c_str();
    #if DEBUG_NETWORK_VERBOSITY > 4
        debug << "UdpPort::sendData(). Sending " << numbytes << " bytes to " << inet_ntoa(m_target_address.sin_addr) << endl;
    #endif
    pthread_mutex_lock(&m_socket_mutex);
    sendto(m_sockfd, data, numbytes, 0, (struct sockaddr *)&m_target_address, sizeof(m_target_address));
    pthread_mutex_unlock(&m_socket_mutex);
}
