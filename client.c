#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // for close()
#include <netinet/in.h>     // for sockaddr_in
#include <arpa/inet.h>      // for inet_pton()
#include <openssl/conf.h>   
#include <openssl/evp.h>    // for AES encryption
#include <openssl/err.h>
#include <sys/socket.h>     // for socket functions

#define PORT 8080

void handleErrors(void){
    ERR_print_errors_fp(stderr);
    abort();
}

int aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext){
    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;

    ctx = EVP_CIPHER_CTX_new();
    if(!ctx)
        handleErrors();
    
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext){
    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;

    ctx = EVP_CIPHER_CTX_new();
    if(!ctx)
        handleErrors();
    
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;
    
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

unsigned char key[32] = "01234567890123456789012345678901"; // 256-bit key
unsigned char iv[16] = "122444666660012"; // 128-bit IV

int main(){
    int sock;
    struct sockaddr_in serv_addr;
    unsigned char buffer[1024] = {0};
    unsigned char enc_buffer[1024];
    unsigned char dec_buffer[1024];

    OpenSSL_add_all_algorithms();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server....\n");

    while(1){
        printf("Client: ");
        fgets((char *)buffer, sizeof(buffer), stdin);

        int enc_len = aes_encrypt(buffer, strlen((char *)buffer), key, iv, enc_buffer);
        send(sock, enc_buffer, enc_len, 0);

        if(strncmp((char *)buffer, "bye", 3) == 0)
            break;
        
        int valread = read(sock, enc_buffer, 1024);
        int dec_len = aes_decrypt(enc_buffer, valread, key, iv, dec_buffer);
        dec_buffer[dec_len] = '\0';
        printf("Server: %s\n", dec_buffer);
    }

    close(sock);
    return 0;
}