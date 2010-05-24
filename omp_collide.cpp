#include <omp.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

bool blargh(vector<int> *i, vector<int> *j) {
	return( (*i)[0] <= (*j)[0] );
}

bool blargh(int *i, int *j) {
	return( i[0] <= j[0] );
}

void collide(void) {
    vector<int> limits(100000);
    int i;
    int max = 10000;

    for (i = 0; i < limits.size(); i++) {
        limits[i] = rand() % max;
        if ( limits[i] < max / 2 ) {
	        limits[i] = 0;
        }
    }

	int j;
    vector<int> one, two;
	vector<int> *op, *tp;

#pragma omp parallel for private(j, one, two, op, tp)
	for (i = 0; i < limits.size(); i++) {
		one.resize(1);
		two.resize(1);
		one[0] = limits[i];
//		cout << "index " << i << " wants " << limits[i] << endl;
		for (j = 0; j < max; j++) {
			two[0] = j;
			op = &one;
			tp = &two;
			if ( blargh(tp, op) ) {
				if ( one[0] != two[0] ) {
					cout << "Collision! Index " << i << " wanted to iterate "
						<< one[0] << " times, but got "
						<< two[0] << "." << endl;
				}
				break;
			}
		}
	}
}

void no_collide(void) {
    vector<int> limits(100000);
    int i;
    int max = 10000;

    for (i = 0; i < limits.size(); i++) {
        limits[i] = rand() % max;
        if ( limits[i] < max / 2 ) {
            limits[i] = 0;
        }
    }

    int j;

    int one[1], two[1];
    int *op, *tp;

#pragma omp parallel for private(j, one, two, op, tp)
    for (i = 0; i < limits.size(); i++) {
        one[0] = limits[i];
//        cout << "index " << i << " wants " << limits[i] << endl;
        for (j = 0; j < max; j++) {
            two[0] = j;
            op = one;
            tp = two;
            if ( blargh(op, tp) ) {
                if ( one[0] != two[0] ) {
                    cout << "Collision! Index " << i << " wanted to iterate "
                         << one[0] << " times, but got "
                             << two[0] << "." << endl;
                }
                break;
            }
        }
    }
}

int main(void) {
	cout << "collide()" << endl;
	collide();
	cout << "no_collide()" << endl;
	no_collide();

	return(0);
}

