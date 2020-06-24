#include "Funcs.h"

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("Usage:\n\t%s IP PORT FILENAME SIZE\n", basename(argv[0]));
        return 4;
    }

    Funcs f;
    f.setAll(argv[1], argv[2], argv[3], argv[4]);
    cout << f.getIP() << " " << f.getPort() << " " << f.getNazwa() << " " << f.getRozmiar() << endl;

    while (!f.finish())
    {
        f.resend(); //wyslij ponownie wszystkie zapytania, na które nie otrzymano odpowiedzi
        for (;;)
        {
            int res = f.receive(); //odczytaj jedną odpowiedź
            if (res <= 0)
                break; //timeout lub błąd (dla błędu być może inaczej zareagować?)
        }
        f.save(); //zapisz wszystkie sąsiadujące z sobą odpowiedzi (może być ich zero)
                  //f.print(); // drukuje procent ukończenia
    }
    return 0;
}
