#include <stdio.h>

// these preprocesser definitions make typing/reading the code easier
#define print_rational(x) printf("%d/%d", (x)->numerator, (x)->denominator);
#define MIN(x, y) (x < y) ? x : y
#define MAX(x, y) (x > y) ? x : y
#define DIVIDE_BY_DIVISOR(a,b,c,d,divisor) \
    a /= divisor; \
    b /= divisor; \
    c /= divisor; \
    d /= divisor; \

// used ot format columns in print statement
#define FORMAT_COL 6

// 2^31 - 1
#define INT_MAX 2147483647


// struct rational typedef to rational
// this is the data structure used to represent fractions
typedef struct rational {
    int numerator;
    int denominator;
} rational;


// reads file and organizes each pair of values into a rational array
// -
// args:
// FILE *f: pointer to file
// rational (*arr)[]: reference to rational array
// -
// returns
// nothing, *f data is stored in *arr
void read_file(FILE *f, rational (*arr)[]) {
    int count = 0;
    int loop = 0;
    do {
        if (count % 2 == 0) loop = fscanf(f, "%d", &(*arr + count / 2)->numerator);
        else loop = fscanf(f, "%d", &(*arr + count / 2)->denominator);
        count++;
    } while (loop == 1);
}


// adds rationals represented by {num1, den1}, {num2, den2} adjusting accuracy if numerator or
// denominator > INT_MAX
// -
// args:
// rational *r: pointer to rational that stores the sum
// int num1: numerator of first addend
// int num2: numerator of second addend
// int den1: denominator of first addend
// int den2: denominator of second addend
// - 
// returns:
// nothing, results are stored in *r
void rational_add(rational *r, int num1, int num2, int den1, int den2) {
    int prod1 = 0;
    int prod2 = 0;
    int den_prod = 0;
    int divisor = 2;
    if (num1 != 0 && den2 != 0) {
        while ((INT_MAX / num1) / den2 == 0) {
            DIVIDE_BY_DIVISOR(num1,num2,den1,den2,divisor)
        }
        prod1  = num1 * den2;
    }

    if (num2 != 0 && den1 != 0) {
        while ((INT_MAX / num2) / den1 == 0) {
            DIVIDE_BY_DIVISOR(num1,num2,den1,den2,divisor)
        }
        prod2  = num2 * den1;
    }
    while ((INT_MAX / den2) / den1 == 0) {
        DIVIDE_BY_DIVISOR(num1,num2,den1,den2,divisor)
    }
    den_prod = den1 * den2;
    
    while (INT_MAX - prod1 - prod2 <= 0) {
        prod1 /= 4;
        prod2 /= 4;
        den_prod /= 4;
    }
    
    r->numerator = prod1 + prod2;
    r->denominator = den_prod;
}   


// multiplies two rationals represented by {num1, den1}, {num2, den2}, adjusting accuracy if either the numerator
// or denominator exceed INT_MAX
// -
// args:
// rational *r: pointer to rational that stores product
// int num1: numerator of first multiplier
// int num2: numerator of second multiplier
// int den1: denominator of first multiplier
// int den2: denominator of second multiplier
// - 
// returns:
// nothing, results stored in *r
void rational_multiply(rational *r, int num1, int num2, int den1, int den2) {
    int prod1 = 0;
    int den_prod = 0;
    int divisor = 2;
    if (num1 != 0 && num2 != 0) {
        while ((INT_MAX / num1) / num2 == 0) {
            DIVIDE_BY_DIVISOR(num1,num2,den1,den2,divisor)
        }
        prod1  = num1 * num2;
    }
    while ((INT_MAX / den2) / den1 == 0) {
        DIVIDE_BY_DIVISOR(num1,num2,den1,den2,divisor)
    }
    den_prod = den1 * den2;

    r->numerator = prod1;
    r->denominator = den_prod;
}


// find greatest common divisor of two numbers
// -
// args:
// int n1: num 1
// int n2: num 2
// -
// returns:
// greatest common divisor
// -
// let gcd(n1,n2) be the greatest common denominator of integers n1 and n2:
// therefore 
// n1 = (some constant k1) * gcd(n1, n2)
// n2 = (some constant k2) * gcd(n1, n2)
// due to this, if n3 = n2 % n1 AND n3 != 0
// gcd(n1, n2) = gcd(n3, n1) = gcd(n3, n2)
// then you use a while loop and return the gcd
int gcd(int n1, int n2) {
    if (n1 < 0) n1 *= -1;
    if (n2 < 0) n2 *= -1;
    int max, min;
    while (n1 != 0 && n2 != 0) {
        max = MAX(n1, n2);
        min = MIN(n1, n2);
        max %= min;
        n1 = max;
        n2 = min;
    }
    return (n1 != 0) ? n1 : n2;
}


// simplifies fraction
// - 
// args:
// rational *r: pointer to rational to be simplified
// - 
// returns;
// nothing, *r is simplified in-place
// -
// uses gcd() function (line 141)
void simplify(rational *r) {
    int divisor = gcd(r->numerator, r->denominator);
    r->numerator /= divisor;
    r->denominator /= divisor;
}


// multiplies two rationals and stores the results in the third argument
// - 
// args:
// const rational *r1: first multiplier (doesn't change so const)
// const rational *r2: second multiplier (doesn't change so const)
// rational *product: pointer to rational that stores product
// -
// returns:
// nothing, results stored in *product
// -
// uses rational_multiply() to multiply while avoiding and integer overflow errors (see line 105)
void multiply(const rational *r1, const rational *r2, rational *product) {
    rational_multiply(product, r1->numerator, r2->numerator, r1->denominator, r2->denominator);
    simplify(product);
}


// adds two rationals and stores the results in the third argument
// - 
// args:
// const rational *r1: first multiplier (doesn't change so const)
// const rational *r2: second multiplier (doesn't change so const)
// rational *sum: pointer to rational that stores sum
// -
// returns:
// nothing, results stored in *sum
// -
// uses rational_add() to add while avoiding and integer overflow errors (see line 59)
void add(const rational *r1, const rational *r2, rational *sum) {
    if (r1->denominator == r2->denominator) {
        sum->denominator = r1->denominator;
        sum->numerator = r1->numerator + r2->numerator;
        return;
    }
    rational_add(sum, r1->numerator, r2->numerator, r1->denominator, r2->denominator);
    simplify(sum);
}


// divides two rationals and stores the results in the third argument
// - 
// args:
// const rational *r1: first multiplier (doesn't change so const)
// const rational *r2: second multiplier (doesn't change so const)
// rational *quotient: pointer to rational that stores quotient
// -
// returns:
// nothing, results stored in *quotient
void divide(const rational *r1, const rational *r2, rational *quotient) {
    rational r2_flipped = {r2->denominator, r2->numerator};
    multiply(r1, &r2_flipped, quotient);
    simplify(quotient);
}


// subtracts two rationals and stores the results in the third argument
// - 
// args:
// const rational *r1: first multiplier (doesn't change so const)
// const rational *r2: second multiplier (doesn't change so const)
// rational *difference: pointer to rational that stores difference
// -
// returns:
// nothing, results stored in *difference
void subtract(const rational *r1, const rational *r2, rational *difference) {
    rational r2_minus = {-1 * r2->numerator, r2->denominator};
    add(r1, &r2_minus, difference);
}

int main(int argc, char *argv[]) {
    // open file and read data into rational fractions[size]
    FILE *f = fopen(argv[1], "r");
    printf("\nfile: %s\n\n", argv[1]);

    // first number is array size
    int size;
    fscanf(f, "%d", &size);
    rational fractions[size];

    
    // print rationals in fractions array and print out the sum/average
    printf("rationals:\n[ \n");
    read_file(f, &fractions);
    rational r = {0, 1};
    for (int i = 0; i < size; i++) {
        print_rational(fractions + i);
        if (i < size - 1) printf(", ");
        simplify(fractions + i);
        add(&((rational) {r.numerator, r.denominator}), fractions + i, &r);
        if ((i+1) % FORMAT_COL == 0) printf("\n");
    }
    printf("\n]\n\nsum:\n");
    print_rational(&r);

    r.denominator *= 50;
    simplify(&r);
    printf("\n\naverage:\n");
    print_rational(&r);
    printf("\n\n");
    
    // close file
    fclose(f);
    return 0;
}
