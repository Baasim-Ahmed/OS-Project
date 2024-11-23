#include <iostream>
#include <queue>
#include <pthread.h>
#include <cstring>
#include <ctime>
#include <cstdlib>


using namespace std;

int frame, n;
int *arr;

int PF[3]={0}; // page faults
double algoTime[3]={0};

void *FIFO(void *arg) {
        clock_t initial,final;
        initial=clock();
        queue<int> PF_fifo;
        bool flag = false; // page fault or not

        // Initialize the queue with sentinel values
        for (int s = 0; s < frame; s++) {
                PF_fifo.push(9999);
        }

        for (int i = 0; i < n; i++) {
                int pag;
                flag = false;
                for (int j = 0; j < frame; j++) {
                        pag = PF_fifo.front();
                        PF_fifo.pop();
                        if (pag == arr[i])
                        {
                                flag = true; // no page folt
                        }
                        PF_fifo.push(pag);
                }
                if (!flag) {
                        PF_fifo.pop();
                        PF_fifo.push(arr[i]);
                        PF[0]++;  // fifo
                }
        }

        final=clock();
        algoTime[0]=double(final-initial)/CLOCKS_PER_SEC * 1000;
        return nullptr;
}

void  *Optimal(void *arg)
{
        clock_t initial,final;
        initial=clock();
        int opt[frame];

        for (int i = 0; i < frame; ++i) {
        opt[i] = -1;
    }

    for (int i = 0; i < n; ++i) {
        bool flag = false;
        for (int j = 0; j < frame; ++j) {// if alrady in
            if (opt[j] == arr[i]) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            int temp[frame]; // Temporary array to count distances
            for (int j = 0; j < frame; ++j) {
                temp[j] = 0;
            }

            // Calculate the distances for each page in the frame
            for (int j = 0; j < frame; j++) {
                int k;
                for (k = i + 1; k < n; k++) {
                    if (opt[j] == arr[k]) {
                        break;
                    }
                    temp[j]++;
                }
                if (k == n) {
                    temp[j] = 99999;
                }
            }

            // Find the max page
            int maxIndex = 0;
            for (int j = 1; j < frame; j++) {
                if (temp[j] > temp[maxIndex]) {
                    maxIndex = j;
                }
            }
            // max page rep
            opt[maxIndex] = arr[i];
            PF[1]++;
        }
        final=clock();
        algoTime[1]=double(final-initial)/CLOCKS_PER_SEC * 1000;
    }
    return nullptr;
}

void  *Lru(void *arg) {
         clock_t initial,final;
        initial=clock();
        queue<int> PF_fifo;
        bool flag = false; // page fault or not

        // from this we preserve size of array
        for (int s = 0; s < frame; s++) {
                PF_fifo.push(9999);
        }

        for (int i = 0; i < n; i++) {
                int pag, existing;
                flag = false;
                for (int j = 0; j < frame; j++) {
                        pag = PF_fifo.front();
                        PF_fifo.pop();
                        if (pag == arr[i])
                        {
                                existing=arr[i];
                                flag = true;
                        }
                        else
                        {
                                PF_fifo.push(pag);
                        }
                }
                if (!flag) {
                        PF_fifo.pop();
                        PF_fifo.push(arr[i]);
                        PF[2]++;
                }
                else
                {
                        PF_fifo.push(existing);
                }
        }
        final=clock();
        algoTime[2]=double(final-initial)/CLOCKS_PER_SEC * 1000;
        return nullptr;
}



int main()
{

        srand(static_cast<unsigned>(time(0)));
        cout << "Welcome to page fault counter " << endl << endl;
        cout << "Enter number of pages: ";
        cin >> frame;
        while(1){
                if(frame<=0){
                        cin >> frame;
                }
                else{
                        break;
                }
        }
        cout << "Enter total number of pages that want to load: ";
        cin >> n;
        arr = new int[n];

        for (int i = 0; i < n; i++) {
                cin >> arr[i];
        }

        pthread_t id[3];

        pthread_create(&id[0],NULL,FIFO, NULL);
        pthread_create(&id[1],NULL,Optimal, NULL);
        pthread_create(&id[2],NULL, Lru , NULL);
        for (int i = 0; i < 3; i++){
                pthread_join(id[i], NULL);
        }
        cout << endl << endl;
        cout << "Total page faults using FIFO are " << PF[0] << endl;
        cout << "Total page faults using Optimal are " << PF[1] << endl;
        cout << "Total page faults using LRU are " << PF[2] << endl;
        cout<<"\t\tTotal time taken bt Algos"<<endl;
        cout<<"Time taken by FIFO is: "<<algoTime[0]<<" ms"<<endl;
        cout<<"Time taken by Optimal is: "<<algoTime[1]<<" ms"<<endl;
        cout<<"Time taken by LRU is: "<<algoTime[2]<<" ms"<<endl;
        delete[] arr;

        return 0;
}