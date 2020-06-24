#include "Funcs.h"

Funcs::Funcs()
{
    //ctor
}

Funcs::~Funcs()
{
    close(sockfd);
    pFile.close();
}

void Funcs::setAll(char *iip, char *iport, char *inazwa, char *irozmiar)
{
    completion = 0;
    ip = iip;
    port = iport;
    nazwa = inazwa;
    rozmiar = atoi(irozmiar);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
    }
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(iport));
    inet_pton(AF_INET, iip, &server_address.sin_addr);

    pFile.open(nazwa);
    ringHead = 0;
    ringSlots[0].start = 0;
    ringSlots[0].size = (rozmiar > MAX_SLOT_SIZE) ? MAX_SLOT_SIZE : rozmiar;
    ringSlots[0].recv = false;
    for (size_t s = 1; s < SLOTS_NR; ++s)
        fillNextSlot(&(ringSlots[s]), &(ringSlots[s - 1]), rozmiar);
}

void Funcs::send(int iter, int rest)
{
    string msg = "GET " + to_string(iter) + " " + to_string(rest) + "\n";
    const char *message = msg.c_str();
    ssize_t message_len = strlen(message);
    if (sendto(sockfd, message, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
    }
}

int Funcs::receive(void)
{
    u_int8_t buffer[IP_MAXPACKET + 1];
    socklen_t sender_len = sizeof(server_address);
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 1000;
    int selectResult = select(sockfd + 1, &fdset, NULL, NULL, &time);
    if (selectResult < 0)
    {
        fprintf(stderr, "select error: %s\n", strerror(errno));
        return -1;
    }

    else if (selectResult == 0)
    { //timeout
        return 0;
    }

    else
    {
        ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr *)&server_address, &sender_len);
        if (datagram_len < 0)
        {
            fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
            return -1;
        }
        string equal_DATA;
        istringstream bufferstring((char *)(buffer));
        bufferstring >> equal_DATA;
        assert(equal_DATA == "DATA");
        int start, length;
        bufferstring >> start >> length;

        struct ringSlot *found = NULL;
        for (size_t s = 0; s < SLOTS_NR; ++s)
        {
            if (ringSlots[s].start == start)
            {
                found = &ringSlots[s];
                break;
            }
        }

        if (!found)
        { //odpowiedź na stare zapytanie lub śmiecie
            return 1;
        }

        if (found->recv)
        { //duplikat/takie dane zostały już zapisane
            return 1;
        }

        int endline_pos = 0;
        while (buffer[endline_pos] != '\n')
            endline_pos++;
        memcpy(found->buff, (char *)(buffer + endline_pos + 1), datagram_len - endline_pos - 1); //zapisuje dane do bufora
        found->recv = true;
        return 1;
    }
}

void Funcs::resend(void)
{
    for (size_t s = 0; s < SLOTS_NR; ++s)
        if (!ringSlots[s].recv)
            if (ringSlots[s].size)
                send(ringSlots[s].start, ringSlots[s].size);
}

void Funcs::fillNextSlot(struct ringSlot *next, const struct ringSlot *prev, unsigned int max_size)
{
    next->start = prev->start + prev->size;
    next->size = max_size - next->start; //jezeli wyjdzie zero, to znaczy, ze slot będzie nieaktywny
    if (next->size > MAX_SLOT_SIZE)
        next->size = MAX_SLOT_SIZE;
    next->recv = false;
}

void Funcs::save(void)
{

    while (ringSlots[ringHead].recv)
    {
        pFile.write((const char *)ringSlots[ringHead].buff, ringSlots[ringHead].size);
        unsigned int ringTail = ringHead ? ringHead - 1 : SLOTS_NR - 1;
        fillNextSlot(&(ringSlots[ringHead]), &(ringSlots[ringTail]), rozmiar);
        ringHead++;
        //completion++; //do drukowania ukończenia
        ringHead %= SLOTS_NR;
    }
}

void Funcs::print()
{
    cout << (float)completion / ((float)rozmiar / 100000.0) << "%" << endl;
}
bool Funcs::finish(void) const
{
    return (ringSlots[ringHead].size == 0); //jeżeli nastepny slot jest nieaktywny, to znaczy, że to koniec (pozostałe tez będą niekatywne)
}

string Funcs::getIP()
{
    return ip;
}
string Funcs::getPort()
{
    return port;
}
string Funcs::getNazwa()
{
    return nazwa;
}
int Funcs::getRozmiar()
{
    return rozmiar;
}
