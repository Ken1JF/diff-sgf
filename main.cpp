//
//  main.cpp
//  diffsgf
//
//  Created by Ken Friedenbach on 5/31/08.
//  Copyright (c) 2008-2013 Ken Friedenbach. All rights reserved.
//
//  This program takes too file names as arguments,
//  and coompares them byte-for-byte, ignoring all white space.
//
//  It is intended for use in comparing .sgf files.
//
//  The -l option will ignore labels, i.e. SGF LB() attributes.
//  The -q option will produce quiet output, i.e. suppress header & args
//
//  test.txt is a script to run diffsgf using the files:
//      Fuseki.sgf	Fuseki2.sgf	T1.sgf T1_orig.sgf
//  test_output.txt is the expected output from running test.txt
//

#include <iostream>
#include <fstream>
using namespace std;

#define END_DIFFS	10

const char * file1 = "-";
const char * file2 = "-";

int ignore_labels = 0;
int print_args = 1;

static int process_args(int argc, const char * argv[])
{
    int optionArgs = 0;
    while ((argc > (optionArgs+1)) && (argv[optionArgs+1][0] == '-')) {
        optionArgs+=1;
        if (argv[optionArgs][1] == 'l') {
            ignore_labels = 1;
        } else if (argv[optionArgs][1] == 'q') {
            print_args = 0;
        } else {
            printf("Unknown option %s\n", argv[optionArgs]);
        }
    }
	if (argc > (optionArgs+1)) {
        file1 = argv[optionArgs+1];
    }
	if (argc > (optionArgs+2)) {
		file2 = argv[optionArgs+2];
    }
    if (print_args == 1){
        printf("diffsgf program, version 1.0, %s: %d arguments\n", __DATE__, argc);
        if (argc > 0)
            {
            int i;
            for (i=0; i<argc; i++)
                printf("   Arg[%d] = \"%s\".\n", i, argv[i]);
            }
        printf("\n");
    }
	return 0;
}

char trySkipLabels(ifstream *ifs, int *count) {
    char c1, c2;
    char ret = 'L';
    
    c1 = ifs->peek();
    if (c1=='B') {
        *ifs >> c1; *count += 1;
        c2 = ifs->peek();
        if (c2=='[') {
        AnotherLabelPoint:
                // skip to character after ']'
            *ifs >> c2; *count += 1;
            do {
                *ifs >> skipws >> ret; *count += 1;
            } while (ret != ']');
            *ifs >> skipws >> ret; *count += 1;
            if (ret == '[') {
                goto AnotherLabelPoint;
            }
        } else {
            ifs->putback(c1); *count -= 1;
        }
    }
    return ret;
}

int main(int argc, const char * argv[])
{
	int count1, count2 = 0;
	int diffs = 0;
	process_args(argc, argv);
	
	ifstream ifs1 ( file1, ifstream::in );
	if (ifs1.is_open()) {
		ifstream ifs2 ( file2, ifstream::in );
		if (ifs2.is_open()) {
			while (ifs1.good() && ifs2.good()) {
				char ch1, ch2;
				
				ifs1 >> skipws >> ch1;
				ifs2 >> skipws >> ch2;
                count1++; count2++;
				if (ch1 != ch2) {
                    if ((ch1 == 'L') && (ignore_labels == 1)) {
                        ch1 = trySkipLabels(&ifs1, &count1);
                    }
                    if ((ch2 == 'L') && (ignore_labels == 1)) {
                        ch2 = trySkipLabels(&ifs2, &count2);
                    }
                    if (ch1 != ch2) {
                        if (diffs == 0)
                            cout << "Differences in " << file1 << " and " << file2 << endl;
                        diffs++;
                        cout << ch1 << ":" << ch2 << " differ near " << count1 << " and " << count2 << endl;
                    }
				}
                if (diffs > END_DIFFS) break;
			}
			ifs2.close();
		}
		else
			cout << "Error opening file 2: " << file2 << endl;
		ifs1.close();
	}
	else
		cout << "Error opening file 1: " << file1 << endl;
    
	if (diffs > 0)
        {
		if (diffs >END_DIFFS)
			cout << "More than " << END_DIFFS << " differences, in first " << count1 << " and " << count2 << " non-whitespace characters." << endl;
		else
			cout << diffs << " differences, in " << count1 << " and " << count2 << " non-whitespace characters." << endl;
        }
    return 0;
}

