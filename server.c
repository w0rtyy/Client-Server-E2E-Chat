#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

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
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    unsigned char buffer[1024] = {0};
    unsigned char enc_buffer[1024];
    unsigned char dec_buffer[1024];

    OpenSSL_add_all_algorithms();

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    printf("Server waiting for connections.... \n");

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Client connected!\n");

    while(1){
        int valread = read(new_socket, enc_buffer, 1024);
        if(valread <= 0)
            break;
        
        int dec_len = aes_decrypt(enc_buffer, valread, key, iv, dec_buffer);
        dec_buffer[dec_len] = '\0';
        printf("Client: %s\n", dec_buffer);

        if(strncmp((char *)dec_buffer, "bye", 3) == 0)
            break;
        
        printf("Server: ");
        fgets((char *)buffer, sizeof(buffer), stdin);

        int enc_len = aes_encrypt(buffer, strlen((char *)buffer), key, iv, enc_buffer);
        send(new_socket, enc_buffer, enc_len, 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}