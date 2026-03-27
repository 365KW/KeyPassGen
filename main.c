#include <stdio.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "key_event.h"
 
#define CHARSET_LOWER    "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_UPPER    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_DIGIT    "0123456789"
#define CHARSET_SPECIAL  "!@#$%^&*()_+-=[]{}|;:,.<>?"
#define CHARSET_ALL      CHARSET_LOWER CHARSET_UPPER CHARSET_DIGIT CHARSET_SPECIAL
 
void _setup();
void _main();
char* hash_to_hex(const unsigned char *hash, size_t len);
 
char* hash_to_hex(const unsigned char *hash, size_t len)
{
    if (hash == NULL || len == 0) return NULL;
 
    char *hex_str = malloc(len * 2 + 1);
    if (hex_str == NULL) return NULL;
 
    static const char hex_table[] = "0123456789abcdef";
 
    for (size_t i = 0; i < len; i++) {
        hex_str[i*2]   = hex_table[hash[i] >> 4];
        hex_str[i*2+1] = hex_table[hash[i] & 0x0F];
    }
    hex_str[len*2] = '\0';
 
    return hex_str;
}
 int generate_password(const unsigned char *hmac_result, size_t hmac_len,
                      char *password, size_t pass_len,
                      const char *salt, size_t salt_len)
{
    if (hmac_result == NULL || password == NULL || pass_len == 0) 
        return -1;
 
    size_t charset_len = strlen(CHARSET_ALL),hmac_index = 0,salt_index = 0;
    
    for (size_t i = 0; i < pass_len; i++)
     {
       unsigned char byte1 = hmac_result[hmac_index % hmac_len],
	 byte2 = (salt && salt_len > 0) ? (unsigned char)salt[salt_index % salt_len] : 0,
	 position = (unsigned char)i,
         mixed = byte1 ^ byte2 ^ position;
       mixed = (mixed * 31 + 17) ^ (byte2 << 2),
	 mixed = (mixed + hmac_index) % 256,
         password[i] = CHARSET_ALL[mixed % charset_len];
        
       hmac_index = (hmac_index + 3) % hmac_len,
       salt_index++;
    }
    
    password[pass_len] = '\0';
    return 0;
}
int ensure_char_types(char *password, size_t length)
{
    if (password == NULL || length < 4) return -1;
    
    size_t indices[4] = {0, 0, 0, 0};
    srand((unsigned int)time(NULL) ^ (unsigned long)password);
    indices[0] = rand() % length,
    password[indices[0]] = CHARSET_UPPER[rand() % strlen(CHARSET_UPPER)];
    indices[1] = rand() % length;
    
    while (indices[1] == indices[0]) indices[1] = rand() % length;
    
    password[indices[1]] = CHARSET_LOWER[rand() % strlen(CHARSET_LOWER)];
    indices[2] = rand() % length;
    
    while (indices[2] == indices[0] || indices[2] == indices[1]) 
        indices[2] = rand() % length;
    
    password[indices[2]] = CHARSET_DIGIT[rand() % strlen(CHARSET_DIGIT)];
    indices[3] = rand() % length;
    
    while (indices[3] == indices[0] || indices[3] == indices[1] || 
           indices[3] == indices[2]) 
        indices[3] = rand() % length;
    
    password[indices[3]] = CHARSET_SPECIAL[rand() % strlen(CHARSET_SPECIAL)];
    return 0;
}

void _setup()
{
    _main();
}
 
void _main()
{
    const int key_size = 250;
    const int key_words_size = 800;
    int total_keys = 0;
 
    int *pressed_keys = (int *)malloc(key_size * sizeof(int));
    if (pressed_keys == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
 
    ke_init();
    printf("\n📝 Press any key to record keystrokes\n");
    printf("   Press ESC to finish recording...\n\n");
 
    while (total_keys < key_size)
    {
        if (ke_kbhit()) {
            int ch = ke_readch();
            printf("   Key: 0x%02X (%c)\n", ch, (ch >= 32 && ch <= 126) ? ch : '.');
            pressed_keys[total_keys] = ch;
            total_keys++;
 
            if (ch == 27) break;
        }
        usleep(10000);
    }
    ke_close();
 
    printf("\n🔐 Enter your secret key string: ");
    char *keyws = (char*)malloc(key_words_size * sizeof(char));
    if (keyws == NULL)
    {
        free(pressed_keys);
        return;
    }
    scanf("%799s", keyws);
 
    unsigned char key[32];
    SHA256((const unsigned char*)keyws, strlen(keyws), key);
    free(keyws);
 
    unsigned char hmac_result[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), key, 32,
         (const unsigned char*)pressed_keys,
         total_keys * sizeof(int),
         hmac_result, NULL);
 
    free(pressed_keys);
    char *hex_result = hash_to_hex(hmac_result, SHA256_DIGEST_LENGTH);
    if (hex_result) 
        free(hex_result);
    
    int password_length;
    printf("\n🔢 Enter desired password length (8-128): ");
    scanf("%d", &password_length);
    
    if (password_length < 8) password_length = 8;
    if (password_length > 128) password_length = 128;
    
    printf("🔒 Enter custom salt (optional, press Enter to skip): ");
    char salt[256] = {0};
    getchar();
    fgets(salt, sizeof(salt), stdin);
    salt[strcspn(salt, "\n")] = '\0';
   
    char *password = malloc(password_length + 1);
    if (password == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    
    if (generate_password(hmac_result, SHA256_DIGEST_LENGTH,
                          password, password_length,
                          salt, strlen(salt)) == 0) {
        ensure_char_types(password, password_length);
        
        printf("Generated: %s\n", password);
    }
    
    free(password);
}
 
int main()
{
    _setup();
    return 0;
}
