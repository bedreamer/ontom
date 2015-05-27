#include <stdio.h>

static unsigned int __atoh(const char *hex)
{
    unsigned int v = 0;

    if ( !hex ) return 0;
    if ( hex[0] == '0' && (hex[1] == 'X' || hex[1] == 'x' ) )
        hex ++, hex ++;

    while ( *hex ) {
        if (*hex >= '0' && *hex <= '9') {
            v = v * 16 + (*hex) - '0';
        } else if (*hex >= 'A' && *hex <= 'F') {
            v = v * 16 + (*hex) - 'A' + 10;
        } else if (*hex >= 'a' && *hex <= 'f') {
            v = v * 16 + (*hex) - 'a' + 10;
        } else break;
        hex ++;
    }

    return v;
}
#define atoh __atoh

unsigned char BCC_code(unsigned char *da,size_t len) {
    size_t i  = 0;
    unsigned char BCC = 0;

    for ( ; i < len; i ++ ) {
        BCC ^= da[i];
    }
    return ~BCC;
}

int main(int argc, const char *argv[])
{
	int n = 1;
	unsigned char code[1024] = {0};
	size_t len = 0;

	for ( n =1; n < argc; n ++ ) {
		code[ len ++ ] = atoh( argv[n] );
	}
	
	if ( len ) {
		unsigned char bcc, i;
		bcc = BCC_code(code, len);
		code[ len ++ ] = bcc;
		for ( i = 0; i < len; i ++ ) {
			printf("%02X", code[ i ] );
			if ( i != len ) {
				printf(" ");
			} else {
				printf("\n");
			}
		}
		return 0;
	}
	return 1;
}