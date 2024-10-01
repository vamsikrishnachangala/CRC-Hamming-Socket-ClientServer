#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/wait.h>
#include <math.h>
#include "encDec.h"

#define BUFFER_SIZE 300001

void crc_division(char *dividend, const char *divisor, int len_dividend, int len_divisor) {
    for (int i = 0; i <= len_dividend - len_divisor; i++) {
        if (dividend[i] == '1') {
            for (int j = 0; j < len_divisor; j++) {
                dividend[i + j] = (dividend[i + j] == divisor[j]) ? '0' : '1';
            }
        }
    }
}

int is_power_of_two(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int calculate_parity_bit(char *data, int position, int n) {
    int parity = 0;
    for (int i = position - 1; i < n; i += 2 * position) {
        for (int j = i; j < i + position && j < n; j++) {
            parity ^= (data[j] - '0');
        }
    }
    return parity;
}

void hamming_decode(char *encoded_data, int m, char *decoded_data) {
    int error_position = 0;

    for (int i = 0; i < m; i++) {
        if (is_power_of_two(i + 1)) {
            int parity = calculate_parity_bit(encoded_data, i + 1, m);
            error_position += (parity * (i + 1));
        }
    }

    if (error_position > 0 && error_position <= m) {
        encoded_data[error_position - 1] ^= 1;
        printf("Error detected and corrected at position: %d", error_position-1);
    } else if (error_position > m) {
        printf("Error detection failed\n");
    } else {
        printf("No error detected");
    }

    // Extract the original data from the corrected encoded_data
    for (int i = 0, j = 0; i < m; i++) {
        if (!is_power_of_two(i + 1)) {
            decoded_data[j++] = encoded_data[i];
        }
    }
}



void *writer_thread(void *arg);
void *char_a(void *arg);
void *char_e(void *arg);
void *char_i(void *arg);
void *char_o(void *arg);
void *char_u(void *arg);
void *sum_numbers(void *arg);

typedef struct {
    char *buffer;
    int sum;
} ThreadData;

int flag;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    
    

    // Check if port is provided
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Initialize server address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Listen for incoming connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Server listening on port %d...\n", portno);

    // Accept incoming connections and handle them
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        printf("Client connected.\n");

       
 //logic to choose between crc or hamming
        
        read(newsockfd,&flag,sizeof(int));
        //printf("flag value %d\n",flag);


        // Create a new thread to handle the connection
        pthread_t thread_id;
        int *client_sockfd = malloc(sizeof(int));
        *client_sockfd = newsockfd;
        if (pthread_create(&thread_id, NULL, writer_thread, (void *) client_sockfd) != 0) {
            perror("ERROR creating thread");
            exit(1);
        }

        // Detach the thread so that it can run independently
        pthread_detach(thread_id);
    }

    close(sockfd);
    return 0;
}

void *writer_thread(void *arg) {
    int newsockfd = *((int *) arg);
    char buffer[BUFFER_SIZE];
    int n;
    long file_size;

    // Clear the buffer and read data from the socket
    bzero(buffer, BUFFER_SIZE);

    n = read(newsockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

//printf("flag value in writer thread %d\n",flag);

pid_t pid;
pid=fork();

if(pid<0){
    fprintf(stderr,"Fork failed\n");
    //return ;
}

else if(pid==0){
///1st process


///////////////CRC
if(flag==1){
    FILE *fp;
    fp=fopen("read_input_binf.binf","w");
    fputs(buffer,fp);
    fclose(fp);
    char* read_input_binf="read_input_binf.binf";
    off_t offset2=0;
    char* read_encoded_array=NULL;
    int read_encoded_array_size=312;
    int inpdecFd = open("inputdecoded.outf", O_CREAT | O_WRONLY, 0644);
    int count=0;

while ((read_encoded_array = read_file(read_input_binf, &offset2,read_encoded_array_size)) != NULL) {

        count+=1;
        printf("len of read_encoded_array %ld\n",strlen(read_encoded_array));
        int len_binstring=280;
        char crcstring[]="100000100110000010001110110110111";
        int len_crcstring=strlen(crcstring);
        char binstring_with_remainder[313]={'\0'};
        int newlen=strlen(read_encoded_array);
        strcpy(binstring_with_remainder,read_encoded_array);
        //binstring_with_remainder[len_binstring+len_crcstring-1]="\0";


        crc_division(binstring_with_remainder, crcstring, len_binstring + len_crcstring - 1, len_crcstring);

        // Extract the remainder (last len_crcstring - 1 bits) from the modified_binstring
    char remainder2[len_crcstring];
    strncpy(remainder2, &binstring_with_remainder[len_binstring], len_crcstring - 1);
    remainder2[len_crcstring - 1] = '\0';

    //printf("CRC remainder2: %s\n", remainder2);


    // If the result contains only zeros, the verification is successful
    int verification_successful = 1;
    for (int i = 0; i < strlen(remainder2); i++) {
        if (remainder2[i] != '0') { // CHANGE HERE
            verification_successful = 0;
            break;
        }
    }

    if (verification_successful) {
        printf("CRC verification successful: no errors detected in frame %d\n",count);}
    else{
        printf("CRC check failed as error was introduced by user in frame %d\n",count);
        //char errormessage[]="Error detected retransmit again";
        //write(newsockfd, errormessage, strlen(errormessage));
        close(newsockfd);
        return 0;
    }


// Exclude first 24 places and last 32 places
    int start_offset = 24;
    int end_offset = 32;
        // Calculate the length of the new string
    int new_len = strlen(read_encoded_array) - start_offset - end_offset;

    // Create a new char pointer that points to the desired position
    char* new_ptr = read_encoded_array + start_offset;

    // Allocate memory for the new char pointer (new_array)
    char* new_array = (char*) malloc((new_len + 1) * sizeof(char));

    // Copy the characters from the new pointer to the new array
    strncpy(new_array, new_ptr, new_len);

    // Add null terminator to the new array
    //new_array[new_len] = '\0';

            //not considering first 24 bits from encoded array
            char* new_read_encoded_array=new_array;
            // length of string
            int new_encoded_len = strlen(new_read_encoded_array);

            int num_chunks = new_encoded_len / 8;

            for (int i = 0; i < num_chunks; i++) {
            char* read_chunk=malloc(sizeof(char)*9);
            strncpy(read_chunk, new_read_encoded_array + i*8, 8);
            read_chunk[8] = '\0';

            char* binary_without_parity=remove_odd_parity(read_chunk);

            char c=binary_to_ascii(binary_without_parity);
            
            //c=toupper(c);

            write(inpdecFd,&c,sizeof(char));
            free(read_chunk);
            }

            if ((int)strlen(read_encoded_array) < read_encoded_array_size) {
                break;
            }
            free(read_encoded_array);
        }
        close(inpdecFd);

}

else if(flag==2){
///Hamming decoding logic

    FILE *fp;
    fp=fopen("read_input_binf.binf","w");
    fputs(buffer,fp);
    fclose(fp);
    char* read_input_binf="read_input_binf.binf";
    off_t offset2=0;
    char* read_encoded_array=NULL;
    int read_encoded_array_size=289;
    int inpdecFd = open("inputdecoded.outf", O_CREAT | O_WRONLY, 0644);
    int count=0;

while ((read_encoded_array = read_file(read_input_binf, &offset2,read_encoded_array_size)) != NULL) {


            count+=1;
            //not considering first 24 bits
            char* ham_encoded_message=read_encoded_array+24;
            int len_ham_encoded_message=strlen(ham_encoded_message);
            // int p=(int)ceil(log2(len_ham_encoded_message+1));
            // int m=n+p;
            //printf("len of ham_encoded_message %ld\n",strlen(ham_encoded_message));

            char* ham_decoded_message = (char*)calloc(len_ham_encoded_message+1,sizeof(char));

            hamming_decode(ham_encoded_message,len_ham_encoded_message,ham_decoded_message);
            printf(" in frame %d \n",count);

            // if (error_position > 0) {
            //         printf("Error detected in frame %d and corrected at position %d\n", count,error_position);
            //     } else {
            //         printf("No errors detected\n");
            //     }

            //not considering first 18 bits from encoded array
            char* new_read_encoded_array=ham_decoded_message;
            // length of string
            int new_encoded_len = strlen(new_read_encoded_array);

            int num_chunks = new_encoded_len / 8;

            for (int i = 0; i < num_chunks; i++) {
            char* read_chunk=malloc(sizeof(char)*9);
            strncpy(read_chunk, new_read_encoded_array + i*8, 8);
            read_chunk[8] = '\0';

            char* binary_without_parity=remove_odd_parity(read_chunk);

            char c=binary_to_ascii(binary_without_parity);
            
            //c=toupper(c);

            write(inpdecFd,&c,sizeof(char));
            free(read_chunk);
            }

            if ((int)strlen(read_encoded_array) < read_encoded_array_size) {
                break;
            }
            free(read_encoded_array);
            free(ham_decoded_message);
        }
        close(inpdecFd);



}
else{
    FILE *fp;
    fp=fopen("read_input_binf.binf","w");
    fputs(buffer,fp);
    fclose(fp);
    char* read_input_binf="read_input_binf.binf";
    off_t offset2=0;
    char* read_encoded_array=NULL;
    int read_encoded_array_size=280;
    int inpdecFd = open("inputdecoded.outf", O_CREAT | O_WRONLY, 0644);

while ((read_encoded_array = read_file(read_input_binf, &offset2,read_encoded_array_size)) != NULL) {


            //not considering first 18 bits from encoded array
            char* new_read_encoded_array=read_encoded_array+24;
            // length of string
            int new_encoded_len = strlen(new_read_encoded_array);

            int num_chunks = new_encoded_len / 8;

            for (int i = 0; i < num_chunks; i++) {
            char* read_chunk=malloc(sizeof(char)*9);
            strncpy(read_chunk, new_read_encoded_array + i*8, 8);
            read_chunk[8] = '\0';

            char* binary_without_parity=remove_odd_parity(read_chunk);

            char c=binary_to_ascii(binary_without_parity);
            
            //c=toupper(c);

            write(inpdecFd,&c,sizeof(char));
            free(read_chunk);
            }

            if ((int)strlen(read_encoded_array) < read_encoded_array_size) {
                break;
            }
            free(read_encoded_array);
        }
        close(inpdecFd);
}

        ///////////////



        

        FILE *file;
        
        char *contents;

        file=fopen("inputdecoded.outf","rb");
        if(file==NULL){
            printf("Error: Unable to open the file. \n");
            //return 1;
        }

        fseek(file,0,SEEK_END);
        file_size=ftell(file);
        fseek(file,0,SEEK_SET);

        contents=(char *)malloc(file_size+1);
        if(contents==NULL){
            printf("Error:Unable to allocate memoey.\n");
            fclose(file);
            //return 1;
        }

        fread(contents,1,file_size,file);
        contents[file_size]='\0';
        fclose(file);
        printf("File contents:\n%s\n", contents);
    ///////////////






    // Initialize shared data for threads
    ThreadData data = {contents, 0};

    // Create and start threads for capitalizing vowels and summing numbers
    pthread_t a_thread, e_thread, i_thread, o_thread, u_thread, sum_numbers_thread;
    pthread_create(&a_thread, NULL, char_a, (void *)&data);
    pthread_create(&e_thread, NULL, char_e, (void *)&data);
    pthread_create(&i_thread, NULL, char_i, (void *)&data);
    pthread_create(&o_thread, NULL, char_o, (void *)&data);
    pthread_create(&u_thread, NULL, char_u, (void *)&data);
    pthread_create(&sum_numbers_thread, NULL, sum_numbers, (void *)&data);

    // Wait for all threads to finish
    pthread_join(a_thread, NULL);
    pthread_join(e_thread, NULL);
    pthread_join(i_thread, NULL);
    pthread_join(o_thread, NULL);
    pthread_join(u_thread, NULL);
    pthread_join(sum_numbers_thread, NULL);

    // Append sum to the buffer
    char num_str[16];
    sprintf(num_str, "%d", data.sum);
    strcat(contents, num_str);


    int outFd = open("input.outf", O_CREAT | O_WRONLY, 0644);
    write(outFd,contents,strlen(contents));

}

else{
    wait(NULL);
//2nd process

    //Server Encoding
    char* filename = "input.outf";
    off_t offset = 0;
    char* ascii_array = NULL;
    int buffer_size=32;
    int chkFd = open("input.chck", O_CREAT | O_WRONLY, 0644);

    while ((ascii_array = read_file(filename, &offset,buffer_size)) != NULL) {
        
        char binary_array[281]={'\0'};
        char arr1[9]={'\0'};
        char* binary1=int_to_binary(22);
        strcpy(arr1,binary1);
            
            
        strcat(binary_array,arr1);
        
        char arr2[9]={'\0'};
        char* binary2=int_to_binary(22);
        strcpy(arr2,binary2);
           
            
        strcat(binary_array,arr2);
       

        char arr3[9]={'\0'};
        char* binary3=int_to_binary((int)strlen(ascii_array));
        strcpy(arr3,binary3);
            
            
        strcat(binary_array,arr3);
        //printf("only 2222 and length%s\n",binary_array);
        //printf("%ld\n",strlen(binary_array));

        

        //storing framed ascii values into binary strings in binary_array and have null character
        for (int i =0; i < strlen(ascii_array); i++) {
            char* binary = ascii_to_binary(ascii_array[i]);
            binary=add_odd_parity(binary);
            char arr[9]={'\0'};

            strcpy(arr,binary);
            strcat(binary_array,arr);
            //printf(" appending each caharacter%s\n",binary_array);
            //printf("%ld\n",strlen(binary_array));
        }


        write(chkFd,binary_array,280);
        
        //write(child_to_parent_pipe[WRITE_END],binary_array,280);
        //printf("%s\n",binary_array);
        //printf("%ld\n",strlen(binary_array));

        if ((int)strlen(ascii_array) < buffer_size) {
            break;
        }
        free(ascii_array);
    }
    close(chkFd);

    FILE *fp_in;
fp_in = fopen("input.chck", "r");
    if (fp_in == NULL) {
        perror("ERROR opening chck file");
        exit(1);
    }


       // get file size
    fseek(fp_in, 0L, SEEK_END);
    file_size = ftell(fp_in);
    fseek(fp_in, 0L, SEEK_SET);

// read entire file into buffer
    char file_contents[file_size + 1];
    fread(file_contents, sizeof(char), file_size, fp_in);
    file_contents[file_size] = '\0';








    // Write the modified buffer back to the client
    n = write(newsockfd, file_contents, strlen(file_contents));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    //printf("Data sent to client: %s\n", file_contents);

    // Close the connection with the client
    close(newsockfd);

    printf("Client disconnected.\n");


}


    return NULL;
}

#define DEFINE_CAPITALIZE_VOWEL_FUNC(vowel) \
void *char_##vowel(void *arg) { \
    ThreadData *data = (ThreadData *)arg; \
    for (int i = 0; i < strlen(data->buffer); i++) { \
        if (tolower(data->buffer[i]) == #vowel[0]) { \
            data->buffer[i] = toupper(data->buffer[i]); \
        } \
    } \
    return NULL; \
}

DEFINE_CAPITALIZE_VOWEL_FUNC(a)
DEFINE_CAPITALIZE_VOWEL_FUNC(e)
DEFINE_CAPITALIZE_VOWEL_FUNC(i)
DEFINE_CAPITALIZE_VOWEL_FUNC(o)
DEFINE_CAPITALIZE_VOWEL_FUNC(u)

void *sum_numbers(void *arg) {
    // ThreadData *data = (ThreadData *)arg;
    // int sum = 0;
    // for (int i = 0; i < strlen(data->buffer); i++) {
    //     if (isdigit(data->buffer[i])) {
    //         sum += data->buffer[i] - '0';
    //     }
    // }
    // data->sum = sum;
    // return NULL;

    ThreadData *data = (ThreadData *)arg;
    int sum = 0;
    int num = 0;
    for (int i = 0; i < strlen(data->buffer); i++) {
        if (isdigit(data->buffer[i])) {
            num = num * 10 + (data->buffer[i] - '0');
        } else {
            sum += num;
            num = 0;
        }
    }
    sum += num; // Add the last number, if any
    data->sum = sum;
    return NULL;
}