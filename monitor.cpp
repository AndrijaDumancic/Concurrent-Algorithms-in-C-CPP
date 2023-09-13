#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stack>
#include <iostream>
#include <pthread.h>
#include <list>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;

pthread_mutex_t m;
pthread_cond_t cam;
pthread_cond_t kan;
pthread_cond_t mis;
int brojOsobaUCamcu = 0;
int obalaCamca = 1; // 0 -> lijeva obala || 1 -> desna obala || -1 -> putuje
int brojKanibalaUCamcu = 0;
int brojMisionaraUCamcu = 0;
list<string> lijevaObala;
list<string> desnaObala;
list<string> osobeUCamcu;
list<string> lijevaObalaM;
list<string> lijevaObalaK;
list<string> desnaObalaM;
list<string> desnaObalaK;

void print_list(const std::list<std::string> &my_list)
{
    for (auto it = my_list.begin(); it != my_list.end(); ++it)
    {
        std::cout << *it;
        if (std::next(it) != my_list.end())
        {
            std::cout << " ";
        }
    }
}

void *misionar(void *id)
{

    pthread_mutex_lock(&m);

    srand(time(nullptr));
    int obalaMisionara = rand() % 2; // 0 -> lijeva obala || 1 -> desna obala

    int m_id = *((int *)id);
    string misionar = "M" + to_string(m_id);
    if (obalaMisionara == 1)
    {
        desnaObala.push_back(misionar);
        desnaObalaM.push_back(misionar);
    }
    else
    {
        lijevaObala.push_back(misionar);
        lijevaObalaM.push_back(misionar);
    }
    cout << misionar << ": dosao na " << (obalaMisionara == 1 ? "desnu obalu" : "lijevu obalu") << endl;
    cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
    print_list(osobeUCamcu);
    cout << "} LO={";
    print_list(lijevaObala);
    cout << "} DO={";
    print_list(desnaObala);
    cout << "}" << endl
         << endl;

    while (brojOsobaUCamcu == 7 || obalaCamca != obalaMisionara || brojKanibalaUCamcu >= brojMisionaraUCamcu + 2)
    {
        pthread_cond_wait(&mis, &m);
    }

    brojMisionaraUCamcu++;
    brojOsobaUCamcu++;
    pthread_cond_broadcast(&mis);
    string ulazi;
    if (obalaCamca == 1)
    {
        ulazi = desnaObalaM.front();
        desnaObala.remove(ulazi);
        desnaObalaM.pop_front();
        osobeUCamcu.push_back(ulazi);
    }
    else if (obalaCamca == 0)
    {
        ulazi = lijevaObalaM.front();
        lijevaObala.remove(ulazi);
        lijevaObalaM.pop_front();
        osobeUCamcu.push_back(ulazi);
    }
    cout << ulazi << ": ušao u čamac" << endl;
    cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
    print_list(osobeUCamcu);
    cout << "} LO={";
    print_list(lijevaObala);
    cout << "} DO={";
    print_list(desnaObala);
    cout << "}" << endl
         << endl;

    if (brojOsobaUCamcu >= 3)
    {
        pthread_cond_signal(&cam);
    }
    pthread_mutex_unlock(&m);
    return NULL;
}
void *kanibal(void *id)
{
    pthread_mutex_lock(&m);
    srand(time(nullptr));
    int obalaKanibala = rand() % 2; // 0 -> lijeva obala || 1 -> desna obala
    int k_id = *((int *)id);
    string kanibal = "K" + to_string(k_id);
    if (obalaKanibala == 1)
    {
        desnaObala.push_back(kanibal);
        desnaObalaK.push_back(kanibal);
    }
    else
    {
        lijevaObala.push_back(kanibal);
        lijevaObalaK.push_back(kanibal);
    }
    cout << kanibal << ": dosao na " << (obalaKanibala == 1 ? "desnu obalu" : "lijevu obalu") << endl;
    cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
    print_list(osobeUCamcu);
    cout << "} LO={";
    print_list(lijevaObala);
    cout << "} DO={";
    print_list(desnaObala);
    cout << "}" << endl
         << endl;

    while (brojOsobaUCamcu == 7 || obalaCamca != obalaKanibala || (brojKanibalaUCamcu == brojMisionaraUCamcu && brojMisionaraUCamcu > 0))
    {
        pthread_cond_wait(&kan, &m);
    }
    brojKanibalaUCamcu++;
    brojOsobaUCamcu++;
    pthread_cond_broadcast(&kan);

    string ulazi;
    if (obalaCamca == 1)
    {
        ulazi = desnaObalaK.front();
        desnaObala.remove(ulazi);
        desnaObalaK.pop_front();
        osobeUCamcu.push_back(ulazi);
    }
    else if (obalaCamca == 0)
    {
        ulazi = lijevaObalaK.front();
        lijevaObala.remove(ulazi);
        lijevaObalaK.pop_front();
        osobeUCamcu.push_back(ulazi);
    }
    cout << ulazi << ": ušao u čamac" << endl;
    cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
    print_list(osobeUCamcu);
    cout << "} LO={";
    print_list(lijevaObala);
    cout << "} DO={";
    print_list(desnaObala);
    cout << "}" << endl
         << endl;

    if (brojOsobaUCamcu >= 3)
    {
        pthread_cond_signal(&cam);
    }
    pthread_mutex_unlock(&m);
    return NULL;
}

void *camac(void *arg)
{

    while (1)
    {
        pthread_mutex_lock(&m);
        int zadnjaObala = obalaCamca;
        if (brojOsobaUCamcu == 0)
        {
            cout << "C: prazan na " << (obalaCamca == 1 ? "desnoj obali" : "lijevoj obali") << endl;
        }
        cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
        print_list(osobeUCamcu);
        cout << "} LO={";
        print_list(lijevaObala);
        cout << "} DO={";
        print_list(desnaObala);
        cout << "}" << endl
             << endl;

        while (brojOsobaUCamcu < 3)
        {
            pthread_cond_wait(&cam, &m);
        }
        cout << "C: tri putnika ukrcana, polazim za jednu sekundu" << endl;

        cout << "C[" << (obalaCamca == 1 ? "D" : "L") << "]={";
        print_list(osobeUCamcu);
        cout << "} LO={";
        print_list(lijevaObala);
        cout << "} DO={";
        print_list(desnaObala);
        cout << "}" << endl
             << endl;
        pthread_mutex_unlock(&m);
        sleep(1);
        pthread_mutex_lock(&m);
        cout << "C: prevozim s " << (obalaCamca == 1 ? "desne" : "lijeve") << " na " << (obalaCamca == 1 ? "lijevu " : "desnu ") << "obalu: ";
        print_list(osobeUCamcu);
        cout << endl
             << endl;

        obalaCamca = -1;

        pthread_mutex_unlock(&m);
        sleep(2);
        pthread_mutex_lock(&m);

        cout << "C: preveo s " << (zadnjaObala == 1 ? "desne" : "lijeve") << " na " << (zadnjaObala == 1 ? "lijevu " : "desnu ") << "obalu ";
        print_list(osobeUCamcu);
        cout << endl
             << endl;

        osobeUCamcu.clear();

        obalaCamca = 1 - zadnjaObala;
        brojOsobaUCamcu = 0;
        brojKanibalaUCamcu = 0;
        brojMisionaraUCamcu = 0;
        pthread_cond_broadcast(&kan);
        pthread_cond_broadcast(&mis);

        pthread_mutex_unlock(&m);
    }

    return NULL;
}

void *kreiranje_m_i_k(void *arg)
{
    // poljeeeeee
    int kanibal_id = 0;
    int misionar_id = 0;

    pthread_t misionari;
    pthread_t kanibali;

    for (int i = 0; i < 5; i++)
    {
        pthread_create(&misionari, NULL, misionar, &misionar_id); // stvara se misionar
        misionar_id++;

        pthread_create(&kanibali, NULL, kanibal, &kanibal_id); // stvara se kanibal
        kanibal_id++;

        sleep(1);

        pthread_create(&kanibali, NULL, kanibal, &kanibal_id); // stvara se kanibal
        kanibal_id++;

        sleep(1);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(misionari, NULL);
        pthread_join(kanibali, NULL);
        pthread_join(kanibali, NULL);
    }

    return NULL;
}

int main()
{

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cam, NULL);
    pthread_cond_init(&kan, NULL);
    pthread_cond_init(&mis, NULL);

    pthread_t c;
    pthread_t misionar_kanibal;

    pthread_create(&c, NULL, camac, NULL);
    pthread_create(&misionar_kanibal, NULL, kreiranje_m_i_k, NULL);

    pthread_join(c, NULL);
    pthread_join(misionar_kanibal, NULL);
    return 0;
}