#include <stdio.h>
#include <string.h>
static inline unsigned int __atoh(const char *hex)
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
static inline void mac_public_code(unsigned char *obf, unsigned char *mac)
{
    unsigned char feed_code[12] = {10, 12, 13, 13,
                                   8, 11, 13, 2,
                                   8, 9, 1, 12};
    int i = 0;
    unsigned char maccode[6 + 1] = {0};
    maccode[0] = atoh(mac[0]);
    maccode[1] = atoh(mac[3]);
    maccode[2] = atoh(mac[6]);
    maccode[3] = atoh(mac[8]);
    maccode[4] = atoh(mac[11]);
    maccode[5] = atoh(mac[14]);

    obf[0] = maccode[0] & 0xF0 | feed_code[0];
    obf[1] = (maccode[0] << 4) & 0xF0 | feed_code[1];
    obf[2] = maccode[1] & 0xF0 | feed_code[2];
    obf[3] = (maccode[1] << 4) & 0xF0 | feed_code[3];
    obf[4] = maccode[2] & 0xF0 | feed_code[4];
    obf[5] = (maccode[2] << 4) & 0xF0 | feed_code[5];
    obf[6] = maccode[3] & 0xF0 | feed_code[6];
    obf[7] = (maccode[3] << 4) & 0xF0 | feed_code[7];
    obf[8] = maccode[4] & 0xF0 | feed_code[8];
    obf[9] = (maccode[4] << 4) & 0xF0 | feed_code[9];
    obf[10] = maccode[5] & 0xF0 | feed_code[10];
    obf[11] = (maccode[5] << 4) & 0xF0 | feed_code[11];

    obf[12] = 0;
    while ( i < 12 ) {
        obf[12] += obf[i];
    }
}

static inline void gen_key(unsigned char *key, const unsigned char *mac_pub_key)
{
    int i;
    const unsigned char private_key[13] =
                          {0x62, 0x79, 0x20, 0x6C,
                           0x69, 0x6A, 0x69, 0x65,
                           0x20, 0x32, 0x30, 0x31, 0x35}; //"by lijie 2015"
    const unsigned char hex_map2_bcd[16] = {9, 6, 3, 1,
                                      9, 4, 8, 3,
                                      2, 0, 7, 4,
                                      4, 5, 4, 4};
    unsigned char check_code[13 + 1] ={0};

    for ( i = 0; i < 13; i ++ ) {
        check_code[i] = private_key[i] ^ mac_pub_key[i];
    }

    for ( i = 0; i < 13; i ++ ) {
        key[i] = hex_map2_bcd[check_code[i] % 16 ] + '0';
    }
}

static inline int check_auth(const char *mac, const char *bcdcode)
{
    unsigned char mac_pub_key[13 + 1] = {0};
    unsigned char check_code_bcd[13 + 1] = {0};

    mac_public_code(mac_pub_key, mac);
    gen_key(check_code_bcd, mac_pub_key);

    if ( 0 == strcmp(check_code_bcd, bcdcode) ) return 1;

    return 0;
}

int main(int argc, const char *argv)
{
	enum command {
		INVALID = 0,
		CHECK_KEY,
		CALC_PUBLIC_KEY
	};
	enum command cmd = INVALID;
	unsigned char mac[32] = {0}, key[64] = {0}, pub_key[32] = {0};
	
	if ( argc < 2 ) return 1;
	int i = 1;
	
	for ( i = 1; i < argc; i ++ ) {
		if ( 0 == strcmp("-c", argv[i]) ||
			 0 == strcmp("-check", argv[i]) ) {
			cmd = CHECK_KEY;	
		}
		if ( 0 == strcmp("-g", argv[i]) ||
			 0 == strcmp("-gen", argv[i]) ) {
			cmd = CALC_PUBLIC_KEY;	
		}
		if ( 0 == strcmp("-mac", argv[i]) ) {
			strncpy(mac, argv[i+1], 32);
		}
		if ( 0 == strcmp("-key", argv[i] ) ) {
			strncpy(key, argv[i+1], 64);
		}
	}
	if ( cmd == INVALID ) {
		printf("keygen -{[c|g]|{check}|{gen}} -mac -key\n");
		return 0;
	}
	
	if ( cmd == CHECK_KEY ) {
		if ( mac[0] == 0 ) {
			printf("need give mac address first!\n");
			return 1;
		}
		if ( key[0] == 0 ) {
			printf("need give key first!\n");
			return 1;
		}
		if ( check_auth(mac, key) ) {
			printf("key %s accepted!\n");
			return 0;
		} else {
			printf("key %s dennied!\n");
		}
	} else {
		if ( mac[0] == 0 ) {
			printf("need give mac address first!\n");
			return 1;
		}
		mac_public_code(pub_key, mac);
		gen_key(key, pub_key);
		printf("pubkey: %s, key: %s\n", pub_key, key);
		return 0;
	}

	return 0;
}