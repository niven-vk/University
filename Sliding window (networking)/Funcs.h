#ifndef FUNCS_H
#define FUNCS_H
#include <string>
#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
using namespace std;

#define MAX_SLOT_SIZE 1000
#define SLOTS_NR 1000

class Funcs
{
        struct ringSlot
        {
            int start;
            int size;
            bool recv; //Flaga równa true jeżeli wcześniej otrzymaliśmy pakiet
            unsigned char buff[MAX_SLOT_SIZE]; //Pole na pojedynczy pakiet w buforze
        };
    public:
        Funcs();
        virtual ~Funcs();
        void setAll(char* iip, char* iport, char* inazwa, char* irozmiar);
        void resend(void);
        void send(int iter, int rest);
	void print();
        int receive(void);
        void save(void);
        string getIP();
        string getPort();
        string getNazwa();
        int getRozmiar();
        bool finish(void)const;

    protected:

    private:
        static void fillNextSlot(struct ringSlot * next, const struct ringSlot * prev, unsigned int max_size);
        string ip,port,nazwa;
        int rozmiar,sockfd,completion;
        struct sockaddr_in server_address;
        struct ringSlot ringSlots[SLOTS_NR];
        unsigned int ringHead;
        ofstream pFile;
};

#endif // FUNCS_H
